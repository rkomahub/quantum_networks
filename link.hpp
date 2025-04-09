#pragma once
#include <utility>

class Link {
public:
  int node1;
  int node2;
  int energy;
  int numTriangles;

  // Default constructor (required for std::map safety)
  Link();

  // Main constructor
  Link(int n1, int n2, int energy_);

  // Helpers
  std::pair<int, int> getSortedNodes() const;
  bool isSaturated(int maxTriangles) const;
};
