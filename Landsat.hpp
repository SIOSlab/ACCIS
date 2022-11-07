#ifndef LANDSAT_HPP
#define LANDSAT_HPP

#include <jni.h>
#include <opencv2/core.hpp>

class Landsat final {

    public:

    // Get Landsat image
    static cv::Mat image(int widp, int lenp,
            double latc, double lonc, double dp);

    private:

    // Download Landsat image and save
    int click(int widp, int lenp,
            double latc, double lonc, double dp);

    // Java runtime variables
    JavaVM *jvm;
    JNIEnv *env;
    jclass classLandsat;
    jmethodID getImage; 

    // Constructor
    Landsat();

    // Destructor
    ~Landsat();

    // Copy constructor (undefined)
    Landsat(const Landsat&);

    // Assignement operator (undefined)
    Landsat& operator = (const Landsat&);

};

#endif
