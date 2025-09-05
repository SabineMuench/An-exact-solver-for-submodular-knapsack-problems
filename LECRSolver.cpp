/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#include "LECRSolver.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <nlohmann/json.hpp>



float LECRSolver::search(const std::vector<int>& S, const std::vector<int>& C, int B, float s_best, int s_weight, const std::unordered_map<int, float>* previous_gains) {
    if (std::chrono::high_resolution_clock::now()>end_time_limit){ //if time limit exceeded, return -1
        return -1.0f;
    }
    counter ++;

    float s_value = f(S); // update objective value of current solution set

    s_best = std::max(s_value, s_best); // update best solution
    
    if (C.empty()) { // if C is empty, a leaf node is reached and s_best returned
        return s_best;
    }
    
    int capacity = B - s_weight; // update left capacity
    
    if (capacity == 0) { // if no capacity is left, return best solution
        return s_best;
    }
    
    // apply lazy evaluations to delete items from the candidate set if they can not fit and order the remaining items according to lazy relative gain
    auto [new_C, current_gains] = LE(S, C, s_value, s_best, capacity, previous_gains);
    
    // Check with a heuristic if the current node can be pruned
    auto SUB_result = SUB_LECR(new_C, capacity, current_gains);  
    float sub = s_value + SUB_result.first;
    if (sub <= s_best) { // if the current objective value + upper bound is less than current best, we can prune the node
        return s_best;
    }

    // apply candidate reduction
    new_C = CR_LE(new_C, s_value, SUB_result.second, s_best, capacity, current_gains);
    
    for (size_t i = 0, n = new_C.size(); i < n; ++i) { // iterate over all items in the candidate set
        std::vector<int> new_S = S; // new_S = S+item i
        new_S.push_back(new_C[i]);
        // recursiv call of search
        s_best = search(new_S, std::vector<int>(new_C.begin() + i + 1, new_C.end()), B, s_best, s_weight + items[new_C[i]].weight, &current_gains);
    }
    return s_best;
}

float LECRSolver::solve(const std::vector<int>& C, int B) {
    float s_best = search({}, C, B, 0, 0);
    return s_best;
}
