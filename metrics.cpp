#include "metrics.hpp"

#include <cmath>
#include <fstream>
#include <map>
#include <queue>
#include <set>

int Metrics::maxDistanceFromInitialTriangle(const Network &net) {
  if (net.nodes.size() < 3)
    return 0;

  std::set<int> seed = {0, 1, 2};
  std::map<int, int> minDistance;

  for (int s : seed) {
    std::queue<std::pair<int, int>> q;
    std::set<int> visited;
    q.push({s, 0});
    visited.insert(s);

    while (!q.empty()) {
      auto [node, dist] = q.front();
      q.pop();

      if (minDistance.find(node) == minDistance.end() ||
          dist < minDistance[node]) {
        minDistance[node] = dist;
      }

      for (int neighbor : net.adjacencyList.at(node)) {
        if (!visited.count(neighbor)) {
          visited.insert(neighbor);
          q.push({neighbor, dist + 1});
        }
      }
    }
  }

  int maxDist = 0;
  for (const auto &[node, dist] : minDistance) {
    if (dist > maxDist)
      maxDist = dist;
  }

  return maxDist;
}

int Metrics::maxDegree(const Network &net) {
  int maxDeg = 0;
  for (const auto &[nodeID, neighbors] : net.adjacencyList) {
    if ((int)neighbors.size() > maxDeg)
      maxDeg = neighbors.size();
  }
  return maxDeg;
}

double Metrics::entropyRate(const Network &net) {
  std::vector<double> weights;

  for (const auto &[key, link] : net.links) {
    if (!link.isSaturated(net.m)) {
      double w = std::exp(-net.beta * link.energy) * (1 + link.numTriangles);
      weights.push_back(w);
    }
  }

  double Z = std::accumulate(weights.begin(), weights.end(), 0.0);
  if (Z == 0.0 || weights.empty())
    return 0.0;

  double entropy = 0.0;
  for (double w : weights) {
    double p = w / Z;
    entropy -= p * std::log(p);
  }

  return entropy;
}
