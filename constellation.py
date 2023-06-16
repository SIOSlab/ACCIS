import math

import build.accis as accis

from accis_io import *
from accis_run import *

sat1 = accis_get_sat('sat')

p1 = sat1.get_param()

a = p1["Orbit Semi-Major Axis (km)"]

mu = 3.986E5;

mean_mot = math.degrees(math.sqrt(mu / a**3))

dt = 300

df = -dt * mean_mot
dW =  dt * 360 / 86400

n_sat = 4

sats = []

for j in range(n_sat):
    
    sat = accis_get_sat('sat')

    p = sat.get_param()

    p["Orbit True Anomaly at Epoch (deg)"] = j * df

    p["Orbit RAAN (deg)"] = j * dW

    p["Satellite ID"] = j + 1

    p["Random Number Generator Seed"] = 11 * j + 14 

    sat.set_param(p)

    sats.append(sat)

t_mins = 100

t_save = 10

steps_per_min = 120

steps = t_mins * steps_per_min
save_cadence = t_save * steps_per_min

accis_run(sats, steps, save_cadence)

accis_save_results(sats)
