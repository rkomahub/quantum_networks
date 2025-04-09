#pragma once
#include "network.hpp"

// Class responsible for growing the network by adding triangles
class GrowthEngine {
public:
  // Constructor: Takes a reference to the network to be grown
  GrowthEngine(Network &network, int lambda = 5);

  // Performs one step of growth by adding a triangle
  void growOneStep();
  void setEnergySampler(std::function<int()> sampler); // 🎯 allows switching!

private:
  Network &net; // Reference to the network being grown
  std::poisson_distribution<int>
      poissonDist;                    // Poisson distribution for random growth
  std::function<int()> energySampler; // 🧠 dynamic sampler
};