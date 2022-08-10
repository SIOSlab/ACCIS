from numpy import *

table = genfromtxt("sift_coefs/gen.csv", delimiter=",")

npts = table.shape[0]
ncol = table.shape[1]

B = table[:, 0:4]
A = table[:, 4:ncol]

X, res, rank, s = linalg.lstsq(A, B, rcond=-1)

savetxt("sift_coefs/h_mat.csv", transpose(X), delimiter=",")

dY = B - matmul(A, X)

R = matmul(transpose(dY), dY) / npts

savetxt("sift_coefs/cov.csv", R, delimiter=",")

bias = sum(dY, axis=0) / npts

savetxt("sift_coefs/bias.csv", bias, delimiter=",")
