#pragma once

#include <array>
#include <vector>
#include <iostream>
#include <memory>

class QuadTreeNode {
public:
    static constexpr int NUM_CHILDREN = 4;

    // Constructor
    explicit QuadTreeNode(double temp = 20.0, int lvl = 0)
        : temperature(temp)
        , isLeaf(true)
        , level(lvl) {
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            children[i] = nullptr;
        }
    }

    // Destructor - clean up children
    ~QuadTreeNode() {
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            delete children[i];
            children[i] = nullptr;
        }
    }

    // Disable copy (we don't want shallow copies)
    QuadTreeNode(const QuadTreeNode&) = delete;
    QuadTreeNode& operator=(const QuadTreeNode&) = delete;

    // Allow move
    QuadTreeNode(QuadTreeNode&& other) noexcept
        : temperature(other.temperature)
        , isLeaf(other.isLeaf)
        , level(other.level) {
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            children[i] = other.children[i];
            other.children[i] = nullptr;
        }
    }

    QuadTreeNode& operator=(QuadTreeNode&& other) noexcept {
        if (this != &other) {
            // Clean up current children
            for (int i = 0; i < NUM_CHILDREN; ++i) {
                delete children[i];
                children[i] = nullptr;
            }

            // Move from other
            temperature = other.temperature;
            isLeaf = other.isLeaf;
            level = other.level;
            for (int i = 0; i < NUM_CHILDREN; ++i) {
                children[i] = other.children[i];
                other.children[i] = nullptr;
            }
        }
        return *this;
    }

    void refine() {
        if (!isLeaf) return;

        // Create 4 children
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            children[i] = new QuadTreeNode(temperature, level + 1);
        }
        isLeaf = false;
    }

    double coarsen() {
        if (isLeaf) return temperature;

        double sum = 0.0;
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            if (children[i]) {
                sum += children[i]->temperature;
            }
        }
        temperature = sum / NUM_CHILDREN;

        // Delete children
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            delete children[i];
            children[i] = nullptr;
        }
        isLeaf = true;
        return temperature;
    }

    int countNodes() const {
        if (isLeaf) return 1;
        int count = 1;
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            if (children[i]) {
                count += children[i]->countNodes();
            }
        }
        return count;
    }

    void printTree(int indent = 0) const {
        std::cout << std::string(indent, ' ')
                  << "Level " << level
                  << ", Temp: " << temperature
                  << ", Leaf: " << (isLeaf ? "Yes" : "No")
                  << ", Nodes: " << countNodes() << std::endl;

        if (!isLeaf) {
            for (int i = 0; i < NUM_CHILDREN; ++i) {
                if (children[i]) {
                    children[i]->printTree(indent + 2);
                }
            }
        }
    }

    void fillUniformGrid(std::vector<double>& buffer,
                        int width, int height,
                        int x0, int y0, int cellSize) const {
        if (isLeaf) {
            for (int dy = 0; dy < cellSize; ++dy) {
                for (int dx = 0; dx < cellSize; ++dx) {
                    int idx = (y0 + dy) * width + (x0 + dx);
                    if (idx >= 0 && idx < static_cast<int>(buffer.size())) {
                        buffer[idx] = temperature;
                    }
                }
            }
        } else {
            int halfCell = cellSize / 2;
            if (children[0]) children[0]->fillUniformGrid(buffer, width, height, x0, y0, halfCell);
            if (children[1]) children[1]->fillUniformGrid(buffer, width, height, x0 + halfCell, y0, halfCell);
            if (children[2]) children[2]->fillUniformGrid(buffer, width, height, x0, y0 + halfCell, halfCell);
            if (children[3]) children[3]->fillUniformGrid(buffer, width, height, x0 + halfCell, y0 + halfCell, halfCell);
        }
    }

    // Public data members
    double temperature;
    bool isLeaf;
    int level;
    QuadTreeNode* children[NUM_CHILDREN];
};