#ifndef KDTREE_H
#define KDTREE_H

#include <iostream>
#include <cmath>
#include <queue>
#include <vector>
#include <algorithm> // For std::nth_element
#include "Vector.h"

using namespace std;

// 1. THE OPTIMIZED NODE
struct KDNode {
    const Vector* data; // ZERO-COPY: Only stores a memory address (8 bytes) instead of a heavy object
    KDNode* left;
    KDNode* right;

    KDNode(const Vector* vec) : data(vec), left(nullptr), right(nullptr) {}
};

// 2. THE OPTIMIZED TREE
class KDTree {
private:
    KDNode* root;

    // CONCEPT: O(N) Median Finding & Perfectly Balanced Building
    // We pass the vector of pointers by reference so we don't copy it.
    KDNode* buildRec(vector<const Vector*>& pts, int start, int end, int depth) {
        if (start >= end) return nullptr;

        int axis = depth % pts[0]->values.size();
        int mid = start + (end - start) / 2;

        // THE FLEX: std::nth_element is an O(N) algorithm that partially sorts the array
        // just enough to put the true median at 'mid', with smaller items to the left and larger to the right.
        nth_element(pts.begin() + start,
                    pts.begin() + mid,
                    pts.begin() + end,
                    [axis](const Vector* a, const Vector* b) {
                        return a->values[axis] < b->values[axis];
                    });

        // The median becomes the root of this subtree
        KDNode* node = new KDNode(pts[mid]);

        // Recursively build perfectly balanced left and right halves
        node->left = buildRec(pts, start, mid, depth + 1);
        node->right = buildRec(pts, mid + 1, end, depth + 1);

        return node;
    }

    void nearestRec(KDNode* node, const Vector& query, int depth, int k, priority_queue<SearchResult>& top_k) {
        if (node == nullptr) return;

        // Note the arrow operator (->) because data is now a pointer!
        float current_dist = query.distance(*(node->data));
        top_k.push({node->data->id, current_dist});

        if (top_k.size() > (size_t)k) {
            top_k.pop();
        }

        int axis = depth % query.values.size();
        bool go_left = query.values[axis] < node->data->values[axis];

        KDNode* first_path = go_left ? node->left : node->right;
        KDNode* second_path = go_left ? node->right : node->left;

        nearestRec(first_path, query, depth + 1, k, top_k);

        float plane_dist = std::abs(query.values[axis] - node->data->values[axis]);

        if (top_k.size() < (size_t)k || plane_dist < top_k.top().distance) {
            nearestRec(second_path, query, depth + 1, k, top_k);
        }
    }

    void destroyTree(KDNode* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    KDTree() : root(nullptr) {}

    ~KDTree() {
        destroyTree(root);
    }

    // THE NEW BUILD COMMAND: Takes a flat list of pointers and builds a balanced tree
    void build(vector<const Vector*>& points) {
        destroyTree(root); // Wipe existing tree if any
        root = nullptr;
        if (points.empty()) return;

        root = buildRec(points, 0, points.size(), 0);
    }

    vector<SearchResult> searchTopK(const Vector& query, int k) {
        priority_queue<SearchResult> top_k;
        nearestRec(root, query, 0, k, top_k);

        vector<SearchResult> results;
        while (!top_k.empty()) {
            results.push_back(top_k.top());
            top_k.pop();
        }
        reverse(results.begin(), results.end());
        return results;
    }
};

#endif //KDTREE_H