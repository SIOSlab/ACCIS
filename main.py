import build.accis as accis

import json

sat1 = accis.sat();
sat2 = accis.sat();

d1 = sat1.get_param();

with open('sat1.json', 'w') as fp:
    json.dump(d1, fp, sort_keys=True, indent=4)

accis.run([sat1], 10)
