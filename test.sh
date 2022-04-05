rm -f results/*
rm -f plots/*
rm -f images/*

python3 twosat.py

python3 accis_plot_err.py
