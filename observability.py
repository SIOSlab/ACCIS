import build.accis as accis

from accis_io import *

import numpy as np
import numdifftools as nd

sat = accis_get_sat('sat')

p1 = sat.get_param()
p2 = sat.get_param()

p1["Orbit True Anomaly at Epoch (deg)"] = 0
p2["Orbit True Anomaly at Epoch (deg)"] = 0.5

p1["Random Number Generator Seed"] = 1
p2["Random Number Generator Seed"] = 2

p1["Satellite ID"] = 1
p2["Satellite ID"] = 2

observer = accis.cam_observer()

observer.set_param(p1, p2)

npts = 2

npix = 100

pts1 = np.random.rand(4, npts) * npix
pts2 = np.random.rand(4, npts) * npix

observer.set_pts(pts1, pts2)

h = lambda dx : observer.h(dx)

ndx = 12

dxc = np.zeros(ndx)

obsmat = nd.Jacobian(h)(dxc)

rk = np.linalg.matrix_rank(obsmat)

print("Observability matrix rank:")
print(rk)
