import json
import numpy as np

import build.accis as accis

class numpy_encoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.integer):
            return int(obj)
        elif isinstance(obj, np.floating):
            return float(obj)
        elif isinstance(obj, np.ndarray):
            return obj.tolist()
        return json.JSONEncoder.default(self, obj)

def make_gen_file(filename, gen = None) :
    if gen == None:
        gen = accis.generator()
    d = gen.get_param()
    with open('inputs/' + filename + '.json', 'w') as f :
        json.dump(d, f, indent = 4, cls = numpy_encoder)

def accis_get_gen(filename) :
    gen = accis.generator()
    with open('inputs/' + filename + '.json') as f :
        d = json.load(f)
        gen.set_param(d)
    return gen

def make_sat_file(filename, sat = None) :
    if sat == None:
        sat = accis.sat()
    d = sat.get_param()
    with open('inputs/' + filename + '.json', 'w') as f :
        json.dump(d, f, indent = 4, cls = numpy_encoder)

def accis_get_sat(filename) :
    sat = accis.sat()
    with open('inputs/' + filename + '.json') as f :
        d = json.load(f)
        sat.set_param(d)
    return sat

def accis_save_results(sats) :
    print('-- Saving Results --')
    sat_ids = []
    for sat in sats :
        param = sat.get_param()
        sat_id = param["Satellite ID"]
        sat_ids.append(sat_id)
        for key, val in sat.get_results().items() :
            k = key.replace(" ", "_").replace("(", "").replace(")", "").lower()
            filename = "results/sat_" + str(sat_id) + "_" + k + ".csv"
            np.savetxt(filename, np.array(val), delimiter=",")
    np.savetxt("results/sat_ids.csv", np.array(sat_ids), delimiter=",", fmt="%d")
