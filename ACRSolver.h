/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#ifndef ACRSolver_H
#define ACRSolver_H

#include "functions.h"
#include <stack>

class ACRSolver : public Solver {
public:
    float solve(const std::vector<int>& C, int B);

private:
    // search: Input solution set S, candidate set C, capacity B, value of currently best solution s_best, weight of S s_weight
    float search(const std::vector<int>& S, const std::vector<int>& C, int B, float s_best, int s_weight);

};
#endif
