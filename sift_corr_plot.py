import numpy as np
import matplotlib.pyplot as plt

dist = np.genfromtxt("sift_coefs/dist.csv", delimiter=",")

x = dist[:, 0]
y = np.radians(dist[:, 1]) * 6378

plt.scatter(x, y, s=0.25)
plt.xlabel("Descriptor Distance")
plt.ylabel("Distance on Earth (km)")
plt.show()
