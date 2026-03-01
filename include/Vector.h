#ifndef VECTOR_H
#define VECTOR_H

#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace std;

// 1. THE DATA
struct Vector {
    string id;
    vector<float> values;

    Vector(const string& input_id, const vector<float>& input_values) {
        id = input_id;
        values = input_values;
    }

    float distance(const Vector& other) const {
        if (values.empty() || other.values.empty()) {
            throw invalid_argument("Error: One or both vectors are empty!");
        }
        if (values.size() != other.values.size()) {
            throw invalid_argument("Error: Vector dimensions must match");
        }

        float sum_sq = 0.0f;
        for (size_t i = 0; i < values.size(); ++i) {
            float diff = values[i] - other.values[i];
            sum_sq += diff * diff;
        }
        return sqrt(sum_sq);
    }
};

// 2. THE SHARED RESULT STRUCT
// Now both KDTree and Datastore can see this!
struct SearchResult {
    string id;
    float distance;

    // Required by Datastore's priority_queue
    bool operator<(const SearchResult& other) const {
        return distance < other.distance;
    }
};

#endif //VECTOR_H