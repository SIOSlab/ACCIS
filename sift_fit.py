import numpy as np

table = np.genfromtxt("results/gen.csv", delimiter=",")

npts = table.shape[0]

ncols = table.shape[1]

dist = table[:, ncols-1]

print("Minimum distance: " + str(np.amin(dist)))
print("Maximum distance: " + str(np.amax(dist)))

X = table[:, 0:(ncols-5)]

Y = table[:, (ncols-5):(ncols-1)]

A, res, rank, s = np.linalg.lstsq(X, Y, rcond=-1)

np.savetxt("results/sift_fit.csv", np.transpose(A), delimiter=",")

dY = np.subtract(Y, np.matmul(X, A))

R = np.matmul(np.transpose(dY), dY) / npts

np.savetxt("results/sift_fit_cov.csv", R, delimiter=",")
