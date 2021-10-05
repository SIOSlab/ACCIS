import numpy as np
import matplotlib.pyplot as plt
import pickle

import build.accis as accis

from accis_run import *
from accis_save_results import *

sat1 = accis.sat()
sat2 = accis.sat()

p1 = sat1.get_param()
p2 = sat2.get_param()

p1["Satellite ID"] = 1
p2["Satellite ID"] = 2

p1["Orbit True Anomaly at Epoch (deg)"] = 0
p2["Orbit True Anomaly at Epoch (deg)"] = 1

sat1.set_param(p1)
sat2.set_param(p2)

sats = [sat1, sat2]

accis_run(sats, 120)

accis_save_results(sats)
