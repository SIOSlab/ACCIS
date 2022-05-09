import build.accis as accis

from accis_io import *

import numpy as np

gen = accis_get_gen("gen")

table = gen.run()

np.savetxt("results/gen.csv", table, delimiter=",")
