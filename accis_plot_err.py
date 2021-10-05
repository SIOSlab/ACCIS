import numpy as np
import matplotlib.pyplot as plt

sat_ids = np.genfromtxt("results/sat_ids.csv", delimiter=",", dtype=int)

ylabels = [
    'Position Error (m)',
    'Velocity Error (m/s)',
    'Angular Velocity Error (deg/s)',
    'Attitude Error (deg)',
    'Camera Attitude Error (deg)',
    'Focal Length Error (mm)'
    ]

fig, axs = plt.subplots(3, 2)

for i in range(3) :
    for j in range(2) :
        for sat_id in sat_ids :
            k = 2*i + j
            t = np.genfromtxt(
                    "results/sat_" + str(sat_id) + "_time_s.csv",
                    delimiter=","
                    )
            err = np.genfromtxt(
                    "results/sat_" + str(sat_id) + "_state_error.csv",
                    delimiter=","
                    )
            e = err[..., k]
            lbl = 'Satellite ' + str(sat_id)
            axs[i][j].plot(t, e, label = lbl)
        axs[i][j].set_xlabel('Time (s)')
        axs[i][j].set_ylabel(ylabels[k])
        axs[i][j].set_yscale('log')
        axs[i][j].legend()

fig.set_size_inches(8.5, 11)
fig.tight_layout()
plt.savefig("plots/err.pdf")
