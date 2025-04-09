#include "network.hpp"
#include <cstdlib> // for rand()
#include <fstream> // for ofstream
#include <limits>  // for numeric_limits
#include <queue>   // for queue
#include <set>     // for set

Network::Network(int seed, int maxTriangles, double betaVal,
                 std::function<double(int, int)> linkEnergyFunc)
    : m(maxTriangles), beta(betaVal) {
  rng.seed(seed);
  // Set the random number generator seed
  if (linkEnergyFunc) {
    linkEnergyFunction = linkEnergyFunc;
  } else {
    // Default: linear ε = ω_i + ω_j
    linkEnergyFunction = [](int omega_i, int omega_j) {
      return static_cast<double>(omega_i + omega_j);
    };
  }
}

void Network::initialize() {
  // Add 3 nodes and connect them in a triangle
  int id1 = addNode(rand() % 10);
  int id2 = addNode(rand() % 10);
  int id3 = addNode(rand() % 10);
  addTriangle(id1, id2, id3);
}

int Network::addNode(int energy) {
  int id = nodes.size();
  nodes.emplace_back(id, energy); // Create a new node with the given energy
  return id;
}

double Network::computeLinkEnergy(int omega_i, int omega_j) {
  return linkEnergyFunction(omega_i, omega_j);
}

void Network::addAdjacency(int u, int v) {
  adjacencyList[u].insert(v);
  adjacencyList[v].insert(u);
}

void Network::addTriangle(int i, int j, int r) {
  triangles.emplace_back(i, j, r);

  auto updateOrAddLink = [&](int u, int v) {
    auto key = std::make_pair(std::min(u, v), std::max(u, v));
    ;
    auto it = links.find(key);
    if (it == links.end()) {
      int ε = computeLinkEnergy(nodes[u].energy, nodes[v].energy);
      links[key] = Link(u, v, ε);
    } else {
      it->second.numTriangles++;
    }
  };

  updateOrAddLink(i, j);
  updateOrAddLink(i, r);
  updateOrAddLink(j, r);

  addAdjacency(i, j);
  addAdjacency(j, r);
  addAdjacency(r, i);
}

void Network::exportCSV(const std::string &filename) const {
  std::ofstream file(filename);
  file << "Source,Target,Energy,NumTriangles\n";

  for (const auto &[key, link] : links) {
    file << link.node1 << "," << link.node2 << "," << link.energy << ","
         << link.numTriangles << "\n";
  }

  file.close();
}

void Network::exportEdgeList(const std::string &filename) const {
  std::ofstream out(filename);
  out << "Source,Target\n";

  for (const auto &[key, link] : links) {
    out << link.node1 << "," << link.node2 << "\n";
  }

  out.close();
}

void Network::exportNodeCurvatures(const std::string &filename) const {
  std::ofstream out(filename);
  out << "Node,Curvature\n";

  for (const Node &node : nodes) {
    int i = node.id;
    int k = adjacencyList.at(i).size(); // degree
    int T = 0;                          // triangle count

    for (const auto &t : triangles) {
      if (t.node1 == i || t.node2 == i || t.node3 == i)
        T++;
    }

    double R = 1.0 - (k / 2.0) + (T / 3.0);
    out << i << "," << R << "\n";
  }

  out.close();
}