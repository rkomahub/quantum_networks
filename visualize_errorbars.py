import os
import glob
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import networkx as nx
from networkx.algorithms.community import modularity
import community as community_louvain

os.makedirs("build/graphs", exist_ok=True)

# Detect phase transition
def detect_phase_transition(metric_data, metric_name):
    print(f"\n🔍 Detecting β_c from {metric_name}...")
    for N, beta_map in metric_data.items():
        betas = sorted(beta_map.keys())
        means = [np.mean(beta_map[b]) for b in betas]
        if len(betas) < 3:
            print(f"⚠️ Not enough points for N={N}")
            continue
        # Compute first-order differences
        deltas = np.diff(means)
        beta_mids = [(betas[i] + betas[i+1]) / 2 for i in range(len(deltas))]
        # Detect β with maximum change
        max_idx = np.argmax(np.abs(deltas))
        beta_c = beta_mids[max_idx]
        print(f"🧠 Estimated β_c for N={N} from {metric_name}: β_c ≈ {beta_c:.3f}")

# Extract max_distance with error bars
def extract_distance_data_with_error(prefix, Ns):
    data = {N: {} for N in Ns}
    for file in glob.glob(f"{prefix}_beta*_N*_seed*.csv"):
        try:
            parts = file.split("_")
            beta_str = parts[-4] + "." + parts[-3]
            beta = float(beta_str.replace("beta", ""))
            N = int(parts[-2].replace("N", ""))
        except:
            continue
        if N not in data:
            continue
        df = pd.read_csv(file)
        if not df.empty and "max_distance" in df.columns:
            val = df["max_distance"].iloc[-1]
            data[N].setdefault(beta, []).append(val)
    return data

def extract_kmax_data_with_error(prefix, Ns):
    data = {N: {} for N in Ns}
    for file in glob.glob(f"{prefix}_beta*_N*_seed*.csv"):
        try:
            parts = file.split("_")
            beta_str = parts[-4] + "." + parts[-3]
            beta = float(beta_str.replace("beta", ""))
            N = int(parts[-2].replace("N", ""))
        except:
            continue
        if N not in data:
            continue
        df = pd.read_csv(file)
        if not df.empty and "k_max" in df.columns:
            val = df["k_max"].iloc[-1]
            data[N].setdefault(beta, []).append(val)
    return data

def extract_entropy_data_with_error(prefix, Ns):
    data = {N: {} for N in Ns}
    for file in glob.glob(f"{prefix}_beta*_N*_seed*.csv"):
        try:
            parts = file.split("_")
            beta_str = parts[-4] + "." + parts[-3]
            beta = float(beta_str.replace("beta", ""))
            N = int(parts[-2].replace("N", ""))
        except:
            continue
        if N not in data:
            continue
        df = pd.read_csv(file)
        if not df.empty and "entropy" in df.columns:
            val = df["entropy"].iloc[-1]
            data[N].setdefault(beta, []).append(val)
    return data

def compute_modularity_from_csv(file):
    df = pd.read_csv(file)
    if df.empty:
        return None
    G = nx.Graph()
    for _, row in df.iterrows():
        G.add_edge(int(row["Source"]), int(row["Target"]))
    if G.number_of_edges() == 0 or G.number_of_nodes() == 0:
        return None
    partition = community_louvain.best_partition(G)
    return modularity(G, [{n for n, comm in partition.items() if comm == c}
                          for c in set(partition.values())])

def extract_modularity_data_with_error(prefix, Ns):
    data = {N: {} for N in Ns}
    for file in glob.glob(f"{prefix}_beta*_N*_edges.csv"):
        try:
            beta_str = file.split("_beta")[1].split("_")[0].replace("_", ".")
            beta = float(beta_str)
            N = int(file.split("_N")[1].split("_")[0])
        except:
            continue
        if N not in data:
            continue
        mod = compute_modularity_from_csv(file)
        if mod is not None:
            data[N].setdefault(beta, []).append(mod)
    return data

