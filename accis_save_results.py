import numpy as np

def accis_save_results(sats) :
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
