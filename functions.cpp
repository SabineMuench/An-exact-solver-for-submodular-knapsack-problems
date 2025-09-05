/* This file is part of the code presented in the paper

	"An Exact Solver for Submodular Knapsack Problems"
	
by Sabine Münch and Stephen Raach
==============================================================================*/

#include "functions.h"
#include <iostream>
#include <queue>

std::chrono::high_resolution_clock::time_point startzeit;
std::chrono::high_resolution_clock::time_point end_time_limit;

// input a vector S, an integer (item) c, and the objective value s_value of S
// output marginal gain of adding item c to S
float Solver::z(const std::vector<int>& S, const int c, float s_value) {
    std::vector<int> updated_S(S);  // updated_S is S+c
    updated_S.push_back(c);
    return f(updated_S) - s_value; // return is f(updated_S)-f(S) = f(S+c)-f(S)
}

// dynamic candidate ordering
// input a vector S, a vector c, the objective value s_value of S and the capacity left in the knapsack (capacity=B-weight(S))
// output sorted_C a vector of all items that can be added to S without exeeding the knapsack capacity ordered according to the relative marginal gain of adding an item to S
std::vector<int> Solver::DCO(const std::vector<int>& S, std::vector<int> C, const float s_value, const int capacity) {
    // delete all items from C that can not fit into the knapsack 
    C.erase(std::remove_if(C.begin(), C.end(), [this,capacity](int c) {
        return items[c].weight > capacity;  
    }), C.end());
    // Max-Heap for ordering remaining items according to relative marginal gain
    auto cmp = [this](int a, int b) {
        return (items[a].value / items[a].weight) < (items[b].value / items[b].weight);
    };

    std::priority_queue<int, std::vector<int>, decltype(cmp)> max_heap(cmp);
    // iterating over all items in C
    for (int c : C) {
        float marginal_gain = z(S, c, s_value);  // calculating marginal gain of adding c to S
        items[c].value = marginal_gain;  // updating c's value

        max_heap.push(c); // adding c to the max-heap
    }
    // extract itmes from heap to a vector
    std::vector<int> sorted_C;
    while (!max_heap.empty()) {
        sorted_C.push_back(max_heap.top());
        max_heap.pop();
    }

    return sorted_C;  
}

// SUB heuritic for pruning nodes
// input a vector C containing all items in the knapsack set and the capacity left in the knapsack (capacity = B-w(S))
// output an upper bound for the total value that can be added to the currently packed knapsack, when S is already packed
// uses greedy for solving a modular fractional knapsack problem exactly
float Solver::SUB(std::vector<int>& C, int capacity) {
    // total weight of all items in C
    int total_weight = std::accumulate(C.begin(), C.end(), 0, [this](int sum, int c) {
        return sum + items[c].weight;
    });
    // pack all items in C if total weight is less/equal to left capacity
    if (total_weight <= capacity) {
        return std::accumulate(C.begin(), C.end(), 0.0f, [this](float sum, int c) {
            return sum + items[c].value;
        });
    }
    // fractional packing according to relative increase if not all items can be packed
    float total_value = 0.0f;
    for (int c : C) { // iterating over all items in c
        const Item& item = items[c];
        if (item.weight <= capacity) { // if item c fits it is packed
            capacity -= item.weight; // left capacity is reduced
            total_value += item.value; // total_value of the knapsack is increased
        } else {
            total_value += item.value / item.weight * capacity; // if item c does not fit into the knapsack, we add as much of it as possible
            break; // knapsack capcity is reached, we stop packing
        }
    }
    return total_value;
}

