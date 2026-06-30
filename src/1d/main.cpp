// 1D Heat Equation Solver
// Solves: dT/dt = alpha * d²T/dx²

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include "../libs/matplotlibcpp.h"

using namespace std;
namespace plt = matplotlibcpp;

// ============================================
// Class to handle the mesh (grid)
// ============================================
class Mesh {
public:
    Mesh(double length, int numPoints) {
        m_length = length;
        m_numPoints = numPoints;
        m_dx = length / (numPoints - 1);  // Spacing between points
        
        // Create points along the rod
        m_points.resize(numPoints);
        for (int i = 0; i < numPoints; i++) {
            m_points[i] = i * m_dx;
        }
    }

    vector<double> getAllPoints() const { return m_points; }
    double getPoint(int i) const { return m_points[i]; }
    double getDx() const { return m_dx; }
    int getNumPoints() const { return m_numPoints; }
    
private:
    double m_length;
    double m_dx;
    int m_numPoints;
    vector<double> m_points;
};

// ============================================
// Main solver class
// ============================================
class HeatSolver {
public:
    HeatSolver(const Mesh& mesh) : m_mesh(mesh) {
        int n = mesh.getNumPoints();
        m_temperature.resize(n, 0.0);
        m_temperatureOld.resize(n, 0.0);
    }
    
    // Set initial temperature distribution
    void setInitialCondition(double initialTemp) {
        for (int i = 0; i < m_mesh.getNumPoints(); i++) {
            m_temperature[i] = initialTemp;
        }
    }
    
    // Set initial condition using a function (like sine wave)
    void setInitialConditionFromFunction(double (*func)(double)) {
        for (int i = 0; i < m_mesh.getNumPoints(); i++) {
            double x = m_mesh.getPoint(i);
            m_temperature[i] = func(x);
        }
    }
    
    // Set boundary conditions
    void setBoundaryConditions(double leftTemp, double rightTemp) {
        m_leftTemp = leftTemp;
        m_rightTemp = rightTemp;
    }
    
    // Solve using explicit method (simplest)
    void solve(double thermalDiffusivity, double dt, double totalTime) {
        double alpha = thermalDiffusivity;
        double dx = m_mesh.getDx();
        int numPoints = m_mesh.getNumPoints();
        
        // Check stability condition (CFL condition)
        double maxDt = dx * dx / (2.0 * alpha);
        if (dt > maxDt) {
            cout << "Warning: dt = " << dt << " > max stable dt = " << maxDt << endl;
            cout << "Solution may become unstable!" << endl;
        }
        
        double currentTime = 0.0;
        int iteration = 0;
        
        cout << "Starting simulation..." << endl;
        
        while (currentTime < totalTime) {
            // Save old temperature
            m_temperatureOld = m_temperature;
            
            // Apply boundary conditions
            m_temperature[0] = m_leftTemp;
            m_temperature[numPoints - 1] = m_rightTemp;
            
            // Update interior points using finite difference
            for (int i = 1; i < numPoints - 1; i++) {
                // Second derivative approximation: (T[i-1] - 2*T[i] + T[i+1]) / dx²
                double secondDerivative = (m_temperatureOld[i-1] - 2*m_temperatureOld[i] + m_temperatureOld[i+1]) / (dx * dx);
                
                // Explicit Euler update
                m_temperature[i] = m_temperatureOld[i] + alpha * dt * secondDerivative;
            }
            
            currentTime += dt;
            iteration++;
            
            // Save results every 1000 steps
            if (iteration % 100 == 0) {
                saveToFile(currentTime);
                cout << "Time: " << currentTime << ", Max temp: " << getMaxTemperature() << endl;
            }
        }
        
        // Save final result
        saveToFile(currentTime);
        cout << "Simulation complete!" << endl;
    }

    void plot() {
        plt::plot(m_mesh.getAllPoints(), m_temperature);
        plt::show();
    }
    
    // Get current temperature at a point
    double getTemperature(int i) const {
        return m_temperature[i];
    }
    
    double getMaxTemperature() const {
        double maxTemp = m_temperature[0];
        for (double temp : m_temperature) {
            if (temp > maxTemp) maxTemp = temp;
        }
        return maxTemp;
    }
    
