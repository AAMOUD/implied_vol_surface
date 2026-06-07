# Implied Volatility Surface Interpolation

Reconstructs a smooth **two-dimensional implied volatility (IV) surface** from option market data using cubic spline interpolation.

## Overview

Given a discrete set of option strikes and maturities with known implied volatilities, this project constructs a continuous IV surface and evaluates it at arbitrary (strike, maturity) points not in the original data grid.

**Test case:** NVDA call options collected on 2026-06-06, with target evaluation point (K*, T*) = (207.0, 130.0).

## Algorithm

The surface is built in two interpolation steps:

1. **Strike direction (horizontal):** For each maturity, fit a cubic spline to IV values across strikes. Evaluate at target strike K* to get IV(T_i, K*).
2. **Maturity direction (vertical):** Fit a cubic spline to the step-1 results across maturities. Evaluate at target maturity T* to get final IV(T*, K*).

### Cubic Spline Solver

- **Method:** Natural cubic splines (second derivative = 0 at boundaries)
- **Linear system:** Tridiagonal system solved via Thomas algorithm (O(n) complexity)
- **Boundary conditions:** Zero second derivatives at domain endpoints

## Project Structure

```
src/
  main.cpp              # orchestrates two-step interpolation
  cubic_spline.cpp/.h   # Cubic spline builder and evaluator
  thomas.cpp/.h         # O(n) tridiagonal matrix solver
  data_loader.cpp/.h    

data/
  data_description.txt  
  options_data.csv      

output/
  result.txt            

tools/
  fetch_data.py         
  visualize.py          

.gitignore             
```

## Input Data

**options_data.csv** format:
```
maturity_days,strike,iv
20,180,0.4857
20,190,0.4623
20,200,0.4201
...
```

**Metadata** (data_description.txt):
- Underlying: NVIDIA (NVDA)
- Spot price: $205.10
- Maturities: 20, 41, 76, 104, 195, 286, 376 days
- Strike range: ±30% of spot

## Build & Run

### Requirements
- C++17 or later
- Standard library (vector, fstream, sstream, algorithm, stdexcept)

### Compile
```bash
g++ -std=c++17 -o implied_vol src/main.cpp src/cubic_spline.cpp src/thomas.cpp src/data_loader.cpp
```

Or with clang:
```bash
clang++ -std=c++17 -o implied_vol src/main.cpp src/cubic_spline.cpp src/thomas.cpp src/data_loader.cpp
```

### Run
```bash
./implied_vol
```

### Output
- **Console:** Step 1 results (IV at target strike for each maturity) and final interpolated IV
- **File:** `output/result.txt` — summary of step-1 values and final result

**Example output:**
```
Step 1 results:
  T=20 | v=0.450123 (45.0123%)
  T=41 | v=0.435678 (43.5678%)
  ...

sigma_IV(T=130, K=207) = 0.442891 (44.2891%)
```

## Key Algorithms

### Thomas Algorithm (Tridiagonal Solver)
Solves Ax = b where A is tridiagonal in O(n) time:
- **Forward sweep:** Eliminate lower diagonal
- **Back substitution:** Recover solution

Reference: TDMA (Tri-Diagonal Matrix Algorithm)

### Cubic Spline Interpolation
Constructs C² continuous piecewise-cubic polynomials between knots:
- Each segment defined by 4 coefficients (cubic polynomial)
- Continuity of function, first, and second derivatives enforced
- Natural BC: f''(x_0) = f''(x_n) = 0

## Python Tools (Optional)

Located in `tools/`:

- **fetch_data.py** — Download live option chains from Yahoo Finance
- **visualize.py** — 3D/contour plot of interpolated IV surface

(Requires: `pandas`, `matplotlib`, `scipy`)

## References

- Cubic splines: de Boor, "A Practical Guide to Splines" (1978)
- Thomas algorithm: Tri-diagonal matrix decomposition for efficient O(n) solution
- Implied volatility: Black-Scholes IV extracted from option prices via numerical root-finding