def compute_clustering_from_csv(file):
    df = pd.read_csv(file)
    if df.empty:
        return None
    G = nx.Graph()
    for _, row in df.iterrows():
        G.add_edge(int(row["Source"]), int(row["Target"]))
    if G.number_of_edges() == 0:
        return None
    return nx.average_clustering(G)

def extract_clustering_data_with_error(prefix, Ns):
    data = {N: {} for N in Ns}
    for file in glob.glob(f"{prefix}_beta*_N*_edges.csv"):
        try:
            beta_str = file.split("_beta")[1].split("_")[0].replace("_", ".")
            beta = float(beta_str)
            N = int(file.split("_N")[1].split("_")[0])
        except:
            continue
        if N not in data:
            continue
        c = compute_clustering_from_csv(file)
        if c is not None:
            data[N].setdefault(beta, []).append(c)
    return data

def plot_with_errorbars(metric_data, ylabel, title, savepath):
    plt.figure(figsize=(8, 6))
    for N, beta_map in metric_data.items():
        betas = sorted(beta_map.keys())
        means = [np.mean(beta_map[b]) for b in betas]
        stds  = [np.std(beta_map[b]) for b in betas]
        plt.errorbar(betas, means, yerr=stds, capsize=3, label=f"N={N}")
    plt.xlabel("β", fontsize=12)
    plt.ylabel(ylabel, fontsize=12)
    plt.title(title)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(savepath, dpi=300)
    plt.close()
    print(f"✅ Saved: {savepath}")

def plot_metric_vs_time_with_error(phase, prefix, metric_col, title, ylabel, outfile):
    files = sorted(glob.glob(f"{prefix}_beta*_N10000_seed*.csv"))
    grouped = {}

    for file in files:
        beta_part = file.split("_beta")[1]
        beta_str = beta_part.split("_N")[0].replace("_", ".")
        beta = float(beta_str)

        df = pd.read_csv(file)
        if df.empty or metric_col not in df.columns:
            continue

        grouped.setdefault(beta, []).append(df)

    for beta, dfs in grouped.items():
        # Align all trials on common step column
        merged = pd.concat([df.set_index("step")[metric_col] for df in dfs], axis=1)
        mean_vals = merged.mean(axis=1)
        std_vals = merged.std(axis=1)

        plt.errorbar(mean_vals.index, mean_vals, yerr=std_vals,
                     label=f"β={beta:.2f}", linewidth=2, capsize=2)

    plt.xlabel("Simulation Step", fontsize=12)
    plt.ylabel(ylabel, fontsize=12)
    plt.title(title)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(outfile, dpi=300)
    plt.close()
    print(f"✅ Saved: {outfile}")

# Callers
Ns = [2500, 5000, 10000]

# -- Max Distance (Fermi) --
d_data_fd = extract_distance_data_with_error("build/raw_csv/fermi", Ns)
plot_with_errorbars(
    d_data_fd,
    "Max Distance", "Distance vs β (Fermi-Dirac)",
    "build/graphs/distance_vs_beta_fermi-dirac.png"
)
detect_phase_transition(d_data_fd, "Distance (Fermi)")

# -- Max Distance (Bose) --
d_data_be = extract_distance_data_with_error("build/raw_csv/bose", Ns)
plot_with_errorbars(
    d_data_be,
    "Max Distance", "Distance vs β (Bose-Einstein)",
    "build/graphs/distance_vs_beta_bose-einstein.png"
)
detect_phase_transition(d_data_be, "Distance (Bose)")

# -- k_max (Fermi) --
k_data_fd = extract_kmax_data_with_error("build/raw_csv/fermi", Ns)
plot_with_errorbars(
    k_data_fd,
    r"$k_{\max}$", "Max Degree vs β (Fermi-Dirac)",
    "build/graphs/kmax_vs_beta_fermi-dirac.png"
)
detect_phase_transition(k_data_fd, "k_max (Fermi)")

