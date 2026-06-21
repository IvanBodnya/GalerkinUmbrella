import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
from matplotlib.animation import FuncAnimation

# Read the CSV file
df = pd.read_csv('results/temperature_history.csv')

# Extract x coordinates
x = df['x'].values

# Get time columns (all except 'x')
time_columns = [col for col in df.columns if col != 'x']
times = [float(col.split('=')[1]) for col in time_columns]

# ==========================================
# 1. Static plot - multiple time snapshots
# ==========================================
plt.figure(figsize=(10, 6))

# Plot every few time steps
step = max(1, len(time_columns) // 6)  # Show up to 6 lines
colors = plt.cm.viridis(np.linspace(0, 1, len(time_columns[::step])))

for i, col in enumerate(time_columns[::step]):
    temps = df[col].values
    plt.plot(x, temps, label=f't = {times[i*step]:.4f}s', color=colors[i])

plt.xlabel('Position (x)')
plt.ylabel('Temperature T(x,t)')
plt.title('1D Heat Equation: Temperature Distribution Over Time')
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('results/heat_equation_snapshots.png', dpi=150)
plt.show()

# ==========================================
# 2. Animated plot (shows evolution)
# ==========================================
fig, ax = plt.subplots(figsize=(10, 6))

line, = ax.plot(x, df[time_columns[0]].values, 'b-', linewidth=2)
ax.set_xlim(0, max(x))
ax.set_ylim(0, df[time_columns].max().max() * 1.1)
ax.set_xlabel('Position (x)')
ax.set_ylabel('Temperature T(x,t)')
ax.set_title('Heat Equation Evolution')
ax.grid(True, alpha=0.3)

def update(frame):
    col = time_columns[frame]
    line.set_ydata(df[col].values)
    ax.set_title(f'Heat Equation Evolution - t = {times[frame]:.4f}s')
    return line,

ani = FuncAnimation(fig, update, frames=len(time_columns), interval=100, blit=True)
ani.save('results/heat_equation_animation.gif', writer='pillow', fps=30)
plt.show()

# ==========================================
# 3. 3D Surface plot (x vs time vs temp)
# ==========================================
from mpl_toolkits.mplot3d import Axes3D

fig = plt.figure(figsize=(12, 8))
ax = fig.add_subplot(111, projection='3d')

X, T = np.meshgrid(x, times)
Z = df[time_columns].values.T  # Transpose: rows = time, cols = position

surf = ax.plot_surface(X, T, Z, cmap='plasma', edgecolor='none', alpha=0.8)
ax.set_xlabel('Position x')
ax.set_ylabel('Time t')
ax.set_zlabel('Temperature T')
ax.set_title('Heat Equation: Temperature Surface')
fig.colorbar(surf, ax=ax, shrink=0.5, aspect=10)
plt.tight_layout()
plt.savefig('results/heat_equation_3d.png', dpi=150)
plt.show()

# ==========================================
# 4. Contour plot
# ==========================================
plt.figure(figsize=(10, 6))
contour = plt.contourf(X, T, Z, levels=20, cmap='plasma')
plt.colorbar(contour, label='Temperature')
plt.xlabel('Position x')
plt.ylabel('Time t')
plt.title('Heat Equation: Contour Plot')
plt.tight_layout()
plt.savefig('results/heat_equation_contour.png', dpi=150)
plt.show()

print("\n✅ All plots saved to 'results' directory!")