#ifndef ANGLES_HPP
#define ANGLES_HPP

#include <cmath>

constexpr double pi = 3.14159265358979323846;

inline double rad2deg(double x) {
    return x * (180 / pi);
}

inline double deg2rad(double x) {
    return x * (pi / 180);
}

inline double rad2min(double x) {
    return x * (180 * 60 / pi);
}

inline double min2rad(double x) {
    return x * (pi / (180 * 60));
}

inline double sec2rad(double x) {
    return x * (pi / (180 * 60 * 60));
}

inline double rad2sec(double x) {
    return x * ((180 * 60 * 60) / pi);
}

inline double hav(double x) {
    return 0.5 * (1 - cos(deg2rad(x)));
}

inline double ahav(double x) {
    return rad2deg(acos(1 - 2*x));
}

inline double hav_dist(double lat1, double lat2, double lon1, double lon2) {
    return ahav(hav(lat2-lat1) + cos(lat1)*cos(lat2)*hav(lon2-lon1));
}

#endif
