import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import CubicSpline

# scipy is only used here for plotting, the real interpolation runs in C++

df = pd.read_csv("data/options_data.csv")
maturities = sorted(df["maturity_days"].unique())

K_STAR = 207.0
T_STAR = 130.0

# step 1 outputs from C++, hardcoded so this script runs standalone
v_j = {
    20:  0.439758,
    41:  0.432656,
    76:  0.456719,
    104: 0.475620,
    195: 0.492886,
    286: 0.503437,
    376: 0.506645,
}

fig, axes = plt.subplots(2, 4, figsize=(16, 8))
axes = axes.flatten()

colors = plt.cm.viridis(np.linspace(0, 1, len(maturities)))

for idx, T in enumerate(maturities):
    subset = df[df["maturity_days"] == T].sort_values("strike")
    strikes = subset["strike"].values
    ivs     = subset["iv"].values

    cs = CubicSpline(strikes, ivs, bc_type="natural")
    k_fine = np.linspace(strikes[0], strikes[-1], 300)

    ax = axes[idx]
    ax.plot(k_fine, cs(k_fine) * 100, color=colors[idx], linewidth=2)
    ax.scatter(strikes, ivs * 100, color=colors[idx], s=20, zorder=5)
    ax.axvline(K_STAR, color="red", linestyle="--", linewidth=1, label="K*=207")
    ax.axhline(v_j[T] * 100, color="orange", linestyle=":", linewidth=1)
    ax.set_title(f"T = {T} days")
    ax.set_xlabel("Strike ($)")
    ax.set_ylabel("IV (%)")
    ax.legend(fontsize=7)
    ax.grid(True, alpha=0.3)

# 7 maturities in a 2x4 grid, last subplot is unused
axes[-1].set_visible(False)

plt.suptitle("NVDA Implied Volatility Smile -- Calls (2026-06-06)", fontsize=14)
plt.tight_layout()
plt.savefig("output/smile_plots.png", dpi=150)
plt.close()
print("Saved: output/smile_plots.png")

# figure 2: spline across maturities to get the final IV at T*
T_vals = np.array(list(v_j.keys()))
v_vals = np.array(list(v_j.values()))

cs_T = CubicSpline(T_vals, v_vals, bc_type="natural")
T_fine = np.linspace(T_vals[0], T_vals[-1], 500)

plt.figure(figsize=(8, 5))
plt.plot(T_fine, cs_T(T_fine) * 100, color="steelblue", linewidth=2, label="Spline G(T)")
plt.scatter(T_vals, v_vals * 100, color="steelblue", s=60, zorder=5, label="v_j (Step 1 results)")
plt.scatter(T_STAR, cs_T(T_STAR) * 100, color="red", s=100, zorder=6,
            label=f"T*={T_STAR} -> {cs_T(T_STAR)*100:.4f}%")
plt.axvline(T_STAR, color="red", linestyle="--", linewidth=1)
plt.xlabel("Maturity (days)")
plt.ylabel("IV at K*=207 (%)")
plt.title("Step 2: Natural Cubic Spline in Maturity Direction (NVDA, K*=207)")
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("output/step2_spline.png", dpi=150)
plt.close()
print("Saved: output/step2_spline.png")
