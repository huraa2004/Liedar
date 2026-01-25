import serial
import struct
import math
import numpy as np
import open3d as o3d
import time

# AH - Wait for UART markers (data start and end)
def wait_for_marker(ser, marker):
    line = ''
    while True:
        if ser.in_waiting:
            char = ser.read().decode('ascii')
            if char == '\n':
                if marker in line:
                    return True
                line = ''
            else:
                line += char

# AH - Initialize serial communication
try:
    ser = serial.Serial(
        port='COM3',
        baudrate=115200,       # AH - Match microcontroller baud rate
        timeout=1,             # AH - 1 second timeout
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS
    )
except serial.SerialException as e:
    print(f"AH - Error opening port: {e}")
    exit(1)

print(f"AH - Opening: {ser.name}")
time.sleep(2)  # AH - Allow port to settle

# AH - Clear communication buffers
ser.reset_input_buffer()
ser.reset_output_buffer()

# AH - Wait for start marker
print("AH - Waiting for data start marker...")
wait_for_marker(ser, "DATA STARTS")

# AH - Read scanning parameters
measurementNum = ord(ser.read(1))
numSamples = ord(ser.read(1))
anglePerSample = 360/numSamples

print(f"AH - Measurements: {measurementNum}")
print(f"AH - Samples per measurement: {numSamples}")
print(f"AH - Angle per sample: {anglePerSample:.2f} degrees")

# AH - Create output file for point cloud
f = open("scan_data.xyz", "w+")

# AH - Process incoming data
for i in range(measurementNum):
    startingAngle = 90 - anglePerSample
    for j in range(numSamples):
        # AH - Read distance measurement
        dataStore = ser.read(2)
        distance = struct.unpack('H', dataStore)[0]
        
        # AH - Calculate 3D coordinates (Z as depth)
        x = distance * math.sin(math.radians(startingAngle))
        y = distance * math.cos(math.radians(startingAngle))
        z = i * 400
        
        # AH - Save coordinates to file
        f.write(f'{x:.1f} {y:.1f} {z:.1f}\n')
        startingAngle -= anglePerSample
    
    print(f"AH - Processed scan {i+1}/{measurementNum}")

f.close()

# AH - Wait for completion marker
wait_for_marker(ser, "DATA ENDS")
ser.close()

# AH - Initialize visualization with specific window settings
print("AH - Creating visualization...")
pcd = o3d.io.read_point_cloud("scan_data.xyz", format="xyz")

# AH - Create visualization window with defined size
vis = o3d.visualization.Visualizer()
vis.create_window(width=1280, height=720)

# AH - Create vertices for connecting lines
yz_slice_vertex = [[x] for x in range(measurementNum * numSamples)]

# AH - Generate line connections
lines = []
# AH - Connect points within each slice
for i in range(0, measurementNum * numSamples, numSamples):
    for j in range(numSamples):
        if j == numSamples-1:
            lines.append([yz_slice_vertex[i+j][0], yz_slice_vertex[i][0]])
        else:
            lines.append([yz_slice_vertex[i+j][0], yz_slice_vertex[i+j+1][0]])

# AH - Connect points between slices
for i in range(0, measurementNum * numSamples - numSamples, numSamples):
    for j in range(numSamples):
        lines.append([yz_slice_vertex[i+j][0], yz_slice_vertex[i+j+numSamples][0]])

# AH - Create visualization elements
line_set = o3d.geometry.LineSet(
    points=o3d.utility.Vector3dVector(np.asarray(pcd.points)),
    lines=o3d.utility.Vector2iVector(lines)
)

# AH - Set custom colors for better visibility
line_set.paint_uniform_color([0, 0.7, 0])    # AH - Green lines
pcd.paint_uniform_color([0.7, 0, 0.7])       # AH - Purple points

# AH - Add geometries to visualizer
vis.add_geometry(pcd)
vis.add_geometry(line_set)

# AH - Set initial view parameters
view_control = vis.get_view_control()
view_control.set_zoom(1.0)
view_control.set_front([0, 0, -1])  # AH - Look at Z axis (depth)
view_control.set_lookat([0, 0, 0])
view_control.set_up([0, 1, 0])

# AH - Run visualizer
print("AH - Displaying 3D scan...")
vis.run()
vis.destroy_window()

# AH - Save processed data
np.savez('scanned_data', 
         points=np.asarray(pcd.points),
         measurementNum=measurementNum,
         numSamples=numSamples)
print("AH - Data saved successfully")