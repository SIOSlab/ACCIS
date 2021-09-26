import build.accis as accis

sat1 = accis.sat();
sat2 = accis.sat();

d = sat1.get_results();

print(d)

sat1.set_param(d);

accis.run([sat1, sat2], 3);
