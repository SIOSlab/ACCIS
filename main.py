import build.accis as accis

sat1 = accis.sat()
sat2 = accis.sat()

d1 = sat1.get_param()

accis.run([sat1, sat2], 10)

print(d1)
