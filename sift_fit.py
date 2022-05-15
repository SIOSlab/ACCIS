from numpy import *

table = genfromtxt("results/gen.csv", delimiter=",")

npts = table.shape[0]

ndx = 10

dist = table[:, 0]

print("Minimum distance: " + str(amin(dist)))
print("Maximum distance: " + str(amax(dist)))

K1 = table[:, 1:5]
K2 = table[:, 5:9]

dx = table[:, 9:(9+ndx)]

K1a = hstack((K1, ones([npts, 1])))
dxa = hstack((dx, ones([npts, 1])))

A = empty([npts, 5*(ndx+1)])
for i in range(npts):
    A[i, :] = kron(K1a[i, :], dxa[i, :])

B = K2

X, res, rank, s = linalg.lstsq(A, B, rcond=-1)

savetxt("results/sift_fit.csv", transpose(X), delimiter=",")

dY = B - matmul(A, X)

R = matmul(transpose(dY), dY) / npts

savetxt("results/sift_fit_cov.csv", R, delimiter=",")

bias = sum(dY, axis=0) / npts

savetxt("results/sift_fit_bias.csv", bias, delimiter=",")
