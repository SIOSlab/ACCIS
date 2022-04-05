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
    'Distortion Parameter c1 Error',
    'Distortion Parameter c2 Error',
    'Distortion Parameter c3 Error'
    ]

fig, axs = plt.subplots(3, 3)

for i in range(3) :
    for j in range(3) :
        for sat_id in sat_ids :
            k = 3*i + j
            t = np.genfromtxt(
                    "results/sat_" + str(sat_id) + "_time_s.csv",
                    delimiter=","
                    )
            t = t / 60
            err = np.genfromtxt(
                    "results/sat_" + str(sat_id) + "_state_error.csv",
                    delimiter=","
                    )
            e = err[..., k]
            lbl = 'Satellite ' + str(sat_id)
            axs[i][j].plot(t, e, label = lbl)
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

for j in range(2) :
    for sat_id in sat_ids :
        k = 9 + j
        t = np.genfromtxt(
                "results/sat_" + str(sat_id) + "_time_s.csv",
                delimiter=","
                )
        t = t / 60
        err = np.genfromtxt(
                "results/sat_" + str(sat_id) + "_state_error.csv",
                delimiter=","
                )
        e = err[..., k]
        lbl = 'Satellite ' + str(sat_id)
        axs[j].plot(t, e, label = lbl)
    axs[j].set_xlabel('Time (min)')
    axs[j].set_ylabel(ylabels[j])
    axs[j].set_yscale('log')
    axs[j].legend()

fig.set_size_inches(10, 5)
fig.tight_layout()
plt.savefig("plots/att_err.pdf")
