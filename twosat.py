import build.accis as accis

from accis_io import *
from accis_run import *

sat1 = accis_get_sat('sat1')
sat2 = accis_get_sat('sat2')

sats = [sat1, sat2]

accis_run(sats, 120)

accis_save_results(sats)
