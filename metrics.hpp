#pragma once
#include "network.hpp"

class Metrics {
public:
  static int maxDistanceFromInitialTriangle(const Network &net);
  static int maxDegree(const Network &net);
  static double entropyRate(const Network &net);
};