    // Save results to CSV file
    void saveToFile(double time) {
        static int fileCounter = 0;
        string filename = "results/temp_" + to_string(fileCounter) + ".csv";
        
        ofstream file(filename);
        if (file.is_open()) {
            file << "x,temperature" << endl;
            for (int i = 0; i < m_mesh.getNumPoints(); i++) {
                file << m_mesh.getPoint(i) << "," << m_temperature[i] << endl;
            }
            file.close();
        }
        fileCounter++;
    }
    
    // Save all results to a single file (simpler)
    void saveFullHistory() {
        ofstream file("results/temperature_history.csv");
        if (!file.is_open()) {
            cout << "Error: Could not create results directory!" << endl;
            return;
        }
        
        // Write header
        file << "x";
        for (size_t i = 0; i < m_savedTimes.size(); i++) {
            file << ",t=" << m_savedTimes[i];
        }
        file << endl;
        
        // Write data
        for (int i = 0; i < m_mesh.getNumPoints(); i++) {
            file << m_mesh.getPoint(i);
            for (size_t t = 0; t < m_savedTemps.size(); t++) {
                file << "," << m_savedTemps[t][i];
            }
            file << endl;
        }
        
        cout << "Results saved to results/temperature_history.csv" << endl;
    }
    
    // Store results for later saving
    void storeCurrentResults(double time) {
        m_savedTimes.push_back(time);
        m_savedTemps.push_back(m_temperature);
    }
    
private:
    const Mesh& m_mesh;
    vector<double> m_temperature;
    vector<double> m_temperatureOld;
    double m_leftTemp = 0.0;
    double m_rightTemp = 0.0;
    
    // For saving full history
    vector<double> m_savedTimes;
    vector<vector<double>> m_savedTemps;
};

// ============================================
// Helper functions for initial conditions
// ============================================

double sineInitial(double x) {
    return sin(M_PI * x);  // Sine wave from 0 to 1
}

double constantInitial(double x) {
    return 100.0;  // Constant temperature
}

double stepInitial(double x) {
    if (x < 0.5) return 0.0;
    else return 100.0;
}

// ============================================
// Main program
// ============================================
int main() {
    cout << "======================================" << endl;
    cout << "   1D Heat Equation Solver" << endl;
    cout << "======================================" << endl;
    cout << endl;
    
    // create output directory
    std::filesystem::create_directory("results");

    // Setup: 1 meter rod with 51 points (50 segments)
    double rodLength = 1.0;
    int numPoints = 51;
    Mesh mesh(rodLength, numPoints);
    
    cout << "Mesh created:" << endl;
    cout << "  Length: " << rodLength << " m" << endl;
    cout << "  Points: " << numPoints << endl;
    cout << "  Spacing (dx): " << mesh.getDx() << " m" << endl;
    cout << endl;
    
    // Create solver
    HeatSolver solver(mesh);
    
    // Set initial condition: sine wave,
    // such that at x = 0 & 1, T = 0 degrees
    // and at x = 1/2 , T = 1 degrees
    cout << "Setting initial condition: T(x,0) = sin(pi * x)" << endl;
    solver.setInitialConditionFromFunction(sineInitial);
    
    // Set boundary conditions: 0 degrees at both ends
    cout << "Boundary conditions: T(0,t) = 0, T(1,t) = 0" << endl;
    solver.setBoundaryConditions(0.0, 0.0);
    
    // Store initial state
    solver.storeCurrentResults(0.0);
    
    // Solver parameters
    double thermalDiffusivity = 1.0;  // Material property (alpha)
    double dt = 0.0001;               // Time step (seconds)
    double totalTime = 0.1;           // Total simulation time (seconds)
    
    cout << endl;
    cout << "Simulation parameters:" << endl;
    cout << "  Thermal diffusivity: " << thermalDiffusivity << " m²/s" << endl;
    cout << "  Time step (dt): " << dt << " s" << endl;
    cout << "  Total time: " << totalTime << " s" << endl;
    cout << "  Steps: " << int(totalTime / dt) << endl;
    cout << endl;
    
    // Run simulation
    solver.solve(thermalDiffusivity, dt, totalTime);

    // Plot results
    solver.plot();

    // Save results
    solver.saveFullHistory();
    
    cout << endl;
    cout << "Simulation finished successfully!" << endl;
    cout << "Check the 'results' folder for output files." << endl;

    std::vector<double> x(mesh.getNumPoints());
    for (int i = 0; i < mesh.getNumPoints(); i++) {
        x[i] = mesh.getPoint(i);
    }

    return 0;
}