import build.accis as accis

from accis_run import *

sat1 = accis.sat()
sat2 = accis.sat()

p1 = sat1.get_param()
p2 = sat2.get_param()

p1["Satellite ID"] = "sat1"
p2["Satellite ID"] = "sat2"

sat1.set_param(p1)
sat2.set_param(p2)

sats = [sat1, sat2]

accis_run(sats, 10)

print(sat1.get_results())
