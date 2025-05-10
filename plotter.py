import matplotlib.pyplot as plt
import matplotlib.animation as animation
import sys
import os
import select

# Buffers to store measurement data
xdata, ydata = [], []
stop_flag = False   # Used to stop plotting loop
finished = False    # Used to avoid redundant operations after finish

# Create plot figure and axis
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=2)  # Line object for real-time updating

# Initialization function for animation
def init():
    ax.set_xlim(0, 100)       # Initial x-axis range
    ax.set_ylim(-1, 1)        # Initial y-axis range
    return line,

# Generate a new file name avoiding overwrites
def get_next_filename(folder, base, ext):
    os.makedirs(folder, exist_ok=True)
    i = 0
    while True:
        name = f"{base}_{i}.{ext}" if i > 0 else f"{base}.{ext}"
        path = os.path.join(folder, name)
        if not os.path.exists(path):
            return path
        i += 1

# Read a single line from stdin if available
def read_stdin_line():
    if select.select([sys.stdin], [], [], 0.01)[0]:
        return sys.stdin.readline()
    return None

# Animation frame update function
def update(frame):
    global stop_flag, finished
    if finished:
        return line,

    while True:
        line_in = read_stdin_line()
        if line_in is None:
            break
        line_in = line_in.strip()
        if line_in == "STOP":
            stop_flag = True
            break
        try:
            # Parse and store numeric input
            y = float(line_in)
            xdata.append(len(xdata))
            ydata.append(y)
            print(f"Plotting {len(xdata)} → {y:.3f}")
        except ValueError:
            pass

    # Update plot with new data
    if xdata:
        line.set_data(xdata, ydata)
        ax.set_xlim(max(0, len(xdata) - 100), len(xdata))
        ymin = min(ydata[-100:])
        ymax = max(ydata[-100:])
        ax.set_ylim(ymin - 0.1, ymax + 0.1)

    # Save final plot if stop flag is triggered
    if stop_flag and plt.fignum_exists(fig.number):
        desktop = os.path.join(os.path.expanduser("~"), "Desktop", "plots")
        filename = get_next_filename(desktop, "plot", "png")

        plt.savefig(filename, dpi=300)
        print(f"Plot saved to: {filename}")
        try:
            fig.canvas.new_timer(interval=500, callbacks=[(plt.close, [fig], {})]).start()
        except Exception as e:
            print("Close fallback:", e)
        finished = True

    return line,

# Launch animation loop (200 ms interval)
ani = animation.FuncAnimation(
    fig, update,
    init_func=init,
    interval=200,
    cache_frame_data=False
)

# Plot styling
plt.title("Live Measurement Plot")
plt.xlabel("Point Index")
plt.ylabel("Value")
plt.grid(True)

# Show plot window
try:
    plt.show()
except KeyboardInterrupt:
    print("Plot interrupted by user.")

