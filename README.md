# ACCIS
Autonomous Cross-Calibration for Imaging Satellites

## Requirements
- C/C++/Fortran toolchain (e.g., gcc https://gcc.gnu.org/)
- Java JDK (https://www.oracle.com/java/technologies/javase-downloads.html)
- CMake (https://cmake.org/)
- OpenCV (https://github.com/opencv/opencv)
- GDAL (https://gdal.org/)
- Eigen (https://eigen.tuxfamily.org/dox/GettingStarted.html)

## Installation
- To build and set up (using CMake), run build.sh
- To run a minimal simulation, run test.sh

## Simulation Steps
For each satellite at each simulation step:
 1. Propagate ground truth state
 2. Predict state estimate
 3. Generate measurements for GPS and star tracker
 4. Update state estimate
 5. If cross-calibration step:
    1. Compute field-of-view based on ground truth state
    2. Acquire Landsat image of Earth surface sector
    3. Trim, project, and distort Landsat image
    4. Compute SIFT key points from image
    5. “Transmit” SIFT key points and state estimate moments to other satellites
    6. Perform SIFT-based update of state estimate using images from other satellites at previous time step

## Third-Party Tools
The following third-party tools and data are included in this repository:
- EGM2008 Gravitational Model (https://earth-info.nga.mil/index.php?dir=wgs84&action=wgs84)
- JPL Planetary and Lunar Ephemerides (https://ssd.jpl.nasa.gov/?planet_eph_export)
- NASA World Wind (https://worldwind.arc.nasa.gov/java)
    - Source code: https://github.com/nasa/World-Wind-Java
    - License: https://worldwind.arc.nasa.gov/LICENSE.html
- NOVAS: Naval Observatory Vector Astronomy Subroutines (https://ui.adsabs.harvard.edu/abs/2012IAUJD...7P..24K/abstract)
- NRLMSISE-00 Atmospheric Model (https://ccmc.gsfc.nasa.gov/modelweb/atmos/nrlmsise00.html)
- RKSUITE ODE Solver (http://www.netlib.org/ode/rksuite)
