/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#include "DCOSUBSolver.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <nlohmann/json.hpp>



float DCOSUBSolver::search(const std::vector<int>& S, const std::vector<int>& C, int B, float s_best, int s_weight) {
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

    // apply dynamic candidate ordering to delete items from the candidate set if they can not fit and order the remaining items according to relative marginal gain
    std::vector<int> new_C = DCO(S, C, s_value, capacity); 

    // Check with a heuristic if the current node can be pruned
    float sub = s_value + SUB(new_C, capacity);
    if (sub <= s_best) {
        return s_best; // if the current objective value + upper bound is less than current best, we can prune the node
    }

    for (size_t i = 0, n = new_C.size(); i < n; ++i) { // iterate over all items in the candidate set
        std::vector<int> new_S = S; // new_S = S+item i
        new_S.push_back(new_C[i]);
        // recursiv call of search
        s_best = search(new_S, {new_C.begin() + i + 1, new_C.end()}, B, s_best, s_weight + items[new_C[i]].weight);
    }

    return s_best;
}

float DCOSUBSolver::solve(const std::vector<int>& C, int B) {
    float s_best = search({}, C, B, 0, 0);
    return s_best;
}
