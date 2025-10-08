/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#include <iostream>
#include <memory>
#include <chrono>
#include "functions.h"
#include "objectivefunctions.h"
#include "Load.h"
#include "DCOSUBSolver.h"
#include "ACRSolver.h"
#include "LESolver.h"
#include "LECRSolver.h"
#include "EPSolver.h"
#include "EPCRSolver.h"
#include "LEEPSolver.h"
#include "LEEPCRSolver.h"
#include "LEgSolver.h"



int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << "<knapsack capacity> <objective_function_type> <solver_type>" << std::endl;
        return 1;
    }

    int B = std::stoi(argv[1]);  // input knapsack capacity B
    int objective_type = std::stoi(argv[2]);  // input objective function
    int solver_type = std::stoi(argv[3]);  // input solver

    std::unique_ptr<Solver> solver;

    // Choose solver
    if (solver_type == 0)
    {
        solver = std::make_unique<DCOSUBSolver>(); // Algorithm 1 
    }
    else if (solver_type == 1)
    {
        solver = std::make_unique<ACRSolver>(); // Candidate Reduction
    }
    else if (solver_type == 2)
    {
        solver = std::make_unique<LESolver>(); // Lazy Evaluations with average decision rule
    } 
    else if (solver_type == 3)
    {
        solver = std::make_unique<EPSolver>(); // Early Pruning
    }
    else if (solver_type == 4)
    {
        solver = std::make_unique<LECRSolver>(); // Lazy Evaluations with average decision rule and Candidate Reduction
    }
    else if (solver_type == 5)
    {
        solver = std::make_unique<EPCRSolver>(); // Lazy Evaluations with average decision rule and Candidate Reduction
    }
    else if (solver_type == 6)
    {
        solver = std::make_unique<LEEPSolver>(); // Lazy Evaluations with average decision rule and Candidate Reduction
    }
    else if (solver_type == 7)
    {
        solver = std::make_unique<LEEPCRSolver>(); // Lazy Evaluations with average decision rule, Early Pruning, and Candidate Reduction
    }
    else if (solver_type == 8)
    {
        solver = std::make_unique<LEgSolver>(); // Lazy Evaluations with greedy decision rule
    }
    else {
        std::cerr << "unknown solver type: " << solver_type << std::endl;
        return 1;
    }
    
    // Choose objective function and data
    if (objective_type == 0) { // COV
        solver->f = f1;
        loadData = loadData_f1;
    } 
    else if (objective_type == 1) { // LOC
        solver->f = f2;
        loadData = loadData_f2;
    } 
    else if (objective_type == 2) { // INF
        solver->f = f3;
        loadData = loadData_f3;
    }
    else {
        std::cerr << "unknown objective function: " << objective_type << std::endl;
        return 1;
    }
    
    loadData();
    std::vector<int> C;
    for (size_t i = 0, n = weights.size(); i < n; ++i) {       
            // construct item
            solver->items.push_back(Solver::Item{std::to_string(i), weights[i], solver->f({static_cast<int>(i)})});
            // add item to initial candidate set
            C.push_back(i);
        }


    // time measurement and time limit
    auto time_limit = std::chrono::seconds(3600); //time limit one hour
    startzeit = std::chrono::high_resolution_clock::now();
    end_time_limit = startzeit + time_limit;

    float best_solution = solver->solve(C, B); // call solver

    auto endzeit = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> runtime = endzeit - startzeit;

    
    // print result
    std::cout << "optimal solution value: " << best_solution << " " << "running time: " << runtime.count() << " " << "considered nodes: " << solver->counter << std::endl;

    return 0;
}
