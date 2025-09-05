/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#include "Load.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// global variables for weights
std::vector<int> weights;
// global variables for COV
std::vector<std::set<int>> districts;
std::vector<float> values;

// global variables for LOC
std::vector<std::vector<float>> benefits;

// global variables for INF
std::vector<float> p;  
std::vector<std::vector<float>> connections;
int m;

// load COV data
void loadData_f1() {
    std::ifstream file;
    json area_values_sets, district_sets, weight_sets;

    file.open("COV_Groundsetvalues.json");
    file >> area_values_sets;
    file.close();

    file.open("COV_Items.json");
    file >> district_sets;
    file.close();

    file.open("COV_Itemsweights.json");
    file >> weight_sets;
    file.close();

    values.clear();
    for (const auto& val : area_values_sets) {
        values.push_back(val.get<float>());
    }

    districts.clear();
    for (const auto& district : district_sets) {
        std::set<int> dset;
        for (const auto& val : district) {
            dset.insert(val.get<int>());
        }
        districts.push_back(dset);
    }

    weights.clear();
    for (const auto& weight : weight_sets) {
        int int_weight = static_cast<int>(weight.get<float>());
        weights.push_back(int_weight);
    }
}

// load LOC data
void loadData_f2() {
    std::ifstream client_benefits_file("LOC_benefits.json");
    std::ifstream location_weights_file("LOC_weights.json");

    json matrix_list, gewichtssaetze;
    client_benefits_file >> matrix_list;
    location_weights_file >> gewichtssaetze;

    

    benefits = matrix_list;

    weights.clear();  
    // Prepare weights for the knapsack
    for (const auto& weight : gewichtssaetze) {
        int int_weight = static_cast<int>(weight.get<float>());
        weights.push_back(int_weight);
    }
}

// load INF data
void loadData_f3() {
    std::ifstream connections_file("INF_connections.json");
    std::ifstream weights_file("INF_weights.json");
    std::ifstream probability_file("INF_probability.json");

    json matrix_list, gewichtssaetze, probability;
    connections_file >> matrix_list;
    weights_file >> gewichtssaetze;
    probability_file >> probability;

    connections = matrix_list; 
    
    for (float value : probability) {
        p.push_back(value);  
    }
    
    int U = connections.size();
    m = connections[0].size();

    weights.clear();  
    
    // Prepare weights for the knapsack
    for (const auto& weight : gewichtssaetze) {
        int int_weight = static_cast<int>(weight.get<float>());
        weights.push_back(int_weight);
    }
}

// default settings for loadData
LoadFunction loadData = loadData_f1;
