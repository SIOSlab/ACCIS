def accis_run(sat_list, num_steps) :
    for k in range(num_steps) :
        for sat in sat_list :
            sat.step()
        for sat1 in sat_list :
            for sat2 in sat_list :
                sat1.transmit(sat2)