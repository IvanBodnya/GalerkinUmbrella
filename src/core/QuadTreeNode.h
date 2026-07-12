#pragma once

#include <array>
#include <vector>
#include <iostream>
#include <memory>

class QuadTreeNode {
public:
    static constexpr int NUM_CHILDREN = 4;

    // Neighbor directions
    enum Direction { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3 };

    // Child indices: 0=NW, 1=NE, 2=SW, 3=SE
    enum ChildIndex { NW = 0, NE = 1, SW = 2, SE = 3 };

    // Constructor
    explicit QuadTreeNode(double temp = 20.0, int lvl = 0, int x = 0, int y = 0)
        : temperature(temp)
        , isLeaf(true)
        , level(lvl)
        , gridX(x)
        , gridY(y) {
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            children[i] = nullptr;
        }

        for (int i = 0; i < NUM_CHILDREN; ++i) {
            neighbors[i] = nullptr;
        }
    }

    // Destructor - clean up children
    ~QuadTreeNode() {
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            delete children[i];
            children[i] = nullptr;

            delete neighbors[i];
            neighbors[i] = nullptr;
        }
    }

    // Disable copy (we don't want shallow copies)
    QuadTreeNode(const QuadTreeNode&) = delete;
    QuadTreeNode& operator=(const QuadTreeNode&) = delete;

    // Allow move
    QuadTreeNode(QuadTreeNode&& other) noexcept
       : temperature(other.temperature)
       , isLeaf(other.isLeaf)
       , level(other.level)
       , gridX(other.gridX)
       , gridY(other.gridY) {
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            children[i] = other.children[i];
            other.children[i] = nullptr;
        }
        for (int i = 0; i < 4; ++i) {
            neighbors[i] = other.neighbors[i];
            other.neighbors[i] = nullptr;
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
        // Child 0 (NW): x-1, y-1
        // Child 1 (NE): x+1, y-1
        // Child 2 (SW): x-1, y+1
        // Child 3 (SE): x+1, y+1
        children[0] = new QuadTreeNode(temperature, level + 1, gridX * 2, gridY * 2);
        children[1] = new QuadTreeNode(temperature, level + 1, gridX * 2 + 1, gridY * 2);
        children[2] = new QuadTreeNode(temperature, level + 1, gridX * 2, gridY * 2 + 1);
        children[3] = new QuadTreeNode(temperature, level + 1, gridX * 2 + 1, gridY * 2 + 1);

        isLeaf = false;
    }

    // Coarsen this node
    double coarsen() {
        if (isLeaf) return temperature;

        double sum = 0.0;
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            if (children[i]) {
                sum += children[i]->temperature;
                delete children[i];
                children[i] = nullptr;
            }
        }

        temperature = sum / NUM_CHILDREN;
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
                  << ", Pos(" << gridX << "," << gridY << ")"
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

    // ========================================================================
    // Neighbor Management Functions
    // ========================================================================

    /**
     * Build all neighbor connections for the entire tree
     * Call this after any refinement/coarsening
     */
    void updateNeighbors() {
        // We need a way to find neighbors. The simplest approach:
        // 1. Collect all leaf nodes in a grid structure
        // 2. For each leaf, find its neighbors using spatial coordinates

        // For now, we'll use a simpler recursive approach
        updateNeighborsRecursive();
    }

    /**
     * Find a neighbor in a specific direction at any level
     */
    QuadTreeNode* findNeighbor(Direction dir, int levelToFind = -1) {
        // If levelToFind == -1, find the neighbor at the same level
        if (levelToFind == -1) {
            levelToFind = level;
        }

        // The algorithm:
        // 1. If this is the root, we can't go further
        // 2. Use the parent to find the neighbor
        // 3. Traverse down to the correct level

        // This is a simplified version - we'll implement the full one below
        return nullptr;
    }

    // Simple version: Only find neighbors at the same level
    void findNeighbors() {
        if (isLeaf) return;

        // For each child, find its neighbors
        for (int i = 0; i < 4; ++i) {
            if (children[i]) {
                // Find neighbor in each direction
                children[i]->neighbors[LEFT] = findNeighbor(LEFT, LEFT);
                children[i]->neighbors[RIGHT] = findNeighbor(RIGHT, RIGHT);
                // ... etc
            }
        }
    }

    void getAllLeaves(std::vector<QuadTreeNode*>& leaves) {
        if (isLeaf) {
            leaves.push_back(this);
        } else {
            for (int i = 0; i < NUM_CHILDREN; ++i) {
                if (children[i]) {
                    children[i]->getAllLeaves(leaves);
                }
            }
        }
    }

    /**
     * Build neighbors using a flat grid approach
     * This is the simplest method and works well for our AMR
     */
    void buildNeighborsUsingGrid() {
        // Step 1: Collect all leaf nodes
        std::vector<QuadTreeNode*> leaves;
        getAllLeaves(leaves);

        // Step 2: Determine the size of the flat grid
        // For simplicity, we'll use a 2D array of pointers
        // The size is 2^maxLevel x 2^maxLevel

        // Step 3: Find max level
        int maxLevel = 0;
        for (auto* leaf : leaves) {
            if (leaf->level > maxLevel) {
                maxLevel = leaf->level;
            }
        }

        int gridSize = 1 << maxLevel;  // 2^maxLevel

        // Step 4: Create flat grid
        std::vector<std::vector<QuadTreeNode*>> flatGrid(
            gridSize, std::vector<QuadTreeNode*>(gridSize, nullptr)
        );

        // Step 5: Fill flat grid with leaf nodes
        for (auto* leaf : leaves) {
            int x = leaf->gridX;
            int y = leaf->gridY;
            if (x >= 0 && x < gridSize && y >= 0 && y < gridSize) {
                flatGrid[y][x] = leaf;
            }
        }

        // Step 6: For each leaf, find neighbors
        for (auto* leaf : leaves) {
            int x = leaf->gridX;
            int y = leaf->gridY;

            // Left neighbor
            if (x > 0) {
                leaf->neighbors[LEFT] = flatGrid[y][x - 1];
            }

            // Right neighbor
            if (x < gridSize - 1) {
                leaf->neighbors[RIGHT] = flatGrid[y][x + 1];
            }

            // Top neighbor
            if (y > 0) {
                leaf->neighbors[TOP] = flatGrid[y - 1][x];
            }

            // Bottom neighbor
            if (y < gridSize - 1) {
                leaf->neighbors[BOTTOM] = flatGrid[y + 1][x];
            }
        }
    }

    // ========================================================================
    // Public data members
    // ========================================================================

    double temperature;
    bool isLeaf;
    int level;
    int gridX;  // Position in the virtual grid at this level
    int gridY;

    QuadTreeNode* children[NUM_CHILDREN];
    QuadTreeNode* neighbors[NUM_CHILDREN];  // [LEFT, RIGHT, TOP, BOTTOM]

private:
    /**
     * Recursive helper for updateNeighbors
     * This is a simplified version that works for uniform grids
     * We'll expand it for AMR in the next step
     */
    void updateNeighborsRecursive() {
        if (isLeaf) {
            // For a leaf node, find neighbors using grid position
            // This only works if we have a parent pointer (which we don't yet)
            // We'll implement a better version below
            return;
        }

        // Recurse into children
        for (int i = 0; i < NUM_CHILDREN; ++i) {
            if (children[i]) {
                children[i]->updateNeighborsRecursive();
            }
        }
    }
};