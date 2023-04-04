import numpy as np
import matplotlib.pyplot as plt

sat_ids = np.genfromtxt("results/sat_ids.csv", delimiter=",", dtype=int)

n_sats = sat_ids.size

ylabels = [
    'Position Error (m)',
    'Velocity Error (m/s)',
    'Angular Velocity Error (deg/s)',
    'Attitude Error (deg)',
    'Camera Attitude Error (deg)',
    'Focal Length Error (mm)',
    'Distortion Parameter c1 Error',
    'Distortion Parameter c2 Error',
    'Distortion Parameter c3 Error'
    ]

fig, axs = plt.subplots(3, 3)

t = np.genfromtxt("results/sat_" + str(sat_ids[0]) + "_time_s.csv", \
        delimiter=",")
t = t / 60
        
for i in range(3) :
    for j in range(3) :

        e = np.empty((n_sats, t.size))

        for s in range(n_sats):
            sat_id = sat_ids[s]
            k = 3*i + j
            err = np.genfromtxt(
                    "results/sat_" + str(sat_id) + "_state_error.csv",
                    delimiter=","
                    )
            e[s, :] = err[:, k]

        max_err = np.amax(e, 0)
        rms_err = np.sqrt(np.mean(e**2, 0))

        axs[i][j].plot(t, max_err, label = "MAX")
        axs[i][j].plot(t, rms_err, label = "RMS")
        
        axs[i][j].set_xlabel('Time (min)')
        axs[i][j].set_ylabel(ylabels[k])
        axs[i][j].set_yscale('log')
        axs[i][j].legend()

fig.set_size_inches(15, 12)
fig.tight_layout()
plt.savefig("plots/est_err.pdf")

#-------------------------------------------------------------------------------

ylabels = [
    'Pointing Error (deg)',
    'Angular Velocity Error (deg/s)'
    ]

fig, axs = plt.subplots(1, 2)

for j in range(2):

    e = np.empty((n_sats, t.size))
    
    for s in range(n_sats):
        sat_id = sat_ids[s]
        k = 9 + j
        err = np.genfromtxt(
                "results/sat_" + str(sat_id) + "_state_error.csv",
                delimiter=","
                )
        e[s, :] = err[:, k]

    max_err = np.amax(e, 0)
    rms_err = np.sqrt(np.mean(e**2, 0))

    axs[j].plot(t, max_err, label = "MAX")
    axs[j].plot(t, rms_err, label = "RMS")
    
    axs[j].set_xlabel('Time (min)')
    axs[j].set_ylabel(ylabels[j])
    axs[j].set_yscale('log')
    axs[j].legend()

fig.set_size_inches(10, 5)
fig.tight_layout()
plt.savefig("plots/att_err.pdf")
