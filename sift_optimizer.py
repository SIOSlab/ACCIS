import build.accis as accis

from accis_io import *

from numpy import *

gen = accis_get_gen("gen")

par = gen.get_param()

N = [5, 10, 25, 50, 100, 250, 500, 1000, 2500, 5000, 10000]

D = [0.01, 0.025, 0.05, 0.1, 0.25, 0.5]

min_pts = 800

min_norm = float('inf')

for n in N:

    for d in D:

        print("-----------------------------------------------")
        print("Running n = " + str(n) + "; d = " + str(d))
        print("-----------------------------------------------")

        par["Number of Key Points"] = n

        par["Max. Keypoint Descriptor Distance"] = d

        gen.set_param(par)

        diffs = gen.get_diffs()

        npts = diffs.shape[0]
        ncol = diffs.shape[1]

        if npts >= min_pts: 

            B = diffs[:, 0:4]
            A = diffs[:, 4:ncol]

            X, res, rank, s = linalg.lstsq(A, B, rcond=-1)

            dY = B - matmul(A, X)

            R = matmul(transpose(dY), dY) / npts

            bias = sum(dY, axis=0) / npts

            cov_norm = linalg.norm(R, 2)

            print("Covariance Norm: " + str(cov_norm))

            if cov_norm < min_norm:            

                min_norm = cov_norm

                make_gen_file("gen", gen)

                savetxt("sift_coefs/h_mat.csv", transpose(X), delimiter=",")
                savetxt("sift_coefs/cov.csv", R, delimiter=",")
                savetxt("sift_coefs/bias.csv", bias, delimiter=",")