// SUB_CR heuristic for pruning nodes
// exact the same as SUB, only that the set that is packed into the knapsack is also stored and returned.
std::pair<float, std::vector<int>> Solver::SUB_CR(const std::vector<int>& C, int capacity) {
    float total_value = 0.0f;
    // total weight of all items in C
    int total_weight = 0;
    for (int c : C) {
        total_weight += items[c].weight;
    }
    
    // pack all items in C if total weight is less/equal to capacity
    if (total_weight <= capacity) {
        for (int c : C) {
            total_value += items[c].value;
        }
        return {total_value, C};  // Return as std::pair
    }
    // pack according to steepest relativ gain if not all items can be packed
    std::vector<int> knapsackset;
    
    for (int c : C) {
        const Item& item = items[c];
        // add item if it fits
        if (item.weight <= capacity) {
            capacity -= item.weight;
            total_value += item.value;
            knapsackset.push_back(c);
        }
        // add fractional part if item does not fit
        else {
            float frac = item.value / item.weight * capacity;
            total_value += frac;
            break;  // break when capacity is reached
        }
    }
    return {total_value, knapsackset};  
}

// SUB_LE heuristic for pruning SUB
// exact the same as SUB, only that current_gains are used instead of item.value
float Solver::SUB_LE(std::vector<int>& C, int capacity, const std::unordered_map<int, float>& current_gains) {
    // total weight of all items in C
    int total_weight = std::accumulate(C.begin(), C.end(), 0, [this](int sum, int c) {
        return sum + items[c].weight;
    });
    // pack all items in C if total weight is less/equal to capacity
    if (total_weight <= capacity) {
        return std::accumulate(C.begin(), C.end(), 0.0f, [&](float sum, int c) {
            return sum + current_gains.at(c)*items[c].weight;
        });
    }
    // pack according to steepest relativ gain if not all items can be packed
    float total_value = 0.0f;
    for (int c : C) {
        const Item& item = items[c];
        if (item.weight <= capacity) {
            capacity -= item.weight;
            total_value += (current_gains.at(c)*items[c].weight);
        } else {
            total_value += current_gains.at(c) * capacity;
            break;
        }
    }
    return total_value;
}


// SUB_LECR heuristic for pruning SUB
// exact the same as SUB_CR, only that current_gains are used instead of item.value
std::pair<float, std::vector<int>> Solver::SUB_LECR(const std::vector<int>& C, int capacity, const std::unordered_map<int, float>& current_gains) {
    // total weight of all items in C
    int total_weight = std::accumulate(C.begin(), C.end(), 0, [this](int sum, int c) {
        return sum + items[c].weight;
    });
    
    // pack all items in C if total weight is less/equal to capacity
    if (total_weight <= capacity) {
        float value = std::accumulate(C.begin(), C.end(), 0.0f, [&](float sum, int c) {
            return sum + current_gains.at(c)*items[c].weight;
        });
        return {value, C};  // Rückgabe als std::pair
    }
    // pack according to steepest relativ gain if not all items can be packed
    std::vector<int> knapsackset;
    float total_value = 0.0f;
    for (int c : C) {
        const Item& item = items[c];
        // add item if it fits
        if (item.weight <= capacity) {
            capacity -= item.weight;
            total_value += (current_gains.at(c)*item.weight);
            knapsackset.push_back(c);
        }
        // add fractional part if item does not fit
        else {
            total_value += current_gains.at(c) * capacity;
            break;  // break when capacity is reached
        }
    }
    return {total_value, knapsackset};  
}

