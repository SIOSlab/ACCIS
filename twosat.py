import build.accis as accis

from accis_io import *
from accis_run import *

sat = accis_get_sat('sat')

p1 = sat.get_param()
p2 = sat.get_param()

p1["Orbit True Anomaly at Epoch (deg)"] = 0
p2["Orbit True Anomaly at Epoch (deg)"] = 0.5

p1["Random Number Generator Seed"] = 1
p2["Random Number Generator Seed"] = 2

p1["Satellite ID"] = 1
p2["Satellite ID"] = 2

sat1 = accis.sat()
sat2 = accis.sat()

sat1.set_param(p1)
sat2.set_param(p2)

sats = [sat1, sat2]

t_mins = 30

t_save = 5

steps_per_min = 60

steps = t_mins * steps_per_min
save_cadence = t_save * steps_per_min

accis_run(sats, steps, save_cadence)

accis_save_results(sats)
