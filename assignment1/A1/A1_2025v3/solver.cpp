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
int random_number(int l, int r) {
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
	uniform_int_distribution<int> dist(l, r);
	return dist(rng);
}

double random_double(double min,double max){
    mt19937 generator(chrono::steady_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> distribution(min,max);
    return distribution(generator);
}

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
double distancing(const Solution &sol, int i, int ind, const ProblemData &prob){
    if (sol[i].trips[ind].drops.empty()) {
        return 0.0;
    } 
    
    // Correctly find the helicopter and its home city coordinates.
    const Helicopter& helicopter = prob.helicopters[sol[i].helicopter_id - 1];
    const Point& homeCity = prob.cities[helicopter.home_city_id - 1];

    double dist = 0.0;
    Point currentPos = homeCity;

    for (const auto& drop : sol[i].trips[ind].drops) {
        const Point& villagePos = prob.villages[drop.village_id - 1].coords;
        dist += distance(currentPos, villagePos);
        currentPos = villagePos;
    }
    
    dist += distance(currentPos, homeCity);
    return dist;
}
double tot_distance(int i, const Solution &sol, const ProblemData& prob){
    double dist = 0;
    for (size_t j = 0; j < sol[i].trips.size(); j++) {
        dist += distancing(sol, i, j, prob);
    }
    return dist;
}
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
Solution generate_neighbours_swap(Solution &sol, const ProblemData &prob , int i){
    int i2 = -1;
    int hid = i;
    for(size_t j = 0; j < sol.size(); j++){
        if(sol[j].helicopter_id - 1 == i) { i2 = j; break; }
    }
    if(i2 == -1) return sol;
    i = i2;
    if(sol[i].trips.empty() || sol[i].trips.size() == 0) return sol;
    int ind = random_number(0, sol[i].trips.size() - 1);
    if(sol[i].trips[ind].drops.size() < 2) return sol;  
    int i1 = random_number(0, sol[i].trips[ind].drops.size() - 1);
    int j1 = random_number(0, sol[i].trips[ind].drops.size() - 1);
    if (i1 == j1) return sol;
    Solution neigh = sol;
    swap(neigh[i].trips[ind].drops[i1], neigh[i].trips[ind].drops[j1]);
    double dist = distancing(neigh, i, ind, prob);
    if(tot_distance(i, neigh, prob) > prob.d_max || dist > prob.helicopters[hid].distance_capacity){
        return sol; 
    }
    return neigh;
} 
Solution generate_neighbour_delete(Solution &sol , const ProblemData &prob , int i){
    int i2 = -1;
    int hid = i;
    for(size_t j = 0; j < sol.size(); j++){
        if(sol[j].helicopter_id - 1 == i) { i2 = j; break; }
    }
    if(i2 == -1) return sol;
    i = i2;
    if(sol[i].trips.empty() || sol[i].trips.size() == 0) return sol;
    int ind = random_number(0, sol[i].trips.size() - 1);
    if(sol[i].trips[ind].drops.empty()) return sol;
    int dp = random_number(0, sol[i].trips[ind].drops.size() - 1);
    Solution neigh = sol;
    neigh[i].trips[ind].dry_food_pickup -= neigh[i].trips[ind].drops[dp].dry_food;
    neigh[i].trips[ind].perishable_food_pickup -= neigh[i].trips[ind].drops[dp].perishable_food;
    neigh[i].trips[ind].other_supplies_pickup -= neigh[i].trips[ind].drops[dp].other_supplies;
    neigh[i].trips[ind].drops.erase(neigh[i].trips[ind].drops.begin() + dp);
    if(distancing(neigh, i, ind, prob) <= prob.helicopters[hid].distance_capacity && tot_distance(i, neigh, prob) <= prob.d_max){
        return neigh;
    }
    return sol;
}
Solution generate_neighbours_add(Solution &sol, const ProblemData & prob,int i){
    Solution neigh = sol;

    int hid = i;
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
    int ind = random_number(0,(int)neigh[i].trips.size());
    if(ind == (int)neigh[i].trips.size()){
        Trip newtrip;
        newtrip.drops = {};
        newtrip.dry_food_pickup = 0;
        newtrip.other_supplies_pickup = 0;
        newtrip.perishable_food_pickup = 0;
        neigh[i].trips.push_back(newtrip);
    }
    set<int>st;
    for(int j = 0;j<prob.villages.size();j++){
        st.insert(j);
    }
    for(auto drop:neigh[i].trips[ind].drops){
        st.erase(drop.village_id-1);
    }
    if(st.size() == 0){
        return sol;
    }
    int chosen = random_number(0,st.size()-1);
    auto it = st.begin();
    advance(it,chosen);
    int id = *it;
    int bet = 0;
    if(neigh[i].trips[ind].drops.size() == 0){
        Drop newdrop;
        newdrop.village_id = id+1;
        neigh[i].trips[ind].drops.push_back(newdrop);
    }
    else{
        Drop newdrop;
        newdrop.village_id = id+1;
        bet = random_number(0,neigh[i].trips[ind].drops.size()-1);
        neigh[i].trips[ind].drops.insert(neigh[i].trips[ind].drops.begin()+bet,newdrop);
    }
    if(tot_distance(i,neigh,prob)<=prob.d_max && distancing(neigh,i,ind,prob)<=prob.helicopters[hid].distance_capacity){
        double cap = prob.helicopters[hid].weight_capacity - neigh[i].trips[ind].dry_food_pickup*prob.packages[0].weight - neigh[i].trips[ind].perishable_food_pickup*prob.packages[1].weight - neigh[i].trips[ind].other_supplies_pickup*prob.packages[2].weight;
        int pmax = cap/prob.packages[1].weight;
        int p = random_number(pmax/3 , pmax);
        int dfnl = cap-p*prob.packages[1].weight;
        int df = random_double(dfnl/10,dfnl);
        int ratio = random_double(0,1);
        int d = (ratio*dfnl)/prob.packages[0].weight;
        int o = ((1-ratio)*dfnl)/prob.packages[2].weight;
        if(cap>=p*prob.packages[1].weight + d*prob.packages[0].weight + o*prob.packages[2].weight){
            neigh[i].trips[ind].drops[bet].dry_food = d;
            neigh[i].trips[ind].drops[bet].perishable_food = p;
            neigh[i].trips[ind].drops[bet].other_supplies = o;
            neigh[i].trips[ind].dry_food_pickup+=d;
            neigh[i].trips[ind].perishable_food_pickup+=p;
            neigh[i].trips[ind].other_supplies_pickup+=o;
            return neigh;
        }
    }
    return sol;
}
// This is the corrected version of the initial solution generator.
// It is designed to always produce a valid solution.
Solution Random_state_generator(const ProblemData &prob){
    Solution sol;
    for(int i = 0;i<prob.helicopters.size();i++){
        double probi = random_double(0.0,1.0);
        if(probi>0.3){
            while(random_double(0.0,1.0)>0.5){
                sol = generate_neighbours_add(sol,prob,i);
            }
        }
    }
    return sol;
}
Solution solve(const ProblemData& problem) {
    cout << "Starting solver..." << endl;
    ProblemData prob = problem;
    ProblemData prob2 = problem;
    auto start_time = steady_clock::now();
    Solution finsol = Random_state_generator(prob);
    auto allowed_duration = milliseconds(long(prob.time_limit_minutes * 60 * 1000));
    auto buffer = milliseconds(1000); // 50ms safety margin
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
    // --- Start of the main loop to replace ---

    // Create a single random number generator before the loop for efficiency.
    mt19937 gen(chrono::steady_clock::now().time_since_epoch().count());

    while(steady_clock::now() < deadline){
        count++;

        // Restart logic (can be kept or removed in SA, but let's keep your structure)
        // if(count % 2500 == 0){
        //     cout << "count:" << count << ' ' << h_val(bestsol, prob2) << endl;
        //     fin = Random_state_generator(prob2, close);
        //     finsol = fin.first;
        //     prob = fin.second;
        //     currhval = h_val(finsol, prob); // Recalculate hval for the new state
        // }
        
        // Randomly select a helicopter to modify
        uniform_int_distribution<int> distr(0, problem.helicopters.size() - 1);
        int i = distr(gen);

        // This is a CRITICAL change. You must create a const copy of the
        // current problem state for the neighbor functions. Do not modify the original.
        Solution neigh;

        // Choose a move (add or swap)
        double probi = random_double(0,1);
        if(probi <=0.6){
            neigh = generate_neighbours_add(finsol, prob, i);
        } else if(probi<=0.8){
            neigh = generate_neighbours_swap(finsol, prob, i);
        }
        else{
            neigh = generate_neighbour_delete(finsol,prob,i);
        }

        // --- CORE SIMULATED ANNEALING LOGIC ---
        
        // 1. Calculate the score of the new neighbor solution.
        //    IMPORTANT: Use the original 'problem' object here for a consistent score evaluation.
        double h = h_val(neigh, prob);
        
        // 2. Find the difference in score. A positive value means the new solution is better.
        double score_diff = h - currhval;

        // 3. The Acceptance Criterion:
        //    - If the new solution is better (score_diff > 0), we ALWAYS accept it.
        //    - If it's worse, we MIGHT accept it based on the probability `exp(score_diff / temperature)`.
        if (score_diff > 0 || acceptance_dist(gen) < exp(score_diff / temperature)) {
            // If accepted, the neighbor becomes our new current solution.
            finsol = neigh; // Update the problem state (with the new d_max)
            currhval = h;
        }
        // --- END OF CORE LOGIC ---


        // Update the best-ever solution if the current one is an improvement.
        if (currhval > best) {
            bestsol = finsol;
            best = currhval;
        }

        // Cool down the temperature to make accepting worse solutions less likely over time.
        temperature *= cooling_rate;
        
        cout << "val:" << currhval << endl;
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
