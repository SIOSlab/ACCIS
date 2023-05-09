import numpy as np
import matplotlib.pyplot as plt

sat_ids = np.genfromtxt("results/sat_ids.csv", delimiter=",", dtype=int)

ylabels = [
    'Position Error (m)',
    'Velocity Error (m/s)',
    'Angular Velocity Error (deg/s)',
    'Attitude Error (deg)',
    'Camera Attitude Error (deg)',
    'Focal Length Error (mm)',
    r'Distortion Parameter $c_1$ Error',
    r'Distortion Parameter $c_2$ Error',
    r'Distortion Parameter $c_3$ Error',
    'Pointing Error (deg)',
    'Angular Velocity Error (deg/s)'
    ]

abbrv = ['pos', 'vel', 'av', 'att', 'ca', 'f', 'c1', 'c2', 'c3', 'pe', 're']

for i in range(11):

    print('Plot ' + str(i+1))

    for j in range(sat_ids.size):

        sat_id = sat_ids[j]
            
        print('    Satellite ' + str(sat_id))
            
        plt.figure(figsize = (4, 4))

        plt.rc('font', size=10) 

        t = np.genfromtxt(
                "results/sat_" + str(sat_id) + "_time_s.csv",
                delimiter=","
                )
        t = t / 60
            
        err = np.genfromtxt(
                "results/sat_" + str(sat_id) + "_state_error.csv",
                delimiter=","
                )
            
        e = err[:, i]
            
        plt.plot(t, e)

        plt.xlabel('Time (min)')
        plt.ylabel(ylabels[i])
        plt.yscale('log')
        plt.xscale('log')

        plt.title("Satellite " + str(sat_id), {'fontsize': 10})

        plt.tight_layout()

        plt.savefig('plots/' + abbrv[i] + '_err_' + str(sat_id) + '.pdf')

        plt.close()
