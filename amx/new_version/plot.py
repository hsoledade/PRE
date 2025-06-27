#!/usr/bin/env python3
"""
Intel AMX Performance Analysis with Beautiful Plots
===================================================
Generates comprehensive performance analysis with matplotlib visualizations.
"""

import os
import re
import numpy as np
import matplotlib.pyplot as plt
from typing import List, Tuple, Dict

def parse_results_file(filepath: str) -> Tuple[int, int, int, float]:
    """
    Parse a results file to extract dimensions and average time.
    Dimensions are extracted from filename (times_MxKxN.ssv)
    
    Returns:
        tuple: (M, K, N, average_time_ms)
    """
    try:
        # Extract dimensions from filename
        filename = os.path.basename(filepath)
        pattern = re.compile(r'times_(\d+)x(\d+)x(\d+)\.ssv')
        match = pattern.match(filename)
        
        if not match:
            print(f"Warning: {filename} doesn't match expected pattern times_MxKxN.ssv")
            return None
        
        M, K, N = map(int, match.groups())
        
        # Read all times from file (each line is a time in ms)
        with open(filepath, 'r') as f:
            lines = f.readlines()
        
        times = []
        for line in lines:
            line = line.strip()
            if line:  # Skip empty lines
                try:
                    times.append(float(line))
                except ValueError:
                    print(f"Warning: Invalid time value in {filepath}: {line}")
        
        if not times:
            print(f"Warning: No valid times found in {filepath}")
            return None
        
        average_time = np.mean(times)
        std_time = np.std(times)
        print(f" {filename}: {M}×{K}×{N}, {len(times)} samples, avg={average_time:.3f}ms ±{std_time:.3f}ms")
        
        return M, K, N, average_time
    
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        return None

def collect_all_results(results_dir: str = "results") -> List[Tuple[int, int, int, float]]:
    """
    Collect all results from the results directory.
    
    Returns:
        list: List of (M, K, N, average_time_ms) tuples
    """
    results = []
    
    if not os.path.exists(results_dir):
        print(f"Results directory '{results_dir}' not found!")
        return results
    
    # Pattern to match times_MxKxN.ssv files
    pattern = re.compile(r'times_(\d+)x(\d+)x(\d+)\.ssv')
    
    files = [f for f in os.listdir(results_dir) if f.endswith('.ssv')]
    files.sort()  # Sort for consistent ordering
    
    print(f"🔍 Found {len(files)} .ssv files in {results_dir}/")
    
    for filename in files:
        filepath = os.path.join(results_dir, filename)
        result = parse_results_file(filepath)
        if result:
            results.append(result)
    
    print(f"Successfully parsed {len(results)} result files")
    return results

