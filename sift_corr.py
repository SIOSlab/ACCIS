import build.accis as accis

from accis_io import *

import numpy as np

gen = accis_get_gen("gen")

dist = gen.get_dist()

np.savetxt("sift_coefs/dist.csv", dist, delimiter=",")
