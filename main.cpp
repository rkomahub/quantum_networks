#include "growth_engine.hpp"
#include "metrics.hpp"
#include "network.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>

#define ERROR_BAR_TRIALS 6

namespace fs = std::filesystem;

void run_simulation(bool isBose, bool useQuadraticEnergy,
                    const std::string &outputPrefix, double betaVal) {
  int m = isBose ? std::numeric_limits<int>::max() : 2;

  auto linearEnergy = [](int omega_i, int omega_j) {
    return static_cast<double>(omega_i + omega_j);
  };

  auto quadraticEnergy = [](int omega_i, int omega_j) {
    double J = (omega_i + omega_j) / 2.0;
    return J * (J + 1.0);
  };

  auto selectedEnergy = useQuadraticEnergy ? quadraticEnergy : linearEnergy;
  std::string phase = isBose ? "Bose-Einstein" : "Fermi-Dirac";

  Network net(42, m, betaVal, selectedEnergy);
  net.initialize();
  GrowthEngine engine(net, 7);

  fs::path outputDir = fs::current_path() / "raw_csv";
  fs::create_directories(outputDir);

  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << betaVal;
  std::string betaFormatted = oss.str();
  std::replace(betaFormatted.begin(), betaFormatted.end(), '.', '_');

  std::string baseName =
      outputPrefix + "_beta" + betaFormatted + "_N100000_seed0";

  fs::path metricsFile = outputDir / (baseName + ".csv");
  fs::path edgesFile = outputDir / (baseName + "_edges.csv");
  fs::path curvatureFile = outputDir / (baseName + "_curvature_nodes.csv");

  std::ofstream out(metricsFile);
  out << "step,max_distance,k_max,entropy\n";

  int steps = 0;

  while ((int)net.triangles.size() < 100000) {
    try {
      engine.growOneStep(); // ✅ wrapped properly
    } catch (const std::runtime_error &e) {
      std::cerr << "❌ ERROR during growth: " << e.what() << "\n";
      break; // Don't exit(1); just stop this simulation
    }

    if (steps % 666 == 0) {
      int d = Metrics::maxDistanceFromInitialTriangle(net);
      int k = Metrics::maxDegree(net);
      double H = Metrics::entropyRate(net);
      out << steps << "," << d << "," << k << "," << H << "\n";
    }

    steps++;
  }

  out.close();

  net.exportEdgeList(edgesFile.string());
  net.exportNodeCurvatures(curvatureFile.string());

  std::cout << "[" << phase << "] β=" << betaVal
            << ", Nodes=" << net.nodes.size()
            << ", Triangles=" << net.triangles.size() << ", steps=" << steps
            << "\nExported to: " << metricsFile.filename() << ", "
            << edgesFile.filename() << ", " << curvatureFile.filename()
            << std::endl;
}

void run_beta_sweep(bool isBose, const std::vector<double> &betas,
                    const std::vector<int> &triangleTargets,
                    const std::string &outputPrefix, int seedOffset = 0) {
  auto linearEnergy = [](int omega_i, int omega_j) {
    return static_cast<double>(omega_i + omega_j);
  };

  std::string phase = isBose ? "Bose-Einstein" : "Fermi-Dirac";

  for (double beta : betas) {
    for (int N : triangleTargets) {
      Network net(42 + seedOffset, isBose ? std::numeric_limits<int>::max() : 2,
                  beta, linearEnergy);
      net.initialize();
      GrowthEngine engine(net, 7);

      int step = 0;
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(2) << beta;
      std::string betaFormatted = oss.str();
      std::replace(betaFormatted.begin(), betaFormatted.end(), '.', '_');

      fs::path outputDir = fs::current_path();
      fs::create_directories(outputDir / "raw_csv");

      fs::path tag =
          outputDir / "raw_csv" /
          (outputPrefix + "_beta" + betaFormatted + "_N" + std::to_string(N) +
           "_seed" + std::to_string(seedOffset) + ".csv");

      std::cout << "📁 Writing to: " << tag << std::endl;
      std::ofstream out(tag);
      out << "step,max_distance,k_max,entropy\n";

      while ((int)net.triangles.size() < N) {
        try {
          engine.growOneStep(); // ✅ Only one, properly wrapped
        } catch (const std::runtime_error &e) {
          std::cout << "❌ ERROR during growth: " << e.what() << "\n";
          break;
        }

        if (step % 666 == 0) {
          int d = Metrics::maxDistanceFromInitialTriangle(net);
          int k = Metrics::maxDegree(net);
          double H = Metrics::entropyRate(net);
          out << step << "," << d << "," << k << "," << H << "\n";
        }
        step++;
      }

      out.close();

      std::string nodeCurvFile =
          (outputDir / "raw_csv" /
           (outputPrefix + "_beta" + betaFormatted + "_N" + std::to_string(N) +
            "_curvature_nodes.csv"))
              .string();
      net.exportNodeCurvatures(nodeCurvFile);

      std::string edgeFile = (outputDir / "raw_csv" /
                              (outputPrefix + "_beta" + betaFormatted + "_N" +
                               std::to_string(N) + "_edges.csv"))
                                 .string();
      net.exportEdgeList(edgeFile);

      std::cout << "[" << phase << "] β=" << beta << ", N=" << N
                << ", steps=" << step << ", Nodes=" << net.nodes.size() << "\n";
    }
  }
}

int main() {
  fs::create_directories(fs::current_path() / "raw_csv");

  run_simulation(false, false, "fermi", 0.05);
  run_simulation(false, false, "fermi", 0.5);
  run_simulation(false, false, "fermi", 5.0);
  run_simulation(true, false, "bose", 0.05);
  run_simulation(true, false, "bose", 0.5);
  run_simulation(true, false, "bose", 5.0);

  if (ERROR_BAR_TRIALS > 0) {
    std::vector<double> betaValues = {0.01, 0.02, 0.03, 0.04, 0.05, 0.07, 0.1,
                                      0.2,  0.3,  0.5,  0.7,  0.9,  1.0,  2.0,
                                      3.0,  4.0,  5.0,  6.0,  7.0};
    std::vector<int> triangleTargets = {2500, 5000, 10000};
    std::vector<double> timeBeta = {0.05, 5.0};

    for (int trial = 0; trial < ERROR_BAR_TRIALS; ++trial) {
      run_beta_sweep(false, betaValues, triangleTargets, "fermi", trial * 1000);
      run_beta_sweep(true, betaValues, triangleTargets, "bose", trial * 1000);
      run_beta_sweep(false, timeBeta, {10000}, "fermi_time", trial * 1000);
      run_beta_sweep(true, timeBeta, {10000}, "bose_time", trial * 1000);
    }
  }

  // 🚀 Single realization for N = 100000 (Fermi + Bose)
  run_beta_sweep(false, {0.05, 0.5, 5.0}, {100000}, "fermi", 0); // Fermi-Dirac
  run_beta_sweep(true, {0.05, 0.5, 5.0}, {100000}, "bose", 0); // Bose-Einstein

  return 0;
}
// This code simulates a quantum network with both Bose-Einstein and Fermi-Dirac
// statistics.