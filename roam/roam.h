#ifndef ROAM_H
#define ROAM_H

#ifdef __cplusplus
extern "C" {
#endif

extern void roam_(const double* ti, const double* xi, const int* nt,
        const double* t, double* x);

extern void roamf_(const double* t, const double* x, double* xd);

extern void roamps_(const char* var, const double* val);

extern void roamos_(const char* var, const int* val);

extern void coe2rv_(const double* y, double* x);

extern void rv2coe_(const double* x, double* y);

extern void truano_(const double* y, double* dt);

extern void losgeo_(const double* t, const double* reci, const double* vlos,
        const double* tol, double* lat, double* lon, double* dist);

extern void ll2eci_(const double* t, const double* lat, const double* lon,
        const double* alt, double* reci);

extern void eci2ll_(const double* t, const double* reci, const double* tol,
        double* lat, double* lon, double* alt);

#ifdef __cplusplus
}
#endif

#endif
