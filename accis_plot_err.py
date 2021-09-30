import numpy as np
import matplotlib.pyplot as plt

def accis_plot_err(sats, filename) :
    ylabels = [
            'Position Error (m)',
            'Velocity Error (m/s)',
            'Angular Velocity Error (deg/s)',
            'Attitude Error (deg)',
            'Camera Attitude Error (deg)',
            'Focal Length Error (deg)'
            ]
    fig, axs = plt.subplots(3, 2)
    for i in range(3) :
        for j in range(2) :
            for sat in sats :
                k = 2*i + j
                par = sat.get_param()
                res = sat.get_results()
                t = res['Time (s)']
                e = res['State Error'][k]
                lbl = 'Satellite ' + str(par['Satellite ID'])
                axs[i][j].plot(t, e, label = lbl)
            axs[i][j].set_xlabel('Time (s)')
            axs[i][j].set_ylabel(ylabels[k])
            axs[i][j].set_yscale('log')
            axs[i][j].legend()
    fig.tight_layout()
    plt.savefig(filename)
