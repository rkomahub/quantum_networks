#pragma once

#include "link.hpp"
#include "node.hpp"
#include "triangle.hpp"

#include <functional>
#include <map>
#include <random>
#include <set>
#include <utility> // for std::pair
#include <vector>

class Network {
private:
  std::function<double(int, int)> linkEnergyFunction;

public:
  std::vector<Node> nodes; // nodes[i] = (ω_i, i)
  std::map<std::pair<int, int>, Link>
      links; // (i,j) -> Link, it acts as sparse adjacency matrix
  std::vector<Triangle> triangles;            // triangles[i] = (i,j,r)
  std::map<int, std::set<int>> adjacencyList; // adjacency list

  int m = 2;         // max triangles per link
  double beta = 0.0; // inverse temperature
  std::mt19937 rng;  // random number generator

  Network(int seed = 42, int maxTriangles = 2, double betaVal = 0.0,
          std::function<double(int, int)> linkEnergyFunc = nullptr);
  // Constructor: Initializes the network with a given seed for random number;

  void initialize();                        // t=1: initial triangle
  int addNode(int energy);                  // adds node with energy ω
  void addTriangle(int i, int j, int r);    // attach triangle to (i,j)
  double computeLinkEnergy(int ωi, int ωj); // ε_ij = ω_i + ω_j

  void addAdjacency(int u, int v); // adds edge (u, v) to adjacency list

  void exportCSV(const std::string &filename) const;

  void exportEdgeList(const std::string &filename) const;

  void exportNodeCurvatures(
      const std::string &filename) const; // export curvatures of nodes
};
