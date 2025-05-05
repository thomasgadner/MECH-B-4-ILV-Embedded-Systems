import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import re
from collections import deque

# Configure the serial port
ser = serial.Serial('COM7', 9600)  # Adjust port and baud rate as needed

# Set ylim of all plots
ylim = [-25000, 25000]

# Initialize data storage
max_points = 100
ax_data = deque(maxlen=max_points)
ay_data = deque(maxlen=max_points)
az_data = deque(maxlen=max_points)

# Set up the plot
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8))
line1, = ax1.plot([], [], 'r-')
line2, = ax2.plot([], [], 'g-')
line3, = ax3.plot([], [], 'b-')

ax1.set_ylabel('AX')
ax2.set_ylabel('AY')
ax3.set_ylabel('AZ')
ax3.set_xlabel('Sample')

for ax in (ax1, ax2, ax3):
    ax.set_xlim(0, max_points)
    ax.grid(True)

# Function to update the plot
def update_plot(frame):
    try:
        # Read a line from the serial port
        line = ser.readline().decode().strip()
        
        # Extract values using regex
        match = re.search(r'AX: (-?\d+) \| AY: (-?\d+) \| AZ: (-?\d+)', line)
        if match:
            ax, ay, az = map(int, match.groups())
            
            # Append new data
            ax_data.append(ax)
            ay_data.append(ay)
            az_data.append(az)
            
            # Update plot data
            line1.set_data(range(len(ax_data)), ax_data)
            line2.set_data(range(len(ay_data)), ay_data)
            line3.set_data(range(len(az_data)), az_data)
            
            # Adjust y-axis limits if necessary
            for ax, data in zip((ax1, ax2, ax3), (ax_data, ay_data, az_data)):
                ax.set_ylim(ylim[0], ylim[1])
            
    except KeyboardInterrupt:
        ser.close()
        plt.close()
    
    return line1, line2, line3

# Create animation
ani = FuncAnimation(fig, update_plot, interval=10, blit=True)

plt.tight_layout()
plt.show()

# Close the serial port when done
ser.close()