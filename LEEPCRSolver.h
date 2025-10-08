/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#ifndef LEEPCRSolver_H
#define LEEPCRSolver_H

#include "functions.h"
#include <stack>

class LEEPCRSolver : public Solver {
public:
    float solve(const std::vector<int>& C, int B);

private:
    // search: Input solution set S, candidate set C, capacity B, value of currently best solution s_best, weight of S s_weight, lazy evaluation gains of C
    float search(const std::vector<int>& S,const std::vector<int>& C,const int &B,float s_best,int s_weight,const std::unordered_map<int,float>* previous_gains);

};
#endif
