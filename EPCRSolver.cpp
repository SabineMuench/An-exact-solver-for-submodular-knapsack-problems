/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#include "EPCRSolver.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <nlohmann/json.hpp>



float EPCRSolver::search(const std::vector<int>& S, const std::vector<int>& C, const int & B, float s_best, int s_weight, const std::unordered_map<int, float>* previous_gains = nullptr) {
    if (std::chrono::high_resolution_clock::now() > end_time_limit) { //if time limit exceeded, return -1
        return -1.0f;
    }

    counter++;
    float s_value = f(S); // update objective value of current solution set

    s_best = std::max(s_value, s_best); // update best solution

    if (C.empty()) { 
        return s_best; // if C is empty, a leaf node is reached and s_best returned
    }

    int capacity = B - s_weight; // update left capacity

    if (capacity == 0) { 
        return s_best; // if no capacity is left, return best solution
    }

  
    auto [status, new_C, current_gains] = EP(S, C, s_value, s_best, capacity, previous_gains); // apply Early Pruning
   
    
    if (status == true){
        return s_best;
    }

    // determine knapsackset for candidate reduction
    auto knapsackset = packedset(new_C, capacity);
    // candidate reduction
    new_C = CRep(new_C, s_value, knapsackset, s_best, capacity, current_gains);
    
    for (size_t i = 0, n = new_C.size(); i < n; ++i) { // iterate over all items in the candidate set
        std::vector<int> new_S = S; // new_S = S+item i
        new_S.push_back(new_C[i]);
        // recursiv call of search
        s_best = search(new_S, {new_C.begin() + i + 1, new_C.end()}, B, s_best, s_weight + items[new_C[i]].weight, &current_gains); //, &current_gains);
    }
    return s_best;
}

float EPCRSolver::solve(const std::vector<int>& C, int B) {
    float s_best = search({}, C, B, 0, 0,nullptr);
    return s_best;
}