# -- k_max (Bose) --
k_data_be = extract_kmax_data_with_error("build/raw_csv/bose", Ns)
plot_with_errorbars(
    k_data_be,
    r"$k_{\max}$", "Max Degree vs β (Bose-Einstein)",
    "build/graphs/kmax_vs_beta_bose-einstein.png"
)
detect_phase_transition(k_data_be, "k_max (Bose)")

# -- Entropy (Fermi) --
h_data_fd = extract_entropy_data_with_error("build/raw_csv/fermi", Ns)
plot_with_errorbars(
    h_data_fd,
    r"$H^{[1]}$", "Entropy Rate vs β (Fermi-Dirac)",
    "build/graphs/entropy_vs_beta_fermi-dirac.png"
)
detect_phase_transition(h_data_fd, "Entropy (Fermi)")

# -- Entropy (Bose) --
h_data_be = extract_entropy_data_with_error("build/raw_csv/bose", Ns)
plot_with_errorbars(
    h_data_be,
    r"$H^{[1]}$", "Entropy Rate vs β (Bose-Einstein)",
    "build/graphs/entropy_vs_beta_bose-einstein.png"
)
detect_phase_transition(h_data_be, "Entropy (Bose)")

plot_with_errorbars(
    extract_modularity_data_with_error("build/raw_csv/fermi", Ns),
    r"Modularity $M$", "Modularity vs β (Fermi-Dirac)",
    "build/graphs/modularity_vs_beta_fermi-dirac.png"
)

plot_with_errorbars(
    extract_modularity_data_with_error("build/raw_csv/bose", Ns),
    r"Modularity $M$", "Modularity vs β (Bose-Einstein)",
    "build/graphs/modularity_vs_beta_bose-einstein.png"
)

plot_with_errorbars(
    extract_clustering_data_with_error("build/raw_csv/fermi", Ns),
    r"Clustering Coefficient $C$", "Clustering vs β (Fermi-Dirac)",
    "build/graphs/clustering_vs_beta_fermi-dirac.png"
)

plot_with_errorbars(
    extract_clustering_data_with_error("build/raw_csv/bose", Ns),
    r"Clustering Coefficient $C$", "Clustering vs β (Bose-Einstein)",
    "build/graphs/clustering_vs_beta_bose-einstein.png"
)

plot_metric_vs_time_with_error(
    "Fermi-Dirac", "build/raw_csv/fermi_time", "max_distance",
    "Max Distance vs Time (Fermi-Dirac)", "Max Distance",
    "build/graphs/distance_vs_time_fermi-dirac.png"
)

plot_metric_vs_time_with_error(
    "Fermi-Dirac", "build/raw_csv/fermi_time", "k_max",
    "Max Degree vs Time (Fermi-Dirac)", r"$k_{\max}$",
    "build/graphs/kmax_vs_time_fermi-dirac.png"
)

plot_metric_vs_time_with_error(
    "Fermi-Dirac", "build/raw_csv/fermi_time", "entropy",
    "Entropy Rate vs Time (Fermi-Dirac)", r"$H^{[1]}$",
    "build/graphs/entropy_vs_time_fermi-dirac.png"
)

plot_metric_vs_time_with_error(
    "Bose-Einstein", "build/raw_csv/bose_time", "max_distance",
    "Max Distance vs Time (Bose-Einstein)", "Max Distance",
    "build/graphs/distance_vs_time_bose-einstein.png"
)

plot_metric_vs_time_with_error(
    "Bose-Einstein", "build/raw_csv/bose_time", "k_max",
    "Max Degree vs Time (Bose-Einstein)", r"$k_{\max}$",
    "build/graphs/kmax_vs_time_bose-einstein.png"
)

plot_metric_vs_time_with_error(
    "Bose-Einstein", "build/raw_csv/bose_time", "entropy",
    "Entropy Rate vs Time (Bose-Einstein)", r"$H^{[1]}$",
    "build/graphs/entropy_vs_time_bose-einstein.png"
)

print("✅ All error-bar plots saved.")