def plot_performance_analysis(results: List[Tuple[int, int, int, float]]):
    """
    Create comprehensive performance plots with beautiful visualizations.
    """
    if not results:
        print("No results to plot!")
        return
    
    # Convert to numpy arrays for easier manipulation
    data = np.array(results)
    M_vals = data[:, 0].astype(int)
    K_vals = data[:, 1].astype(int)
    N_vals = data[:, 2].astype(int)
    times = data[:, 3]
    
    # Calculate additional metrics
    flops = 2 * M_vals * K_vals * N_vals  # 2*M*K*N FLOPs for matrix multiplication
    gflops = flops / (times * 1e6)  # GFLOP/s (time in ms)
    matrix_sizes = M_vals * K_vals * N_vals
    
    # Set up the plot style
    plt.style.use('default')
    plt.rcParams.update({
        'font.size': 12,
        'axes.titlesize': 14,
        'axes.labelsize': 12,
        'xtick.labelsize': 10,
        'ytick.labelsize': 10,
        'legend.fontsize': 10,
        'figure.titlesize': 16
    })
    
    # Create figure with subplots
    fig, axes = plt.subplots(2, 2, figsize=(16, 12))
    fig.suptitle('Intel AMX Performance Analysis', fontsize=18, fontweight='bold', y=0.95)
    
    # 1. Time vs Matrix Size (Log-Log Scale)
    ax1 = axes[0, 0]
    scatter1 = ax1.scatter(matrix_sizes, times, c=gflops, cmap='viridis', s=100, alpha=0.8, edgecolors='black', linewidth=0.5)
    ax1.set_xlabel('Matrix Size (MxKxN elements)', fontweight='bold')
    ax1.set_ylabel('Execution Time (ms)', fontweight='bold')
    ax1.set_title('Execution Time vs Matrix Size', fontweight='bold', pad=20)
    ax1.set_xscale('log')
    ax1.set_yscale('log')
    ax1.grid(True, alpha=0.3, linestyle='--')
    cbar1 = plt.colorbar(scatter1, ax=ax1)
    cbar1.set_label('Performance (GFLOP/s)', fontweight='bold')
    
    # Add trend line
    log_sizes = np.log10(matrix_sizes)
    log_times = np.log10(times)
    z = np.polyfit(log_sizes, log_times, 1)
    p = np.poly1d(z)
    ax1.plot(matrix_sizes, 10**p(log_sizes), "r--", alpha=0.8, linewidth=2, label=f'Trend (slope={z[0]:.2f})')
    ax1.legend()
    
    # 2. GFLOP/s vs Matrix Size
    ax2 = axes[0, 1]
    scatter2 = ax2.scatter(matrix_sizes, gflops, c=times, cmap='plasma', s=100, alpha=0.8, edgecolors='black', linewidth=0.5)
    ax2.set_xlabel('Matrix Size (MxKxN elements)', fontweight='bold')
    ax2.set_ylabel('Performance (GFLOP/s)', fontweight='bold')
    ax2.set_title('Performance vs Matrix Size', fontweight='bold', pad=20)
    ax2.set_xscale('log')
    ax2.grid(True, alpha=0.3, linestyle='--')
    cbar2 = plt.colorbar(scatter2, ax=ax2)
    cbar2.set_label('Time (ms)', fontweight='bold')
    
    # Add horizontal line for peak performance
    peak_gflops = np.max(gflops)
    ax2.axhline(y=peak_gflops, color='red', linestyle='--', alpha=0.7, linewidth=2, label=f'Peak: {peak_gflops:.1f} GFLOP/s')
    ax2.legend()
    
    # 3. Performance by Matrix Dimensions (Square matrices if available)
    ax3 = axes[1, 0]
    square_mask = (M_vals == K_vals) & (K_vals == N_vals)
    if np.any(square_mask):
        square_sizes = M_vals[square_mask]
        square_gflops = gflops[square_mask]
        square_times = times[square_mask]
        
        # Sort by size for line plot
        sort_idx = np.argsort(square_sizes)
        
        # Performance line
        line1 = ax3.plot(square_sizes[sort_idx], square_gflops[sort_idx], 'o-', 
                        linewidth=3, markersize=8, color='blue', label='Performance (GFLOP/s)')
        ax3.set_xlabel('Matrix Dimension (N for NxNxN)', fontweight='bold')
        ax3.set_ylabel('Performance (GFLOP/s)', color='blue', fontweight='bold')
        ax3.set_title('Square Matrix Performance Scaling', fontweight='bold', pad=20)
        ax3.grid(True, alpha=0.3, linestyle='--')
        ax3.tick_params(axis='y', labelcolor='blue')
        
        # Add secondary y-axis for time
        ax3_twin = ax3.twinx()
        line2 = ax3_twin.plot(square_sizes[sort_idx], square_times[sort_idx], 's-', 
                             color='red', alpha=0.8, linewidth=2, markersize=6, label='Time (ms)')
        ax3_twin.set_ylabel('Execution Time (ms)', color='red', fontweight='bold')
        ax3_twin.tick_params(axis='y', labelcolor='red')
        
        # Combined legend
        lines = line1 + line2
        labels = [l.get_label() for l in lines]
        ax3.legend(lines, labels, loc='best')
    else:
        ax3.text(0.5, 0.5, 'No Square Matrices Found\n(M=K=N)', 
                ha='center', va='center', transform=ax3.transAxes, 
                fontsize=14, bbox=dict(boxstyle="round,pad=0.5", facecolor="lightgray"))
        ax3.set_title('Square Matrix Performance', fontweight='bold', pad=20)
    
    # 4. Performance Distribution and Statistics
    ax4 = axes[1, 1]
    
    # Create histogram of performance
    n_bins = min(20, len(gflops))
    counts, bins, patches = ax4.hist(gflops, bins=n_bins, alpha=0.7, color='skyblue', edgecolor='black')
    ax4.set_xlabel('Performance (GFLOP/s)', fontweight='bold')
    ax4.set_ylabel('Number of Matrices', fontweight='bold')
    ax4.set_title('Performance Distribution', fontweight='bold', pad=20)
    ax4.grid(True, alpha=0.3, linestyle='--')
    
    # Add vertical lines for statistics
    mean_gflops = np.mean(gflops)
    median_gflops = np.median(gflops)
    ax4.axvline(mean_gflops, color='red', linestyle='--', linewidth=2, label=f'Mean: {mean_gflops:.1f}')
    ax4.axvline(median_gflops, color='orange', linestyle='--', linewidth=2, label=f'Median: {median_gflops:.1f}')
    ax4.legend()
    
    plt.tight_layout()
    
    # Save the plot with high quality
    plt.savefig('amx_performance_analysis.png', dpi=300, bbox_inches='tight', facecolor='white')
    plt.savefig('amx_performance_analysis.pdf', bbox_inches='tight', facecolor='white')
    print("Plots saved as:")
    print("   - amx_performance_analysis.png (high resolution)")
    print("   - amx_performance_analysis.pdf (vector format)")
    
    plt.show()

