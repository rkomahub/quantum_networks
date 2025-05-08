#include "growth_engine.hpp"
#include <iostream> // for std::cout
#include <numeric>
#include <random>
#include <stdexcept> // for std::runtime_error
#include <vector>

GrowthEngine::GrowthEngine(Network &network, int lambda)
    : net(network), poissonDist(lambda) {
  energySampler = [this]() { return poissonDist(net.rng); };
}
// Constructor: Takes a reference to the network to be grown
// and initializes the Poisson distribution with a given lambda value.
// The lambda value determines the average number of triangles to be added
// in each growth step. The energySampler is initialized to sample from
// a Poisson distribution with the specified lambda value.

void GrowthEngine::setEnergySampler(std::function<int()> sampler) {
  energySampler = sampler;
}
// 🎯 allows switching!
// This function allows the user to set a custom energy sampling function
// for the growth process.
// The default is a Poisson distribution with mean 5.
// This can be replaced with any other sampling strategy.

void GrowthEngine::growOneStep() {
  std::vector<std::pair<std::pair<int, int>, double>> probabilities;
  double Z = 0.0;

  for (const auto &[key, link] : net.links) {
    if (!link.isSaturated(net.m)) {
      double w = std::exp(-net.beta * link.energy) * (1 + link.numTriangles);
      probabilities.emplace_back(key, w);
      Z += w;
    }
  }

  if (Z == 0.0 || probabilities.empty()) {
    std::cout << "⚠️ No valid links available for growth (Z = 0). Hanging "
                 "prevented.\n";
    throw std::runtime_error("No possible growth steps (Z = 0).");
  }

  // Create a discrete distribution
  std::vector<double> weights;
  for (const auto &p : probabilities)
    weights.push_back(p.second);

  std::discrete_distribution<> dist(weights.begin(), weights.end());

  // Select a link to attach a new triangle
  int idx = dist(net.rng);
  auto [i, j] = probabilities[idx].first;

  // Create a new node with random energy ω
  int ω = energySampler();      // 🔁 uses current sampler
  int newNode = net.addNode(ω); // 🎯 Add new node to the network
  net.addTriangle(i, j, newNode);
}