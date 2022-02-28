'''import alphashape
import matplotlib.pyplot as plt
from descartes import PolygonPatch

points = [(17, 158),(15, 135),(38, 183),(43, 19),(93, 88),(96, 140),(149, 163),(128, 248),(216, 265),(248, 210),(223, 167),(256, 151),(331, 214),(340, 187),(316, 53),(298, 35),(182, 0),(121, 42)]

alpha = 0.95 * alphashape.optimizealpha(points)
hull = alphashape.alphashape(points, alpha)
hull_pts = hull.exterior.coords.xy

fig, ax = plt.subplots()
ax.scatter(hull_pts[0], hull_pts[1], color='red')
ax.add_patch(PolygonPatch(hull, fill=False, color='green'))
'''


import alphashape
import matplotlib.pyplot as plt
from descartes import PolygonPatch

# Define input points
points = [(0., 0.), (0., 1.), (1., 1.), (1., 0.), (0.5, 0.5)]
points = [(17, 158),(15, 135),(38, 183),(43, 19),(93, 88),(96, 140),(149, 163),(128, 248),(216, 265),(248, 210),(223, 167),(256, 151),(331, 214),(340, 187),(316, 53),(298, 35),(182, 0),(121, 42)]

# Define alpha parameter
alpha = 0.

alpha = 0.95 * alphashape.optimizealpha(points)

# Generate the alpha shape
alpha_shape = alphashape.alphashape(points, alpha)
print(alpha_shape)

# Initialize plot
fig, ax = plt.subplots()

# Plot input points
ax.scatter(*zip(*points))

# Plot alpha shape
ax.add_patch(PolygonPatch(alpha_shape, alpha=.2))

plt.show()
