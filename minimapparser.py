def axeschange(x, y, z = 0):
    return y, x

p1_map = (14, 760)
p1_world = axeschange(-1755.968750, -59.999840, -60.598217)

p2_map = (682, 140)
p2_world = axeschange(1536.031250, 2959.972900, 137.455612)

model_width = 32

c1_map = (0, 0)
c2_map = (935, 918)

cam_xdist = 4
cam_ydist = 4

world_xscale = (p2_world[0] - p1_world[0]) / (p2_map[0] - p1_map[0])
world_yscale = (p2_world[1] - p1_world[1]) / (p2_map[1] - p1_map[1])

c1_world = (p1_world[0] - (p1_map[0] - c1_map[0]) * world_xscale, p1_world[1] - (p1_map[1] - c1_map[1]) * world_yscale)
c2_world = (p1_world[0] - (p1_map[0] - c2_map[0]) * world_xscale, p1_world[1] - (p1_map[1] - c2_map[1]) * world_yscale)

print(c1_world)
print(c2_world)