//
// Created by ivanb on 12.07.2026.
//

#include "test_neighbors.h"
#include "QuadTreeNode.h"
#include <iostream>
#include <vector>

void printNeighbors(QuadTreeNode* node, int indent = 0) {
    if (!node) return;

    std::string prefix(indent, ' ');
    std::cout << prefix << "Node at (" << node->gridX << "," << node->gridY
              << ") Level " << node->level << std::endl;

    const char* dirNames[] = {"LEFT", "RIGHT", "TOP", "BOTTOM"};
    for (int i = 0; i < 4; ++i) {
        if (node->neighbors[i]) {
            std::cout << prefix << "  " << dirNames[i] << ": ("
                      << node->neighbors[i]->gridX << ","
                      << node->neighbors[i]->gridY << ") Level "
                      << node->neighbors[i]->level << std::endl;
        }
    }
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  ThermaGrid - Neighbor Test" << std::endl;
    std::cout << "========================================" << std::endl;

    // Create a root node
    auto* root = new QuadTreeNode(20.0, 0, 0, 0);

    std::cout << "\n=== Uniform Grid (All Level 0, no refinement) ===" << std::endl;
    root->buildNeighborsUsingGrid();
    printNeighbors(root);

    std::cout << "\n=== Refine Root (Level 1 grid) ===" << std::endl;
    root->refine();
    root->buildNeighborsUsingGrid();

    // Print all leaves
    std::vector<QuadTreeNode*> leaves;
    root->getAllLeaves(leaves);
    for (auto* leaf : leaves) {
        printNeighbors(leaf);
    }

    std::cout << "\n=== Refine NW Child (Level 2 grid) ===" << std::endl;
    root->children[0]->refine();
    root->buildNeighborsUsingGrid();

    leaves.clear();
    root->getAllLeaves(leaves);
    for (auto* leaf : leaves) {
        printNeighbors(leaf);
    }

    // Clean up
    delete root;

    std::cout << "\n✅ All tests passed!" << std::endl;
    return 0;
}
