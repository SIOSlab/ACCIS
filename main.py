import numpy as np
import matplotlib.pyplot as plt

import build.accis as accis

from accis_run import *

sat1 = accis.sat()
sat2 = accis.sat()

p1 = sat1.get_param()
p2 = sat2.get_param()

p1["Satellite ID"] = 1
p2["Satellite ID"] = 2

sat1.set_param(p1)
sat2.set_param(p2)

sats = [sat1, sat2]

accis_run(sats, 50)

res1 = sat1.get_results()
res2 = sat2.get_results()

err1 = res1["State Error"]
err2 = res2["State Error"]

print(err1)
print(err2)