// CR Candidate Reduction
// Input: candidate items in vector new_C, s_value objective value of current solution S, knapsackset the set packed by pruning heuritic SUB_CR, and the left knapsack capacity
// tests if an item c can be deleted from the candidate set
// Output: updated candidate set 
std::vector<int> Solver::CR(std::vector<int>& new_C, const float s_value, const std::vector<int>& knapsackset, const float s_best, const int capacity) {
    
    if (knapsackset.empty()) { // if the knapsackset is empty, new_C is returned
        return new_C;  
    }

    // betrachten contains all items that are in new_C and not in knapsackset, since only these once need to be tested
    std::vector<int> betrachten;
    std::set_difference(new_C.begin(), new_C.end(), knapsackset.begin(), knapsackset.end(), std::back_inserter(betrachten));
 
    // iterates over betrachten
    for (int c : betrachten) {
        Item& item = items[c];
        auto SUB_result = SUB_CR(new_C, capacity - item.weight); // apply pruning heuristic to obtain an upper bound the modular knapsack when S and item c are already packed
        if (s_value + SUB_result.first + item.value <= s_best) { // if s_value + the value of c and the upper bound are less/equal to the value of the currently best solution, we do not need to consider S+c any further 
                new_C.erase(std::remove(new_C.begin(), new_C.end(), c), new_C.end()); // c is deleted from the candidate set
        }        
    } 
    return new_C; 
}

// CR_LE Candidate Reduction with Lazy Evaluations
// exact the same as CR, only that current_gains are used instead of item.value
std::vector<int> Solver::CR_LE(std::vector<int>& new_C, const float s_value, const std::vector<int>& knapsackset, const float s_best, const int capacity, const std::unordered_map<int, float>& current_gains) {
    if (knapsackset.empty()) {
        return new_C;  // if the knapsackset is empty, new_C is returned
    }
    
    // betrachten contains all items that are in new_C and not in knapsackset, since only these once need to be tested
    std::vector<int> betrachten;
    std::set_difference(new_C.begin(), new_C.end(), knapsackset.begin(), knapsackset.end(), std::back_inserter(betrachten));

    // iterates over betrachten
    for (int c : betrachten) {
        Item& item = items[c];
        auto SUB_result = SUB_LECR(new_C, capacity - item.weight,current_gains); // apply pruning heuristic to obtain an upper bound the modular knapsack when S and item c are already packed
        if (s_value + SUB_result.first + current_gains.at(c)*item.weight <= s_best) { // if s_value + the value of c and the upper bound are less/equal to the value of the currently best solution, we do not need to consider S+c any further 
            new_C.erase(std::remove(new_C.begin(), new_C.end(), c), new_C.end()); // c is deleted from the candidate set
        }  
    }
    return new_C; 
} 

// Lazy Evaluations
std::pair<std::vector<int>, std::unordered_map<int, float>> Solver::LE(
    const std::vector<int>& S,
    const std::vector<int>& C,
    const float s_value,
    const float s_best,
    const int capacity,
    const std::unordered_map<int, float>* previous_gains
) {
    std::unordered_map<int, float> current_gains; // initialize current_gains
    std::vector<std::pair<int, float>> ratios;  // initializes relative marginal gains

    bool updated = false;  // flag for breaking the update of current gains

    if (!previous_gains) { // initialize current_gains if there are no previous gains
        for (int c : C) {
            if (items[c].weight <= capacity) {
                float ratio = items[c].value / items[c].weight;
                current_gains[c] = ratio;
                ratios.push_back({c, ratio});
            }
        }
    } else {
        float R = (s_best - s_value) / capacity; // benchmark for updating current gains
        for (int c : C) {
            if (items[c].weight <= capacity) { // delete all items that are to heavy

                if (!updated && previous_gains->at(c) >= R) { // update current gain
                    items[c].value = z(S, c, s_value);  
                    current_gains[c] = items[c].value / items[c].weight;
                } else {
                    current_gains[c] = previous_gains->at(c); // if previous gain smaller than benchmark use previous gain as current gain
                    updated = true;  // flag to stop update
                }
                ratios.push_back({c, current_gains[c]});
            }
        }
    }
    // Sort according to relative gain (ratio)
    std::sort(ratios.begin(), ratios.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
        return a.second > b.second;  
    });
    // extract sorted items
    std::vector<int> sorted_filtered_C;
    for (const auto& ratio : ratios) {
        sorted_filtered_C.push_back(ratio.first);
    }
    return {sorted_filtered_C, current_gains};
}
