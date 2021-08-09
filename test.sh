rm -f images/*
rm -f results/HOUSE/*
rm -f results/UKF/*
rm -f plots/*

./build/accis u
./build/accis h

gnuplot plot_ukf_err.plt
gnuplot plot_house_err.plt
