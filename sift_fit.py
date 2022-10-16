import build.accis as accis

from accis_io import *

from numpy import *

gen = accis_get_gen("gen")

diffs = gen.get_diffs()

npts = diffs.shape[0]
ncol = diffs.shape[1]

B = diffs[:, 0:4]
A = diffs[:, 4:ncol]

X, res, rank, s = linalg.lstsq(A, B, rcond=-1)

savetxt("sift_coefs/h_mat.csv", transpose(X), delimiter=",")

dY = B - matmul(A, X)

R = matmul(transpose(dY), dY) / npts

savetxt("sift_coefs/cov.csv", R, delimiter=",")

bias = sum(dY, axis=0) / npts

savetxt("sift_coefs/bias.csv", bias, delimiter=",")
