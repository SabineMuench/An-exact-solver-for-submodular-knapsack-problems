/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#ifndef LOAD_H
#define LOAD_H

#include <vector>
#include <set>
#include <functional>

// global variables for weights
extern std::vector<int> weights;

// global variables for COV
extern std::vector<std::set<int>> districts;
extern std::vector<float> values;

// global variables for LOC
extern std::vector<std::vector<float>> benefits;

// global variables for INF
extern std::vector<float> p;  
extern std::vector<std::vector<float>> connections;
extern int m;

using LoadFunction = std::function<void()>;
extern LoadFunction loadData;

// load COV data
void loadData_f1();
// load LOC data
void loadData_f2();
// load INF data
void loadData_f3();

#endif // LOAD_H
