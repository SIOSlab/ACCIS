import build.accis as accis

from accis_io import *

import numpy as np

gen = accis_get_gen("gen")

cov = gen.get_cov()

np.savetxt("sift_coefs/cov.csv", cov, delimiter=",")
