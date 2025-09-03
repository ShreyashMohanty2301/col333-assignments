#include "solver.h"
#include <iostream>
#include <chrono>
#include <bits/stdc++.h>
using namespace std;
#include <random>
#define pb push_back
// You can add any helper functions or classes you need here.

/**
 * @brief The main function to implement your search/optimization algorithm.
 * * This is a placeholder implementation. It creates a simple, likely invalid,
 * plan to demonstrate how to build the Solution object. 
 * * TODO: REPLACE THIS ENTIRE FUNCTION WITH YOUR ALGORITHM.
 */

double h_val(Solution &sol,const ProblemData &problem){
    double tot = 0.0;
    for(auto plan:sol){
        for(auto trip:plan.trips){
            double dist = 0.0;
            for(int i = 0;i<trip.drops.size();i++){
                if(i == 0){
                    dist+=distance(problem.villages[problem.helicopters[plan.helicopter_id].home_city_id].coords,problem.villages[trip.drops[i].village_id].coords);
                }
                else{
                    dist+=distance(problem.villages[trip.drops[i-1].village_id].coords,problem.villages[trip.drops[i].village_id].coords);
                }
            }
            dist+=distance(problem.villages[trip.drops[trip.drops.size()-1].village_id].coords,problem.villages[problem.helicopters[plan.helicopter_id].home_city_id].coords);
            double cost = problem.helicopters[plan.helicopter_id].fixed_cost + problem.helicopters[plan.helicopter_id].alpha*dist;
            double val = 0.0;
            for(auto drop:trip.drops){
                if(9*problem.villages[drop.village_id].population<drop.perishable_food+drop.dry_food ||problem.villages[drop.village_id].population<drop.other_supplies){
                    continue;
                }
                val+=problem.packages[0].value*drop.dry_food + problem.packages[1].value*drop.perishable_food + problem.packages[2].value*drop.other_supplies;
            }
            tot+=val-cost;
        }  
    }
    return tot;
}

vector<pair<Solution,ProblemData>> generate_neighbours_add(Solution &sol, const ProblemData & prob){
    vector<pair<Solution,ProblemData>>genn;
    for(auto plan : sol){
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int>distr(0,plan.trips.size()-1);
        int ind = distr(gen);
        auto trip = &plan.trips[ind];
        Solution neigh = sol;
        set<int>st;
        ProblemData prob1 = prob;
        for(int i = 0;i<prob.villages.size();i++){
            st.insert(i);
        }
        for(auto drop:trip->drops){
            st.erase(drop.village_id);
        }
        uniform_int_distribution<int> dist(0, (int)st.size()-1);
        auto it = st.begin();
        advance(it, dist(gen));
        int id = *it;
        if(trip->drops.size() == 0){
            if(2*distance(prob.cities[prob.helicopters[plan.helicopter_id].home_city_id],prob.villages[id].coords)<=prob1.d_max&&distance(prob.cities[prob.helicopters[plan.helicopter_id].home_city_id],prob.villages[id].coords)<=prob.helicopters[plan.helicopter_id].distance_capacity){
                uniform_real_distribution<double> dist1(0.0, 1.0);
                double ratio = dist1(gen);
                uniform_real_distribution<double> dist2(1.0-ratio,1.0);
                double r2 = dist2(gen);
                uniform_int_distribution<int> dist3(0,prob.helicopters[plan.helicopter_id].weight_capacity-trip->dry_food_pickup-trip->other_supplies_pickup-trip->perishable_food_pickup);
                int morefood = dist3(gen);
                int d = ratio*morefood;
                int p = r2*morefood;
                int o = morefood-d-p;
                Drop newdrop;
                newdrop.dry_food = d;
                newdrop.other_supplies = o;
                newdrop.perishable_food = p;
                newdrop.village_id = id;
                trip->dry_food_pickup+=d;
                trip->other_supplies_pickup+=o;
                trip->perishable_food_pickup+=p;
                trip->drops.push_back(newdrop);
                prob1.d_max-=2*distance(prob.cities[prob.helicopters[plan.helicopter_id].home_city_id],prob.villages[id].coords);
                genn.pb({neigh,prob1});
            }
        }
        else{
            uniform_int_distribution<int>dist2(0,trip->drops.size()-1);
            int bet = dist2(gen);
            double d1 = distance(prob.villages[trip->drops[bet].village_id].coords,prob.villages[id].coords);
            double d2 = distance(prob.villages[trip->drops[(bet+1)%trip->drops.size()].village_id].coords,prob.villages[id].coords);
            if(d1+d2<=prob1.d_max&&d1<=prob.helicopters[plan.helicopter_id].distance_capacity&&d2<=prob.helicopters[plan.helicopter_id].distance_capacity){
                uniform_real_distribution<double> dist1(0.0, 1.0);
                double ratio = dist1(gen);
                uniform_real_distribution<double> dist2(1.0-ratio,1.0);
                double r2 = dist2(gen);
                uniform_int_distribution<int> dist3(0,prob.helicopters[plan.helicopter_id].weight_capacity-trip->dry_food_pickup-trip->other_supplies_pickup-trip->perishable_food_pickup);
                int morefood = dist3(gen);
                int d = ratio*morefood;
                int p = r2*morefood;
                int o = morefood-d-p;
                Drop newdrop;
                newdrop.dry_food = d;
                newdrop.other_supplies = o;
                newdrop.perishable_food = p;
                newdrop.village_id = id;
                trip->dry_food_pickup+=d;
                trip->other_supplies_pickup+=o;
                trip->perishable_food_pickup+=p;
                trip->drops.push_back(newdrop);
                prob1.d_max-=(d1+d2);
                genn.pb({neigh,prob1});
            }
        }
    }
    return genn;
}

Solution solve(const ProblemData& problem) {
    cout << "Starting solver..." << endl;

    Solution solution;
    auto start = chrono::high_resolution_clock::now();
    while(true){
        auto curr = chrono::high_resolution_clock::now();
        if(chrono::duration_cast<chrono::milliseconds>(curr-start).count()>=(problem.time_limit_minutes)*60*1000){
            break;
        }

    }
    // --- START OF PLACEHOLDER LOGIC ---
    // This is a naive example: send each helicopter on one trip to the first village.
    // This will definitely violate constraints but shows the structure.
    
    // for (const auto& helicopter : problem.helicopters) {
    //     HelicopterPlan plan;
    //     plan.helicopter_id = helicopter.id;

    //     if (!problem.villages.empty()) {
    //         Trip trip;
    //         // Pickup 1 of each package type
    //         trip.dry_food_pickup = 1;
    //         trip.perishable_food_pickup = 1;
    //         trip.other_supplies_pickup = 1;

    //         // Drop them at the first village
    //         Drop drop;
    //         drop.village_id = problem.villages[0].id;
    //         drop.dry_food = 1;
    //         drop.perishable_food = 1;
    //         drop.other_supplies = 1;

    //         trip.drops.push_back(drop);
    //         plan.trips.push_back(trip);
    //     }
    //     solution.push_back(plan);
    // }
    
    // --- END OF PLACEHOLDER LOGIC ---

    cout << "Solver finished." << endl;
    return solution;
}