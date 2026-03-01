#ifndef NANOVECTORDB_H
#define NANOVECTORDB_H

#include <iostream>
#include <string>
#include <vector>
#include "Datastore.h"
#include "KDTree.h"

using namespace std;

class NanoVectorDB {
private:
    Datastore storage;
    KDTree index;
    string db_filepath;
    
    // THE FLEX: The Lazy Rebuilding Flag
    bool is_dirty; 

    void rebuildIndex() {
        vector<const Vector*> pointers;
        pointers.reserve(storage.size()); // Pre-allocate memory for speed
        
        // Grab the exact memory addresses of the vectors living in the Datastore
        for (const auto& [id, vec] : storage.getAllVectors()) {
            pointers.push_back(&vec);
        }
        
        // Build the perfectly balanced tree in one shot
        index.build(pointers);
        is_dirty = false;
        cout << "[DB Core] Index rebuilt and balanced with " << pointers.size() << " vectors." << endl;
    }

public:
    NanoVectorDB(const string& filepath) : db_filepath(filepath), is_dirty(false) {
        try {
            storage.loadFromFile(db_filepath);
            is_dirty = true; // We loaded new data, so the index needs a rebuild
            cout << "[DB Core] Boot sequence complete. Data loaded from disk." << endl;
        } catch (const exception& e) {
            cout << "[DB Core] No existing data found. Starting fresh." << endl;
        }
    }

    void insert(const Vector& vec) {
        storage.addVector(vec);
        is_dirty = true; // Mark the index as out-of-sync
    }

    vector<SearchResult> search(const Vector& query, int k) {
        // Only pay the O(N log N) rebuild cost if new data was actually added!
        if (is_dirty) {
            rebuildIndex();
        }
        return index.searchTopK(query, k);
    }

    void commit() {
        storage.saveToFile(db_filepath);
    }
    
    size_t size() const {
        return storage.size();
    }
};

#endif //NANOVECTORDB_H