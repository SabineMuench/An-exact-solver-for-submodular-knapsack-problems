/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#include <vector>
#include <unordered_set>
#include <queue>
#include <functional> 
#include <set>
#include <iostream>
#include <fstream>
#include <chrono>
#include <nlohmann/json.hpp>
#include <iomanip> 
#include "Load.h"

// objective function is COV
float f1(const std::vector<int>& X) {
    std::vector<bool> covered_areas(values.size(), false); 
    float total_value = 0.0f;

    for (int v : X) {
        for (int district : districts[v]) {
            if (!covered_areas[district]) {
                covered_areas[district] = true;
                total_value += values[district];
            }
        }
    }
    return total_value; 
}

// objective function is LOC
float f2(const std::vector<int>& X) {
    if (X.empty()) {
        return 0.0f;
    }
    int m = benefits[0].size();  
    std::vector<float> max_values(m, std::numeric_limits<float>::lowest());
    for (int x : X) {  
        for (int j = 0; j < m; ++j) { 
            max_values[j] = std::max(max_values[j], benefits[x][j]);
        }
    }
    return std::accumulate(max_values.begin(), max_values.end(), 0.0f); 
}

// objective function is INF
float f3(const std::vector<int>& X) {
    if (X.empty()) return 0.0;
    std::vector<float> pro(m, 1.0);
    for (int v : X) {
        for (int j = 0; j < m; j++) {
            if (connections[v][j] == 1) {
                pro[j] *= (1 - p[v]);
            }
        }
    }
    float sum = 0.0;
    for (float val : pro) {
        if (val < 1) {
            sum += (1 - val);
        }
        else{
            sum += 0;
        }
    }
    return sum;
}
