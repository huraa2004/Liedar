import open3d as o3d
import numpy as np

points = np.loadtxt("scan_data.xyz")
pcd = o3d.geometry.PointCloud()
pcd.points = o3d.utility.Vector3dVector(points)

if not pcd.has_points():
    print("No points found in file")
    exit(1)

vis = o3d.visualization.Visualizer()
vis.create_window(width=1280, height=720)
pcd.paint_uniform_color([0.7, 0, 0.7])
vis.add_geometry(pcd)

axes = o3d.geometry.TriangleMesh.create_coordinate_frame(size=1000)
vis.add_geometry(axes)

unique_z = np.unique(points[:, 2])
measurementNum = len(unique_z)
numSamples = len(points) // measurementNum

lines = []
for i in range(measurementNum):
    start_idx = i * numSamples
    for j in range(numSamples):
        current_idx = start_idx + j
        next_idx = start_idx + (j + 1) % numSamples
        lines.append([current_idx, next_idx])

for i in range(measurementNum - 1):
    for j in range(numSamples):
        current_idx = i * numSamples + j
        next_slice_idx = (i + 1) * numSamples + j
        lines.append([current_idx, next_slice_idx])

line_set = o3d.geometry.LineSet(
    points=o3d.utility.Vector3dVector(points),
    lines=o3d.utility.Vector2iVector(lines)
)

line_set.paint_uniform_color([0, 0.7, 0])
vis.add_geometry(line_set)

view_control = vis.get_view_control()
view_control.set_zoom(0.8)
view_control.set_front([0, 0, -1])
view_control.set_lookat([0, 0, 2000])
view_control.set_up([0, 1, 0])

vis.run()
vis.destroy_window()