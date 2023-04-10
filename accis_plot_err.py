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

    plt.figure(figsize = (6, 3))

    plt.rc('font', size=10) 

    for sat_id in sat_ids :
            
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
            
        lbl = 'Satellite ' + str(sat_id)
        plt.plot(t, e, label = lbl)

    plt.xlabel('Time (min)')
    plt.ylabel(ylabels[i])
    plt.yscale('log')
    plt.legend()

    plt.tight_layout()
    plt.savefig('plots/' + abbrv[i] + '_err.pdf')

    plt.close()
