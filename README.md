# 🌌 Complex Quantum Network Geometries

This C++ project simulates the **evolution of complex quantum networks** following the model described by Ginestra Bianconi in [Complex Quantum Network Geometries: Evolution and Phase Transition (2015)](https://arxiv.org/abs/1503.04739).

## 📘 Theoretical Background

The simulation grows **simplicial 2-complexes** (i.e. networks built from triangles) that evolve under:
- **Fermi-Dirac statistics**: each link can host at most one extra triangle (m = 2).
- **Bose-Einstein statistics**: each link can host an unlimited number of triangles (m = ∞).

Each link has an **energy** ε determined by node energies \( \omega_i, \omega_j \), e.g.:
- Linear: \( \varepsilon_{ij} = \omega_i + \omega_j \)
- Quadratic: \( \varepsilon_{ij} = J(J + 1), \; J = \frac{\omega_i + \omega_j}{2} \)

Growth is controlled by an inverse temperature parameter \( \beta \). At high β, networks undergo structural **phase transitions**.

---

## 🧠 Key Features

- Fermi-Dirac & Bose-Einstein quantum network evolution
- Poisson-distributed node energies (configurable)
- Dynamic link selection weighted by energy + triangle count
- Automatic export of:
  - 📏 Max shortest path distance
  - 📶 Max degree \( k_{\text{max}} \)
  - 🧠 Entropy rate \( H^{[1]} \)
  - 🧩 Modularity (Louvain method)
  - 🔗 Clustering coefficient \( C \)
  - 🌀 Node curvature \( R_i \)
- Python scripts for:
  - Metric plots vs. \( \beta \)
  - Metric evolution over time
  - Error bars over trials
  - Degree, clustering, and curvature distributions
  - Automatic detection of phase transition \( \beta_c \)

---

## 🔧 Compilation

```bash
mkdir build
cd build
cmake ..
make
```

## 🚀 Run Simulations (with multiple seeds for error bars)

Edit `main.cpp` to select:

- Number of trials (default: 3)
- Beta values and network sizes

```cpp
#define ERROR_BAR_TRIALS 3
std::vector<double> betas = {0.01, 0.2, 1.0, 5.0};
std::vector<int> Ns = {5000};
```

Then run:

```bash
./quantum_net
```

This will export CSVs into `build/raw_csv/`.

## 📊 Visualize Results

```bash
python3 visualize_errorbars.py
python3 plot_network_metrics.py
```

This will generate plots in:

```
build/graphs/
```

## 📁 Project Folder Structure

```
├── CMakeLists.txt
├── main.cpp
├── node.cpp/hpp, link.cpp/hpp, triangle.cpp/hpp
├── network.cpp/hpp
├── growth_engine.cpp/hpp
├── metrics.cpp/hpp
├── plot_network_metrics.py
├── visualize_errorbars.py
├── README.md
└── build/
    ├── raw_csv/     # Exported data
    ├── graphs/      # Final plots
    └── metrics/     # Degree, curvature, clustering
```

## 📘 Reference

Bianconi, G. (2015). *Complex Quantum Network Geometries: Evolution and Phase Transition*. [arXiv:1503.04739](https://arxiv.org/pdf/1503.04739)