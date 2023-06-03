import math

import build.accis as accis

from accis_io import *
from accis_run import *

#-------------------------------------------------------------------------------

n_plan = 4 
n_phas = 1

t_mins = 400

t_save = 20

steps_per_min = 120

#-------------------------------------------------------------------------------

sat_default = accis_get_sat('sat')

par_default = sat_default.get_param()

a = par_default["Orbit Semi-Major Axis (km)"]

mu = 3.986E5;

mean_mot = math.degrees(math.sqrt(mu / a**3))

dt = 86400 / n_plan

df = mean_mot * dt

sats = []

for i in range(n_plan):
    for j in range(n_phas):

        par = par_default

        par["Orbit RAAN (deg)"]                  = i * 360.0 / n_plan
        par["Orbit True Anomaly at Epoch (deg)"] = j * 360.0 / n_phas - i * df
        par["Satellite ID"]                      = i * n_phas + j + 1
        par["Random Number Generator Seed"]      = j * n_plan + i + 1

        sat = accis.sat()

        sat.set_param(par)

        sats.append(sat)

steps = t_mins * steps_per_min
save_cadence = t_save * steps_per_min

accis_run(sats, steps, save_cadence)

accis_save_results(sats)