def print_detailed_results(results: List[Tuple[int, int, int, float]]):
    """
    Print detailed results table with beautiful formatting.
    """
    if not results:
        return
    
    print("\n" + "="*90)
    print("DETAILED PERFORMANCE ANALYSIS")
    print("="*90)
    print(f"{'Matrix':<15} {'Time (ms)':<12} {'GFLOP/s':<12} {'Elements':<12} {'Efficiency':<12} {'Rank':<8}")
    print("-"*90)
    
    # Calculate performance metrics
    perf_data = []
    for M, K, N, time_ms in results:
        flops = 2 * M * K * N
        gflops_val = flops / (time_ms * 1e6)
        elements = M * K * N
        perf_data.append((M, K, N, time_ms, gflops_val, elements))
    
    # Sort by performance (descending)
    perf_data.sort(key=lambda x: x[4], reverse=True)
    max_gflops = perf_data[0][4] if perf_data else 1
    
    for rank, (M, K, N, time_ms, gflops_val, elements) in enumerate(perf_data, 1):
        matrix_str = f"{M}x{K}x{N}"
        efficiency = (gflops_val / max_gflops) * 100
        
        # Add emoji for top performers
        rank_str = f"#{rank}"
        if rank == 1:
            rank_str = "🥇"
        elif rank == 2:
            rank_str = "🥈" 
        elif rank == 3:
            rank_str = "🥉"
        
        print(f"{matrix_str:<15} {time_ms:<12.3f} {gflops_val:<12.1f} {elements:<12,} {efficiency:<11.1f}% {rank_str:<8}")

def save_csv_results(results: List[Tuple[int, int, int, float]], filename: str = "amx_results.csv"):
    """
    Save results to CSV with comprehensive metrics.
    """
    try:
        with open(filename, 'w') as f:
            f.write("Matrix_Size,M,K,N,Time_ms,GFLOPS,Matrix_Elements,FLOPs,Efficiency_Percent\n")
            
            # Calculate max performance for efficiency
            max_gflops = 0
            gflops_list = []
            for M, K, N, time_ms in results:
                flops = 2 * M * K * N
                gflops = flops / (time_ms * 1e6)
                gflops_list.append(gflops)
                max_gflops = max(max_gflops, gflops)
            
            for i, (M, K, N, time_ms) in enumerate(results):
                matrix_size = f"{M}x{K}x{N}"
                flops = 2 * M * K * N
                gflops = gflops_list[i]
                elements = M * K * N
                efficiency = (gflops / max_gflops) * 100
                
                f.write(f"{matrix_size},{M},{K},{N},{time_ms:.6f},{gflops:.3f},{elements},{flops},{efficiency:.2f}\n")
        
        print(f"Detailed results saved to {filename}")
    except Exception as e:
        print(f"Error saving CSV: {e}")

def main():
    """
    Main function to analyze AMX benchmark results with beautiful visualizations.
    """
    print("Analyzing Intel AMX benchmark results...")
    print("Generating comprehensive performance analysis with plots...")
    
    # Collect all results
    results = collect_all_results("results")
    
    if not results:
        print("No valid results found!")
        print("Make sure you have .ssv files in the 'results/' directory")
        return
    
    # Print detailed results
    print_detailed_results(results)
    
    # Create comprehensive plots
    plot_performance_analysis(results)
    
    # Save CSV for further analysis
    save_csv_results(results)
    
    print("\nAnalysis complete!")
    print("Check 'amx_results.csv' for detailed data")
    print("Check the generated PNG/PDF files for visualizations")

if __name__ == "__main__":
    main()