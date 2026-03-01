#ifndef DATASTORE_H
#define DATASTORE_H

#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "Vector.h"

using namespace std;

class Datastore {
private:
    unordered_map<string, Vector> storage;

public:
    Datastore() = default;

    void addVector(const Vector& v) {
        storage.insert({v.id, v});
    }

    Vector& getVector(const string& id) {
        if (storage.find(id) == storage.end()) {
            throw invalid_argument("ID not found in Datastore: " + id);
        }
        return storage.at(id);
    }

    // ==========================================
    // THE SENIOR FIX: PURE BINARY SERIALIZATION
    // ==========================================

    void saveToFile(const string& filename) {
        // ios::binary tells C++ not to format this as text
        ofstream out(filename, ios::binary);
        if (!out.is_open()) {
            throw runtime_error("Failed to open file for writing: " + filename);
        }

        // 1. Write the total number of vectors first so we know what to expect when loading
        size_t num_vectors = storage.size();
        out.write(reinterpret_cast<const char*>(&num_vectors), sizeof(size_t));

        for (const auto& [id, vec] : storage) {
            // 2. Write the size of the ID string, then the string itself
            size_t id_len = vec.id.size();
            out.write(reinterpret_cast<const char*>(&id_len), sizeof(size_t));
            out.write(vec.id.c_str(), id_len);

            // 3. Write the number of dimensions
            size_t dim = vec.values.size();
            out.write(reinterpret_cast<const char*>(&dim), sizeof(size_t));

            // 4. THE FLEX: Dump the entire array of floats directly from RAM to Disk in ONE operation
            out.write(reinterpret_cast<const char*>(vec.values.data()), dim * sizeof(float));
        }
        out.close();
        cout << "[Datastore] Successfully saved " << num_vectors << " vectors to BINARY disk." << endl;
    }

    void loadFromFile(const string& filename) {
        ifstream in(filename, ios::binary);
        if (!in.is_open()) {
            throw runtime_error("Failed to open file for reading: " + filename);
        }

        storage.clear(); // Wipe current memory before loading

        // 1. Read how many vectors are in this file
        size_t num_vectors;
        if (!in.read(reinterpret_cast<char*>(&num_vectors), sizeof(size_t))) {
            return; // Empty file
        }

        for (size_t i = 0; i < num_vectors; ++i) {
            // 2. Read the ID
            size_t id_len;
            in.read(reinterpret_cast<char*>(&id_len), sizeof(size_t));
            string id(id_len, '\0'); // Create a blank string of the exact right size
            in.read(&id[0], id_len);

            // 3. Read the dimensions
            size_t dim;
            in.read(reinterpret_cast<char*>(&dim), sizeof(size_t));

            // 4. Read the raw floats straight into a new array
            vector<float> vals(dim);
            in.read(reinterpret_cast<char*>(vals.data()), dim * sizeof(float));

            // Reconstruct and save
            addVector(Vector(id, vals));
        }
        in.close();
        cout << "[Datastore] Successfully loaded " << num_vectors << " vectors from BINARY disk." << endl;
    }

    vector<SearchResult> search(const Vector& query, int k) {
        priority_queue<SearchResult> top_k;
        for (const auto& [id, vec] : storage) {
            float dist = query.distance(vec);
            top_k.push({id, dist});
            if (top_k.size() > (size_t)k) top_k.pop();
        }
        vector<SearchResult> results;
        while (!top_k.empty()) {
            results.push_back(top_k.top());
            top_k.pop();
        }
        reverse(results.begin(), results.end());
        return results;
    }

    size_t size() const { return storage.size(); }

    const unordered_map<string, Vector>& getAllVectors() const { return storage; }
};

#endif //DATASTORE_H