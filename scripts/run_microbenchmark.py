
# Miscellaneous
from __future__ import annotations

# Calculations and illustration
import numpy as np
import matplotlib.pyplot as plt

# Pathing and control
import subprocess
from pathlib import Path
import re

# Test info
TEST_NAME = "Benchmark_01"
RUNS_WARMUP = 20
RUNS_MEASURED = 200

# Paths
SCRIPT_P = Path(__file__).resolve().parent
ROOT = SCRIPT_P.parent
TEXT_DATA_P = ROOT / "data" / f"{TEST_NAME}_complete_test_list.txt"
PLOT_DATA_P = ROOT / "data" / f"{TEST_NAME}_results.png"
EXECUTE_P = ROOT / "build" / "vulkan_microbenchmark.exe"

# Pattern to find gpu time in terminal (what to search for in std::cout)
TIME_PATTERN = re.compile(r"GPU time:\s*([0-9]+(?:\.[0-9]+)?)")

# Function to run test once
def run_test() -> float:

    if not EXECUTE_P.exists(): raise FileNotFoundError("Executable was not found")
    testResult = subprocess.run([str(EXECUTE_P)], capture_output=True, text=True, check=True)

    time = TIME_PATTERN.search(testResult.stdout)
    if time is None: raise RuntimeError("Could not find GPU time\n")

    return float(time.group(1))

# Main initiates many runs and plots the results
def main() -> None:

    print(f"Warmup runs: {RUNS_WARMUP}\nMeasured runs: {RUNS_MEASURED}")

    # Warmup runs
    for i in range(RUNS_WARMUP):
        time = run_test()
        print(f"Warmup run {i+1} completed with time: {time} us")
    print("\nWarmup runs completed\n\nInitiating measured runs...\n")

    # Measured runs
    results: list[float] = []
    for i in range(RUNS_MEASURED):
        time = run_test()
        results.append(time)
        print(f"Run {i+1} completed with time: {time} us")
    print("\nMeasured runs completed, plotting results...\n")

    # Write results to file
    numpyResults = np.array(results, dtype=np.float32)
    with TEXT_DATA_P.open("w", encoding="utf-8") as file:
        for time in numpyResults:
            file.write(f"{time}\n")
    print(f"Measured results have been saved to: {TEXT_DATA_P}")

    # Calculate statistics
    min = np.min(numpyResults)
    max = np.max(numpyResults)
    mean = np.mean(numpyResults)
    median = np.median(numpyResults)
    std_deviation = np.std(numpyResults)

    # Print results
    print("\nBENCHMARK SUMMARY:\n")
    print(f"Lowest time: {min:.3f} us")
    print(f"Highest time: {max:.3f} us")
    print(f"Mean time: {mean:.3f} us")
    print(f"Median time: {median:.3f} us")
    print(f"Standard deviation: {std_deviation:.3f} us")

    # Plot results
    xValues = ["Min", "Max", "Mean", "Median", "Std deviation"]
    yValues = [min, max, mean, median, std_deviation]

    plt.figure(figsize=(10, 7))
    plt.bar(xValues, yValues)
    plt.title("Vulkan microbenchmark summary")
    plt.ylabel("GPU time (us)")
    plt.tight_layout()
    plt.savefig(PLOT_DATA_P, dpi=150)
    plt.show()

    print(f"\nPlot of summary has been saved to: {PLOT_DATA_P}\n")
    print(f"Vulkan microbenchmark ran successfully\n")

if __name__ == "__main__":
    main()