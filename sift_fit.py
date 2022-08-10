from numpy import *

table = genfromtxt("sift_coefs/gen.csv", delimiter=",")

npts = table.shape[0]
ncol = table.shape[1]

K1 = table[:, 0:4]
K2 = table[:, 4:8]

dx = table[:, 8:ncol]

u = hstack((dx, K1, ones([npts, 1])))

A = empty([npts, (ncol-3)**2])
for i in range(npts):
    A[i, :] = kron(u[i, :], u[i, :])

B = K2

X, res, rank, s = linalg.lstsq(A, B, rcond=-1)

savetxt("sift_coefs/h_mat.csv", transpose(X), delimiter=",")

dY = B - matmul(A, X)

R = matmul(transpose(dY), dY) / npts

savetxt("sift_coefs/cov.csv", R, delimiter=",")

bias = sum(dY, axis=0) / npts

savetxt("sift_coefs/bias.csv", bias, delimiter=",")
