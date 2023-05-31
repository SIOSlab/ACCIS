rm -f results/*
rm -f plots/*
rm -f images/*

python3 delta.py

python3 accis_plot_err.py
