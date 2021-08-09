set terminal postscript eps color enhanced size 5,3.5 font 'Helvetica,11pt'

set datafile separator ','

set xlabel 'Time (s)'

set logscale y

set format y '10^{%T}'

set style line 2 dashtype 3 lw 4 lc rgb 'blue'
set style line 1 dashtype 1 lw 4 lc rgb 'red'

#-------------------------------------------------------------------------------

set output 'plots/ukf_dynamics_err.eps'

set multiplot layout 2,2

set ylabel 'Position Error (m)'
plot 'results/UKF/sat1_err.csv' u 1:2 title 'Satellite 1' w l ls 1, \
     'results/UKF/sat2_err.csv' u 1:2 title 'Satellite 2' w l ls 2

set ylabel 'Velocity Error (m/s)'
plot 'results/UKF/sat1_err.csv' u 1:3 title 'Satellite 1' w l ls 1, \
     'results/UKF/sat2_err.csv' u 1:3 title 'Satellite 2' w l ls 2

set ylabel 'Attitude Error (deg)'
plot 'results/UKF/sat1_err.csv' u 1:4 title 'Satellite 1' w l ls 1, \
     'results/UKF/sat2_err.csv' u 1:4 title 'Satellite 2' w l ls 2

set ylabel 'Angular Velocity Error (deg/s)'
plot 'results/UKF/sat1_err.csv' u 1:5 title 'Satellite 1' w l ls 1, \
     'results/UKF/sat2_err.csv' u 1:5 title 'Satellite 2' w l ls 2

unset multiplot

#-------------------------------------------------------------------------------

set output 'plots/ukf_camera_err.eps'

set multiplot layout 2,2

set ylabel 'Camera Attitude Error (deg)'
plot 'results/UKF/sat1_err.csv' u 1:6  title 'Satellite 1' w l ls 1, \
     'results/UKF/sat2_err.csv' u 1:6  title 'Satellite 2' w l ls 2

set ylabel 'c_1 Error'
plot 'results/UKF/sat1_err.csv' u 1:8  title 'Satellite 1' w l ls 1, \
     'results/UKF/sat2_err.csv' u 1:8  title 'Satellite 2' w l ls 2

set ylabel 'c_2 Error'
plot 'results/UKF/sat1_err.csv' u 1:9  title 'Satellite 1' w l ls 1, \
     'results/UKF/sat2_err.csv' u 1:9  title 'Satellite 2' w l ls 2

set ylabel 'c_3 Error'
plot 'results/UKF/sat1_err.csv' u 1:10 title 'Satellite 1' w l ls 1, \
     'results/UKF/sat2_err.csv' u 1:10 title 'Satellite 2' w l ls 2

unset multiplot

#-------------------------------------------------------------------------------
