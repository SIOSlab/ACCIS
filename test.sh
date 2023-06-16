rm -f results/*
rm -f plots/*
rm -f images/*

python3 constellation.py

python3 accis_plot_err.py

xdg-open plots/est_err.pdf
