/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;



extern std::chrono::high_resolution_clock::time_point startzeit;
extern std::chrono::high_resolution_clock::time_point end_time_limit;

// extern std::vector<std::set<int>> districts;
extern std::vector<float> values;
extern std::vector<int> weights;

class Solver {
    public:    
    struct Item {
        std::string name;
        int weight;
        float value;
    
        Item(std::string n, int w, float v) : name(std::move(n)), weight(w), value(v) {}
    };
    
        std::vector<Item> items; 
        int counter = 0;

        // objective function f 
        std::function<float(const std::vector<int>&)> f;

        // marginal gain function z
        float z(const std::vector<int>& S, const int c, float s_value);

        // dynamic candidate ordering
        std::vector<int> DCO(const std::vector<int>& S, std::vector<int> C, const float s_value, const int capacity) ;

        // SUB heuristic for pruning nodes
        float SUB(std::vector<int>& C, int capacity);

        // SUB_CR heuritic for pruning nodes
        std::pair<float, std::vector<int>> SUB_CR(const std::vector<int>& C, int capacity);

        // SUB_LE heuristic for pruning SUB
        float SUB_LE(std::vector<int>& C, int capacity, const std::unordered_map<int, float>& current_gains);

        // SUB_LECR heuristic for pruning SUB
        std::pair<float, std::vector<int>> SUB_LECR(const std::vector<int>& C, int capacity, const std::unordered_map<int, float>& current_gains);

        // CR Candidate Reduction
        std::vector<int> CR(std::vector<int>& new_C, const float s_value, const std::vector<int>& knapsackset, const float s_best, const int capacity);

        // CR_LE Candidate Reduction with Lazy Evaluations
        std::vector<int> CR_LE(std::vector<int>& new_C, const float s_value, const std::vector<int>& knapsackset, const float s_best, const int capacity, const std::unordered_map<int, float>& current_gains);

        // Lazy Evaluations
        std::pair<std::vector<int>, std::unordered_map<int, float>> LE(
            const std::vector<int>& S,
            const std::vector<int>& C,
            const float s_value,
            const float s_best,
            const int capacity,
            const std::unordered_map<int, float>* previous_gains = nullptr);

        // default settings for solver
        Solver() = default;

        virtual ~Solver() = default;

        virtual float solve(const std::vector<int>& C, int B) = 0;
    };

    
#endif
