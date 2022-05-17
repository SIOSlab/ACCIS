import build.accis as accis

from accis_io import *

import glob

import numpy as np

gen = accis_get_gen("gen")

img_files = sorted(glob.glob("gen/img_*.png"))

state_files = sorted(glob.glob("gen/state_*.csv"))

table = gen.proc_imgs(img_files, state_files) 

np.savetxt("results/sift_pairs.csv", table, delimiter=",")
