import time

import build.accis as accis

from accis_io import *

def accis_run(sat_list, num_steps, save_cadence) :
    print("Running ACCIS")
    ti = time.time()
    for k in range(num_steps) :
        if (k % 10 == 0) :
            print('Step ' + str(k))
        for sat in sat_list :
            sat.step()
        for sat1 in sat_list :
            for sat2 in sat_list :
                sat1.transmit(sat2)
        if (k != 0 and k % save_cadence == 0) :
            accis_save_results(sat_list)
    tf = time.time()
    run_time = round((tf - ti) / 60, 1)
    print("Run Time (min): %s" % run_time)
