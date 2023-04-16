import build.accis as accis

from accis_io import *
from accis_run import *

sat_default = accis_get_sat('sat')

par_default = sat_default.get_param()

n_plan = 1
n_phas = 6

sats = []

for i in range(n_plan):
    for j in range(n_phas):

        par = par_default

        par["Orbit RAAN (deg)"]                  = i * 360.0 / n_plan
        par["Orbit True Anomaly at Epoch (deg)"] = j * 360.0 / n_phas
        par["Satellite ID"]                      = i * n_phas + j + 1
        par["Random Number Generator Seed"]      = j * n_plan + i + 1

        sat = accis.sat()

        sat.set_param(par)

        sats.append(sat)

t_mins = 1000

t_save = 10

steps_per_min = 120

steps = t_mins * steps_per_min
save_cadence = t_save * steps_per_min

accis_run(sats, steps, save_cadence)

accis_save_results(sats)
