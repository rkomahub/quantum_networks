#include "link.hpp"

Link::Link() : node1(-1), node2(-1), energy(0), numTriangles(0) {}

Link::Link(int n1, int n2, int energy_)
    : node1(n1), node2(n2), energy(energy_), numTriangles(1) {}

std::pair<int, int> Link::getSortedNodes() const {
  return (node1 < node2) ? std::make_pair(node1, node2)
                         : std::make_pair(node2, node1);
}

bool Link::isSaturated(int maxTriangles) const {
  return numTriangles >= maxTriangles;
}