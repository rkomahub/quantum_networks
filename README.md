# Complex Quantum Network Geometries

This project simulates the evolution of complex quantum networks following the model described by Ginestra Bianconi in ["Complex Quantum Network Geometries: Evolution and Phase Transition"](https://arxiv.org/pdf/1503.04739).

## 🧠 Key Features

- Fermi-Dirac and Bose-Einstein statistics
- Linear energy model for link formation
- Growth driven by inverse temperature parameter \( \beta \)
- Automatic export of:
  - Max shortest distance
  - Maximum node degree \( k_{\text{max}} \)
  - Entropy rate \( H^{[1]} \)
  - Modularity \( M \)
  - Clustering coefficient \( C \)
  - Node curvature \( R_i \)
- Visualization scripts with error bars from multiple trials
- Automatic detection of phase transition \( \beta_c \)

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
python3 visualize.py
```

This will generate plots in:

```
build/graphs/
```

## 📁 Project Folder Structure

```
quantum_net/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── network.cpp / .hpp
│   ├── metrics.cpp / .hpp
│   └── ...
├── visualize_errorbars.py
├── README.md
└── build/
    ├── raw_csv/              # Exported CSVs from C++
    ├── graphs/               # Plots from Python
    └── metrics/              # Intermediate metric results (optional)
```

## 📘 Reference

Bianconi, G. (2015). *Complex Quantum Network Geometries: Evolution and Phase Transition*. [arXiv:1503.04739](https://arxiv.org/pdf/1503.04739)
