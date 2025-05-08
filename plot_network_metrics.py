# plot_network_metrics.py
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import networkx as nx

os.makedirs("build/metrics", exist_ok=True)

def plot_degree_distribution(edge_file, label, outname):
    with open(edge_file, "r") as f:
        lines = f.readlines()[1:]  # Skip header
        G = nx.parse_edgelist(lines, delimiter=",", nodetype=int)
    degrees = [d for _, d in G.degree()]
    values, counts = np.unique(degrees, return_counts=True)
    plt.figure()
    plt.loglog(values, counts / counts.sum(), 'o-')
    plt.xlabel("Degree k")
    plt.ylabel("P(k)")
    plt.title(f"Degree Distribution - {label}")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"build/metrics/{outname}_degree_distribution.png", dpi=300)
    plt.close()
    print(f"✅ Saved: {outname}_degree_distribution.png")

def plot_clustering_vs_degree(edge_file, label, outname):
    with open(edge_file, "r") as f:
        lines = f.readlines()[1:]  # Skip header
        G = nx.parse_edgelist(lines, delimiter=",", nodetype=int)
    clustering = nx.clustering(G)
    degree = dict(G.degree())
    data = [(degree[n], clustering[n]) for n in G.nodes()]
    df = pd.DataFrame(data, columns=["k", "C(k)"])
    avg_ck = df.groupby("k").mean().reset_index()
    plt.figure()
    plt.plot(avg_ck["k"], avg_ck["C(k)"], 'o-')
    plt.xlabel("Degree k")
    plt.ylabel("Clustering Coefficient C(k)")
    plt.title(f"Clustering vs Degree - {label}")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"build/metrics/{outname}_clustering_vs_k.png", dpi=300)
    plt.close()
    print(f"✅ Saved: {outname}_clustering_vs_k.png")

def plot_curvature_distribution(curvature_file, label, outname):
    df = pd.read_csv(curvature_file)
    R = df["Curvature"]
    plt.figure()
    plt.hist(R, bins=100, density=True)
    plt.xlabel("Curvature R")
    plt.ylabel("P(R)")
    plt.title(f"Curvature Distribution - {label}")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"build/metrics/{outname}_curvature_distribution.png", dpi=300)
    plt.close()
    print(f"✅ Saved: {outname}_curvature_distribution.png")

# --- Batch run for β = 0.05, 0.5, 5.0 ---
cases = ["fermi", "bose"]
betas = ["0_05", "0_50", "5_00"]

def plot_adjacency_matrix(edge_file, label, outname, max_nodes=1000):
    with open(edge_file, "r") as f:
        lines = f.readlines()[1:]  # Skip header
        G = nx.parse_edgelist(lines, delimiter=",", nodetype=int)

    # Take only a subset of the nodes
    nodes = sorted(G.nodes())[:max_nodes]
    G_sub = G.subgraph(nodes)
    A = nx.to_numpy_array(G_sub, nodelist=nodes)

    plt.figure(figsize=(8, 8))
    plt.imshow(A, cmap="viridis", interpolation="nearest")
    plt.title(f"Adjacency Matrix - {label} (Top {max_nodes} nodes)")
    plt.xlabel("Node index")
    plt.ylabel("Node index")
    plt.tight_layout()
    plt.savefig(f"build/metrics/{outname}_adjacency_matrix.png", dpi=300)
    plt.close()
    print(f"✅ Saved: {outname}_adjacency_matrix.png")

for case in cases:
    for beta_str in betas:
        tag = f"beta{beta_str}_N100000_seed0"
        prefix = f"build/raw_csv/{case}_{tag}"
        label = f"{case.capitalize()} β={beta_str.replace('_', '.')}"

        if not os.path.exists(f"{prefix}_edges.csv"):
            print(f"⚠️  Skipping {prefix} — edges file not found.")
            continue

        plot_degree_distribution(f"{prefix}_edges.csv", label, f"{case}_{tag}")
        plot_clustering_vs_degree(f"{prefix}_edges.csv", label, f"{case}_{tag}")
        plot_curvature_distribution(f"{prefix}_curvature_nodes.csv", label, f"{case}_{tag}")
        plot_adjacency_matrix(f"{prefix}_edges.csv", label, f"{case}_{tag}")