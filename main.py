import build.accis as accis

from accis_run import *

sat1 = accis.sat()
sat2 = accis.sat()

d1 = sat1.get_param()

sats = [sat1, sat2]

accis_run(sats, 10)

print(sats[1].get_results())
