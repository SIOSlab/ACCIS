import numpy as np
import matplotlib.pyplot as plt

dist = np.genfromtxt("sift_coefs/dist.csv", delimiter=",")

x = dist[:, 0]
y = dist[:, 1]

plt.scatter(x, y, s=1)
plt.show()
