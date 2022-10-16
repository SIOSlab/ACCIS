import build.accis as accis

from accis_io import *

import numpy as np

gen = accis_get_gen("gen")

diffs = gen.get_diffs()

np.savetxt("sift_coefs/diffs.csv", diffs, delimiter=",")
