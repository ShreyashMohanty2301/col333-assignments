#include "solver.h"
#include <iostream>
#include <chrono>
#include <bits/stdc++.h>
using namespace std;
#include <random>
#define pb push_back
// You can add any helper functions or classes you need here.
using namespace chrono;
/**
 * @brief The main function to implement your search/optimization algorithm.
 * * This is a placeholder implementation. It creates a simple, likely invalid,
 * plan to demonstrate how to build the Solution object. 
 * * TODO: REPLACE THIS ENTIRE FUNCTION WITH YOUR ALGORITHM.
 */

using Item = pair<double, pair<Solution, ProblemData>>;

struct CompareByFirst {
    bool operator()(const Item &a, const Item &b) const {
        return a.first < b.first;  // only compare first element
    }
};


// double h_val(Solution &sol,const ProblemData &problem){
//     double tot = 0.0;
//     vector<vector<int>>villagedrop(problem.villages.size(),vector<int>(3,0));
//     double Cost = 0.0;
//     for(auto plan:sol){
//         for(auto trip:plan.trips){
//             double dist = 0.0;
//             for(int i = 0;i<(int)trip.drops.size();i++){
//                 if(i == 0){
//                     dist+=distance(problem.cities[problem.helicopters[plan.helicopter_id-1].home_city_id-1],problem.villages[trip.drops[i].village_id-1].coords);
//                 }
//                 else{
//                     dist+=distance(problem.villages[trip.drops[i-1].village_id-1].coords,problem.villages[trip.drops[i].village_id-1].coords);
//                 }
//             }
//             if(trip.drops.size()>0){
//                 dist+=distance(problem.villages[trip.drops[trip.drops.size()-1].village_id-1].coords,problem.cities[problem.helicopters[plan.helicopter_id-1].home_city_id-1]);}
//             double cost = problem.helicopters[plan.helicopter_id-1].fixed_cost + problem.helicopters[plan.helicopter_id-1].alpha*dist;
//             Cost+=cost;
//             // double val = 0.0;
//             // for(auto drop:trip.drops){ 
//             //     val+=min(9*problem.villages[drop.village_id].population - min(9*problem.villages[drop.village_id].population,drop.perishable_food),drop.dry_food)*problem.packages[0].value + problem.packages[1].value*min(9*problem.villages[drop.village_id].population,drop.perishable_food) + problem.packages[2].value*min(drop.other_supplies,problem.villages[drop.village_id].population);
//             // }
//             for(auto drop : trip.drops){
//                 villagedrop[drop.village_id-1][0]+=drop.dry_food;
//                 villagedrop[drop.village_id-1][1]+=drop.perishable_food;
//                 villagedrop[drop.village_id-1][2]+=drop.other_supplies;
//             }
//         }  
//     }
//     for(int i = 0;i<(int)villagedrop.size();i++){
//         tot+=min(9*problem.villages[i].population - min(9*problem.villages[i].population,villagedrop[i][1]),villagedrop[i][0])*problem.packages[0].value + problem.packages[1].value*min(9*problem.villages[i].population,villagedrop[i][1]) + problem.packages[2].value*min(villagedrop[i][2],problem.villages[i].population);
//     }
//     tot-=Cost;
//     return tot;
// }
// return the total distance travelled in a trip from a home city
double h_val(Solution& solution, const ProblemData& problem) {
    vector<double> food_delivered_to_village(problem.villages.size() + 1, 0.0);
    vector<double> other_delivered_to_village(problem.villages.size() + 1, 0.0);

    double total_value_gained = 0.0;
    double trip_cost = 0.0;
    for (const auto& plan : solution) {
        const Helicopter& curr_heli = problem.helicopters[plan.helicopter_id - 1];
        const Point& homeCity = problem.cities[curr_heli.home_city_id - 1];

        for (const auto& trip : plan.trips) {
            if (trip.drops.empty()) {
                continue;
            }
            double tripDist = 0.0;
            Point currPos = homeCity;
            for (const auto &drop : trip.drops) {
                auto &village = problem.villages[drop.village_id-1];
                tripDist += distance(currPos, village.coords);
                currPos = village.coords;
            }
            if(trip.drops.size()>0){
                tripDist += distance(currPos, homeCity);
            }
            trip_cost += curr_heli.fixed_cost + (curr_heli.alpha * tripDist);
            for (const auto& drop : trip.drops) {
                const Village& village = problem.villages[drop.village_id - 1];
                
                double max_food_needed = village.population * 9.0;
               
                double food_room_left = max(0.0, max_food_needed - food_delivered_to_village[village.id]);
                
                double food_in_this_drop = drop.dry_food + drop.perishable_food;
                double effective_food_this_drop = min(food_in_this_drop, food_room_left);
                double effective_vp = min((double)drop.perishable_food, effective_food_this_drop);
                total_value_gained += effective_vp * problem.packages[1].value;

                double remaining_effective_food = effective_food_this_drop - effective_vp;
                double effective_vd = min((double)drop.dry_food, remaining_effective_food);
                total_value_gained += effective_vd * problem.packages[0].value;
                food_delivered_to_village[village.id] += food_in_this_drop;
                double max_other_needed = village.population * 1.0;
                double other_room_left = max(0.0, max_other_needed - other_delivered_to_village[village.id]);
                double effective_vo = min((double)drop.other_supplies, other_room_left);
                total_value_gained += effective_vo * problem.packages[2].value;
                other_delivered_to_village[village.id] += drop.other_supplies;
            }
        }
    }

    return total_value_gained - trip_cost;
}
bool isvalid(Solution &sol,int i ,int ind, const ProblemData &prob){
    if(sol[i].trips[ind].drops.size()==0){
        return true;
    } double dist = 0.0;
    for(int j = 0;j<(int)sol[i].trips[ind].drops.size();j++){
        if( j == 0){
            dist+=distance(prob.cities[sol[i].helicopter_id-1],prob.villages[sol[i].trips[ind].drops[j].village_id-1].coords);
        }
        else{
            dist+=distance(prob.villages[sol[i].trips[ind].drops[j-1].village_id-1].coords,prob.villages[sol[i].trips[ind].drops[j].village_id-1].coords);
        }
    }
    dist+=distance(prob.cities[sol[i].helicopter_id-1],prob.villages[sol[i].trips[ind].drops[sol[i].trips[ind].drops.size()-1].village_id-1].coords);
    return (dist<=prob.helicopters[sol[i].helicopter_id-1].distance_capacity);
}
pair<Solution,ProblemData> generate_neighbours_swap(Solution &sol, ProblemData &prob , int i){
    int i2 = -1;
    for(int j = 0;j<(int)sol.size();j++){
        if(sol[j].helicopter_id-1 == i){
            i2 = j;
            break;
        }
    }
    if(i2 == -1){
        return {sol,prob};
    }
    i = i2;
    if(sol[i].trips.size() == 0){
        return {sol,prob};
    }
    // cout<<"hey"<<endl;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int>distrind(0,sol[i].trips.size()-1);
    int ind = distrind(gen);
    if(sol[i].trips[ind].drops.size() < 2){
        return {sol,prob};
    }
    set<int>st;
    for(int j = 0 ;j<(int)sol[i].trips[ind].drops.size();j++){
        st.insert(j);
    }
    if(st.size() == 0){
        return {sol,prob};
    }
    uniform_int_distribution<int>indi(0,st.size()-1);
    auto it = st.begin();
    advance(it,indi(gen));
    int i1 = *it;
    // cout<<"i1:"<<i1<<endl;
    st.erase(i1);
    it = st.begin();
    uniform_int_distribution<int>ind1(0,st.size()-1);
    advance(it,ind1(gen));
    int j = *it;
    // cout<<"j:"<<j<<endl;
    Solution neigh = sol;
    if(i1>j){
        swap(i1,j);
    }
    // cout<<"hey"<<endl;
    swap(neigh[i].trips[ind].drops[i1],neigh[i].trips[ind].drops[j]);
    // cout<<"no"<<endl;
    if(!isvalid(neigh,i,ind,prob)){
        return {sol,prob};
    }
    double dist = 0;
    for(int j = 0;j<(int)neigh[i].trips[ind].drops.size();j++){
        if( j == 0){
            dist+=distance(prob.cities[neigh[i].helicopter_id-1],prob.villages[neigh[i].trips[ind].drops[j].village_id-1].coords);
        }
        else{
            dist+=distance(prob.villages[neigh[i].trips[ind].drops[j-1].village_id-1].coords,prob.villages[neigh[i].trips[ind].drops[j].village_id-1].coords);
        }
    }
    if(neigh[i].trips[ind].drops.size()>0){
        dist+=distance(prob.villages[neigh[i].trips[ind].drops[neigh[i].trips[ind].drops.size()-1].village_id-1].coords,prob.cities[neigh[i].helicopter_id-1]);
    }
    double dist2 = 0.0;
    for(int j = 0;j<(int)sol[i].trips[ind].drops.size();j++){
        if( j == 0){
            dist2+=distance(prob.cities[sol[i].helicopter_id-1],prob.villages[sol[i].trips[ind].drops[j].village_id-1].coords);
        }
        else{
           dist2+=distance(prob.villages[sol[i].trips[ind].drops[j-1].village_id-1].coords,prob.villages[sol[i].trips[ind].drops[j].village_id-1].coords);
        }
    }
    if(sol[i].trips[ind].drops.size()>0){
        dist2+=distance(prob.villages[sol[i].trips[ind].drops[sol[i].trips[ind].drops.size()-1].village_id-1].coords,prob.cities[sol[i].helicopter_id-1]);
    }
    if(prob.d_max<dist-dist2){
        return {sol,prob};
    }
    ProblemData newprob = prob;
    newprob.d_max -= (dist-dist2);
    // cout<<"d_max"<<newprob.d_max<<endl;
    return {neigh,newprob};
} 
double distancing(Solution &sol,int i ,int ind, const ProblemData &prob){
    if(sol[i].trips[ind].drops.size()==0){
        return 0.0;
    } double dist = 0.0;
    for(int j = 0;j<(int)sol[i].trips[ind].drops.size();j++){
        if( j == 0){
            dist+=distance(prob.cities[sol[i].helicopter_id-1],prob.villages[sol[i].trips[ind].drops[j].village_id-1].coords);
        }
        else{
            dist+=distance(prob.villages[sol[i].trips[ind].drops[j-1].village_id-1].coords,prob.villages[sol[i].trips[ind].drops[j].village_id-1].coords);
        }
    }
    dist+=distance(prob.cities[sol[i].helicopter_id-1],prob.villages[sol[i].trips[ind].drops[sol[i].trips[ind].drops.size()-1].village_id-1].coords);
    return dist;
} 
pair<Solution,ProblemData> generate_neighbours_add(Solution &sol, ProblemData & prob,int i){
    Solution neigh = sol;
    int i1 = -1;
    for(int j = 0;j<(int)sol.size();j++){
        if(sol[j].helicopter_id-1 == i){
            i1 = j;
            break;
        }
    }
    if(i1 == -1){
        HelicopterPlan hplan;
        hplan.helicopter_id = i+1;
        // cout<<hplan.helicopter_id<<endl;
        hplan.trips = {};
        neigh.pb(hplan);
        i1 = neigh.size()-1;
    }
    i = i1;
    mt19937 gen2(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int>distr(0,(int)neigh[i].trips.size());
    int ind = distr(gen2);
    if(ind == (int)neigh[i].trips.size()){
        Trip newtrip;
        newtrip.drops = {};
        newtrip.dry_food_pickup = 0;
        newtrip.other_supplies_pickup = 0;
        newtrip.perishable_food_pickup = 0;
        neigh[i].trips.push_back(newtrip);
    }
    auto trip = &neigh[i].trips[ind];
    auto plan = neigh[i];
    set<int>st;
    ProblemData prob1 = prob;
    for(int j = 0;j<(int)prob.villages.size();j++){
        st.insert(j);
    }
    for(auto drop:trip->drops){
        st.erase(drop.village_id-1);
    }
    if(st.size() == 0){
        return {neigh,prob};
    }
    mt19937 gen1(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(0, (int)st.size()-1);
    auto it = st.begin();
    advance(it, dist(gen1));
    int id = *it;
    if(trip->drops.size() == 0){
        if(2*distance(prob.cities[prob.helicopters[plan.helicopter_id-1].home_city_id-1],prob.villages[id].coords)<=prob1.d_max&&2*distance(prob.cities[prob.helicopters[plan.helicopter_id-1].home_city_id-1],prob.villages[id].coords)<=prob.helicopters[plan.helicopter_id-1].distance_capacity){
            mt19937 gen5(chrono::steady_clock::now().time_since_epoch().count());
            uniform_real_distribution<double> dist1(0.0, 1.0);
            double r1 = dist1(gen5);
            double r2 = dist1(gen5);
            double r3 = dist1(gen5);
            double tot = r1+r2+r3;
            r1/=tot;
            r2/=tot;
            r3/=tot;
             mt19937 gen3(chrono::steady_clock::now().time_since_epoch().count());
            uniform_real_distribution<double> dist3(0.0,prob.helicopters[plan.helicopter_id-1].weight_capacity-prob.packages[0].weight*trip->dry_food_pickup-prob.packages[2].weight*trip->other_supplies_pickup-prob.packages[1].weight*trip->perishable_food_pickup);
            double morefood = dist3(gen3);
            int d = (r1*morefood)/prob.packages[0].weight;
            int p = (r2*morefood)/prob.packages[1].weight;
            int o = (r3*morefood)/prob.packages[2].weight;
            // cout<<"r1: "<<r1<<"r2: "<<r2<<"r3: "<<r3<<endl;
            Drop newdrop;
            newdrop.dry_food = d;
            newdrop.other_supplies = o;
            newdrop.perishable_food = p;
            newdrop.village_id = id+1;
            trip->dry_food_pickup+=d;
            trip->other_supplies_pickup+=o;
            trip->perishable_food_pickup+=p;
            trip->drops.push_back(newdrop);
            prob1.d_max-=2*distance(prob.cities[prob.helicopters[plan.helicopter_id-1].home_city_id-1],prob.villages[id].coords);
        }
    }
    else{
        mt19937 gen(chrono::steady_clock::now().time_since_epoch().count());
        uniform_int_distribution<int>dist2(0,(int)trip->drops.size()-1);
        int bet = dist2(gen);
        double dist = distancing(sol,i,ind,prob);
        double d1 = distance(prob.villages[trip->drops[bet].village_id-1].coords,prob.villages[id].coords);
        double d2 = distance((bet+1 == (int)trip->drops.size()?prob.cities[prob.helicopters[plan.helicopter_id-1].home_city_id-1]:prob.villages[trip->drops[bet+1].village_id-1].coords),prob.villages[id].coords);
        if(d1+d2 - distance(prob.villages[trip->drops[bet].village_id-1].coords,(bet+1 == (int)trip->drops.size()?prob.cities[prob.helicopters[plan.helicopter_id-1].home_city_id-1]:prob.villages[trip->drops[bet+1].village_id-1].coords))<=prob1.d_max&&d1+d1- distance(prob.villages[trip->drops[bet].village_id-1].coords,(bet+1 == (int)trip->drops.size()?prob.cities[prob.helicopters[plan.helicopter_id-1].home_city_id-1]:prob.villages[trip->drops[bet+1].village_id-1].coords))+dist<=prob.helicopters[plan.helicopter_id-1].distance_capacity){
             mt19937 gen8(chrono::steady_clock::now().time_since_epoch().count());
            uniform_real_distribution<double> dist1(0.0, 1.0);
            double r1 = dist1(gen8);
            double r2 = dist1(gen8);
            double r3 = dist1(gen8);
            double tot = r1+r2+r3;
            r1/=tot;
            r2/=tot;
            r3/=tot;
             mt19937 gen9(chrono::steady_clock::now().time_since_epoch().count());
            uniform_real_distribution<double> dist3(0.0,prob.helicopters[plan.helicopter_id-1].weight_capacity-prob.packages[0].weight*trip->dry_food_pickup-prob.packages[2].weight*trip->other_supplies_pickup-prob.packages[1].weight*trip->perishable_food_pickup);
            double morefood = dist3(gen9);
            int d = (r1*morefood)/prob.packages[0].weight;
            int p = (r2*morefood)/prob.packages[1].weight;
            int o = (r3*morefood)/prob.packages[2].weight;
            Drop newdrop;
            newdrop.dry_food = d;
            newdrop.other_supplies = o;
            newdrop.perishable_food = p;
            newdrop.village_id = id+1;
            trip->dry_food_pickup+=d;
            trip->other_supplies_pickup+=o;
            trip->perishable_food_pickup+=p;
            trip->drops.insert(trip->drops.begin()+bet+1,newdrop);
            prob1.d_max-=(d1+d2 - distance(prob.villages[trip->drops[bet].village_id-1].coords,(bet+1 == (int)trip->drops.size()?prob.cities[prob.helicopters[plan.helicopter_id-1].home_city_id-1]:prob.villages[trip->drops[bet+1].village_id-1].coords)));
        }
    }
    return {neigh,prob1};
}
pair<Solution,ProblemData> Random_state_generator(ProblemData &prob , vector<int>&close){
    Solution sol = {};
    ProblemData newprob = prob;
    int count = 0;
    for(int i = 0;i<prob.helicopters.size();i++){
        mt19937 gen(chrono::steady_clock::now().time_since_epoch().count());
        int max_travels = newprob.d_max/(1+distance(newprob.villages[close[i]].coords,newprob.cities[newprob.helicopters[i].home_city_id-1]));
        uniform_int_distribution<int>distr(0,max_travels);
        int num_trips = distr(gen);
        HelicopterPlan hi;
        hi.helicopter_id = i+1;               
        hi.trips = {};
        for(int j =0;j<num_trips;j++){
          
            mt19937 gen1(chrono::steady_clock::now().time_since_epoch().count());
            uniform_int_distribution<int>distrind(0,newprob.villages.size());
            int numvisited = distrind(gen1);
            set<int>st;
            double curr_dist = 0;
            double curr_id = -1;
            Trip newtrip;
            newtrip.drops = {};
            newtrip.dry_food_pickup = 0;
            newtrip.other_supplies_pickup = 0;
            newtrip.perishable_food_pickup = 0;
            for(int k = 0 ;k<numvisited;k++){
                mt19937 gen2(chrono::steady_clock::now().time_since_epoch().count());
                uniform_int_distribution<int>distrin(0,newprob.villages.size()-1);
                int visid = distrin(gen2);
                if(st.find(visid)==st.end()){
                    if(curr_id == -1){
                        curr_dist += distance(newprob.villages[visid].coords,newprob.cities[newprob.helicopters[i].home_city_id-1]);
                    }
                    else{
                        curr_dist += distance(newprob.villages[visid].coords,newprob.villages[curr_id].coords);
                    }
                    if(curr_dist+distance(newprob.cities[newprob.helicopters[i].home_city_id-1],newprob.villages[visid].coords)<=newprob.helicopters[i].distance_capacity && curr_dist+distance(newprob.cities[newprob.helicopters[i].home_city_id-1],newprob.villages[visid].coords)<=newprob.d_max){
                        curr_id = visid;
                        st.insert(visid);
                        Drop newdrop;
                        newdrop.village_id = visid+1;
                        newdrop.dry_food = 0;
                        newdrop.other_supplies = 0;
                        newdrop.perishable_food = 0;
                        newtrip.drops.push_back(newdrop);
                    }
                    else{
                        if(curr_id == -1){
                            curr_dist -= distance(newprob.villages[visid].coords,newprob.cities[newprob.helicopters[i].home_city_id-1]);
                        }
                        else{
                            curr_dist -= distance(newprob.villages[visid].coords,newprob.villages[curr_id].coords);
                        }
                    }
                }
            }
            if(curr_id!=-1){
                newprob.d_max-=curr_dist+distance(newprob.cities[newprob.helicopters[i].home_city_id-1],newprob.villages[curr_id].coords);
            } 
            hi.trips.push_back(newtrip);
        }
        sol.push_back(hi);
    }
    for(auto &plan : sol){
        if(plan.trips.size()>0){
            for(int i = 0;i<plan.trips.size();i++){
                double curr = newprob.helicopters[plan.helicopter_id-1].weight_capacity;
                if(plan.trips[i].drops.size()>0){
                    for(int j = 0;j<plan.trips[i].drops.size();j++){
                        mt19937 gen3(chrono::steady_clock::now().time_since_epoch().count());
                        uniform_int_distribution<int>distrin(min((int)(curr/newprob.packages[1].weight),9*newprob.villages[plan.trips[i].drops[j].village_id-1].population/2),min(9*newprob.villages[plan.trips[i].drops[j].village_id-1].population,(int)(curr/newprob.packages[1].weight)));
                        int p = distrin(gen3);
                        plan.trips[i].perishable_food_pickup+=p;
                        plan.trips[i].drops[j].perishable_food+=p;
                        curr-=p*newprob.packages[1].weight;
                        mt19937 gen4(chrono::steady_clock::now().time_since_epoch().count());
                        uniform_int_distribution<int>distrin1(0,min(9*newprob.villages[plan.trips[i].drops[j].village_id-1].population-p,(int)(curr/newprob.packages[0].weight)));
                        int d = distrin1(gen4);
                        plan.trips[i].dry_food_pickup+=d;
                        plan.trips[i].drops[j].dry_food+=d;
                        curr-=d*newprob.packages[0].weight;
                        mt19937 gen5(chrono::steady_clock::now().time_since_epoch().count());
                        uniform_int_distribution<int>distrin2(0,min(newprob.villages[plan.trips[i].drops[j].village_id-1].population,(int)(curr/newprob.packages[2].weight)));
                        int o = distrin2(gen5);
                        plan.trips[i].other_supplies_pickup+=o;
                        plan.trips[i].drops[j].other_supplies+=o;
                        curr-=o*newprob.packages[2].weight;
                    }
                }
            }
        }
    }
    return {sol,newprob};
}
Solution solve(const ProblemData& problem) {
    cout << "Starting solver..." << endl;
    ProblemData prob = problem;
    ProblemData prob2 = problem;
    auto start_time = steady_clock::now();
    vector<int>close(prob.helicopters.size());
    // cout<<"hi"<<endl;
    for(int i = 0;i<prob.helicopters.size();i++){
        int curr = -1;
        double dist = DBL_MAX;
        for(int j = 0;j<prob.villages.size();j++){
            if(distance(prob.villages[j].coords,prob.cities[prob.helicopters[i].home_city_id-1])<dist){
                dist = distance(prob.villages[j].coords,prob.cities[prob.helicopters[i].home_city_id-1]);
                curr = j;
            }
        }
        close[i] = curr;
    }
    pair<Solution,ProblemData> fin = Random_state_generator(prob,close);
    Solution finsol = fin.first;
    prob = fin.second;
    auto allowed_duration = milliseconds(long(prob.time_limit_minutes * 60 * 1000));
    auto buffer = milliseconds(50); // 50ms safety margin
    auto deadline = start_time + allowed_duration - buffer;
    double currhval = h_val(finsol,prob);
    Solution bestsol = finsol;
    int count = 0;
    double temperature = 10000.0;
    double cooling_rate = 0.99995;
    double best = currhval;
    uniform_real_distribution<double> acceptance_dist(0.0, 1.0);
    uniform_real_distribution<double> move_dist(0.0, 1.0);
    int iterations_at_temp = 0;
    const int MOVES_PER_TEMP_STEP = 100;
    while(steady_clock::now() < deadline){
                //solve
            // count++;
            // if(count%2500 == 0){
            //     cout<<"count:"<<count<<' '<<h_val(bestsol,prob)<<endl;
            //     fin = Random_state_generator(prob,close);
            //     finsol = fin.first;
            //     prob = fin.second;
            // }
            auto current_time = steady_clock::now();
            auto elapsed_duration = duration_cast<milliseconds>(current_time - start_time);
            double elapsed_ratio = (double)elapsed_duration.count() / allowed_duration.count();
            double swap_probability = 0.2 + (0.2 * elapsed_ratio);
            
            mt19937 gen(chrono::steady_clock::now().time_since_epoch().count());
            uniform_int_distribution<int>distr(0,prob.helicopters.size()-1);
            mt19937 gen10(chrono::steady_clock::now().time_since_epoch().count());
            uniform_real_distribution<double> move_dist(0.0, 1.0);
            bool ok = false;
            int i = distr(gen);
            // for(int i = 0;i<(int)prob.helicopters.size();i++){
                // double probi = distr(gen);
                // Solution neigh;
                // ProblemData neprob;
                pair<Solution,ProblemData>neigh;
                if(move_dist(gen10)<=0.5){
                    neigh = generate_neighbours_add(finsol,prob,i);
                    // double h = h_val(neigh1.first,neigh1.second);
                    // if(h>currhval){
                    //     finsol = neigh1.first;
                    //     prob = neigh1.second;
                    //     ok = true;
                    //     bestsol = finsol;
                    //     currhval = h;
                    // }
                }
                else{
                    neigh = generate_neighbours_swap(finsol,prob,i);
                    // dh = h_val(neigh.first,neigh.second);
                    // if(h>currhval){
                    //     finsol = neigh.first;
                    //     prob = neigh.second;
                    //     ok = true;
                    //     bestsol = finsol;
                    //     currhval = h;
                    // }
                    // pair<Solution,ProblemData>ni = generate_neighbours_swap(finsol, prob, i);
                    // neigh = ni.first;
                    // neprob = ni.second;
                }
                 double h = h_val(neigh.first, problem);
                double score_diff = h - currhval;
                if (score_diff > 0 || acceptance_dist(gen) < exp(score_diff / temperature)) {
            // If we accept, the neighbor becomes our new current solution.
                    finsol = neigh.first;
                    currhval = h;
                    prob = neigh.second;
                }
                if (currhval > best) {
                    bestsol = finsol;
                    best = currhval;
                }
                // double h = h_val(neigh, prob);
                // double score_diff = h - currhval;
                // if (score_diff > 0 || acceptance_dist(gen) < exp(score_diff / temperature)) {
                //     finsol = neigh;
                //     currhval = h;
                //     prob = neprob;
                // }

                // // Update the best-ever solution found
                // if (currhval > best) {
                //     bestsol = finsol;
                //     best = currhval;
                // }
                
                // // Cool down the temperature for the next iteration
                // temperature *= cooling_rate;
            // }
            // if(!ok){
            //     fin = Random_state_generator(prob,close);
            //     finsol = fin.first;
            //     prob = fin.second;
            //     double h = h_val(finsol,prob);
            //     cout<<"count:"<<count<<' '<<h_val(bestsol,prob)<<endl;
            //     if(h>currhval){
            //         currhval = h;
            //         bestsol = finsol;  
            //     }
            // }
            iterations_at_temp++;
            if (iterations_at_temp >= MOVES_PER_TEMP_STEP) {
                temperature *= cooling_rate;
                iterations_at_temp = 0; // Reset the counter for the new temperature.
            }
            cout<<"val:"<<currhval<<endl;
    }
    // // --- START OF PLACEHOLDER LOGIC ---
    // // This is a naive example: send each helicopter on one trip to the first village.
    // // This will definitely violate constraints but shows the structure.
    
    // // for (const auto& helicopter : problem.helicopters) {
    // //     HelicopterPlan plan;
    // //     plan.helicopter_id = helicopter.id;

    // //     if (!problem.villages.empty()) {
    // //         Trip trip;
    // //         // Pickup 1 of each package type
    // //         trip.dry_food_pickup = 1;
    // //         trip.perishable_food_pickup = 1;
    // //         trip.other_supplies_pickup = 1;

    // //         // Drop them at the first village
    // //         Drop drop;
    // //         drop.village_id = problem.villages[0].id;
    // //         drop.dry_food = 1;
    // //         drop.perishable_food = 1;
    // //         drop.other_supplies = 1;

    // //         trip.drops.push_back(drop);
    // //         plan.trips.push_back(trip);
    // //     }
    // //     solution.push_back(plan);
    // // }
    
    // // --- END OF PLACEHOLDER LOGIC ---

    // cout << "Solver finished." << endl;
    return bestsol;
}
void printsol(Solution &s){
    for(auto plan : s){
        cout<<plan.helicopter_id<<endl;
        cout<<plan.trips.size()<<endl;
        for(auto trip : plan.trips){
            for(int i = 0;i<(int)trip.drops.size();i++){
                cout<<i<<":"<<trip.drops[i].village_id<<' '<<trip.drops[i].dry_food<<' '<<trip.drops[i].perishable_food<<' '<<trip.drops[i].other_supplies<<' ';
            }
            cout<<endl;
        }
        cout<<endl;
    }
}
// int main(){
//     ProblemData prob;
//     prob.time_limit_minutes = 1;
//     prob.d_max = 1000;
//     PackageInfo d;
//     d.weight = 0.2;
//     d.value = 3;
//     PackageInfo p;
//     p.weight = 0.5;
//     p.value = 5;
//     PackageInfo o;
//     o.weight = 0.3;
//     o.value = 4;
//     prob.packages = {d,p,o};
//     Point c1;
//     c1.x = 0;
//     c1.y = 0;
//     Point c2;
//     c2.x = 100;
//     c2.y = 0;
//     prob.cities = {c1,c2};
//     Village v1;
//     v1.coords.x = 10;
//     v1.coords.y = 10;
//     v1.population = 10;
//     v1.id = 1;
//     Village v2;
//     v2.coords.x = 90;
//     v2.coords.y = 20;
//     v2.population = 30;
//     v2.id = 2;
//     Village v3;
//     v3.coords.x =-20;
//     v3.coords.y = 50;
//     v3.population = 50;
//     v3.id = 3;
//     prob.villages = {v1,v2,v3};
//     Helicopter h1;
//     h1.home_city_id = 1;
//     h1.weight_capacity = 100;
//     h1.distance_capacity = 200;
//     h1.fixed_cost = 50;
//     h1.alpha = 1;
//     h1.id = 1;
//     Helicopter h2;
//     h2.home_city_id = 2;
//     h2.weight_capacity = 50;
//     h2.distance_capacity = 100;
//     h2.fixed_cost = 75;
//     h2.alpha = 1.5;
//     h2.id = 2;
//     prob.helicopters = {h1,h2};
//     const ProblemData p1 = prob;
//     solve(p1);
// }
// int main(){
//     ProblemData prob;
//     prob.time_limit_minutes = 1;
//     prob.d_max = 100;
//     PackageInfo d;
//     d.value = 1;
//     d.weight = 0.01;
//     PackageInfo p;
//     p.weight = 0.1;
//     p.value = 2;
//     PackageInfo o;
//     o.value = 0.1;
//     o.weight = 0.005;
//     prob.packages = {d,p,o};
//     Point p1;
//     p1.x = 0;
//     p1.y = 0;
//     Point p2;
//     p2.x = 10;
//     p2.y = 10;
//     prob.cities = {p1,p2};
//     Village v1,v2;
//     v1.id = 0;
//     v2.id = 1;
//     v1.population = 1000;
//     v2.population = 1000;
//     Point v_1;
//     v_1.x = 0;
//     v_1.y = 5;
//     v1.coords = v_1;
//     Point v_2;
//     v_2.x= 0;
//     v_2.y = 10; 
//     v2.coords = v_2;
//     prob.villages = {v1,v2};
//     Helicopter h1 , h2;
//     h1.home_city_id = 0;
//     h2.home_city_id = 1;
//     h1.weight_capacity = 100;
//     h2.weight_capacity = 100;
//     h1.distance_capacity = 25;
//     h2.distance_capacity = 50;
//     h1.fixed_cost = 10;
//     h2.fixed_cost = 10;
//     h1.alpha = 1;
//     h2.alpha = 1;
//     h1.id = 0;
//     h2.id = 1;
//     prob.helicopters = {h1,h2};
//     // Solution s;
//     // HelicopterPlan ht;
//     // ht.helicopter_id = 0;
//     // Trip t1;
//     // t1.dry_food_pickup = 9000;
//     // t1.other_supplies_pickup = 2000;
//     // t1.perishable_food_pickup = 0;
//     // Drop d1;
//     // d1.village_id = 0;
//     // d1.dry_food = 9000;
//     // d1.other_supplies = 1000;
//     // d1.perishable_food  = 0;
//     // Drop d2;
//     // d2.dry_food = 0;
//     // d2.other_supplies = 1000;
//     // d2.perishable_food = 0;
//     // d2.village_id = 1;
//     // t1.drops = {d1,d2};
//     // ht.trips.push_back(t1);
//     // s.push_back(ht);
//     // cout<<h_val(s,prob)<<endl;
//     // printsol(s);
//     // pair<Solution , ProblemData>neigh = generate_neighbours_swap(s,prob,0);
//     // // pair<Solution , ProblemData>neigh1 = generate_neighbours_add(s,prob,0);
//     // printsol(neigh.first);
//     // // printsol(neigh1.first);
//     cout<<h_val(finsol,prob)<<endl;
//     printsol(finsol);
//     return 0;
// }
