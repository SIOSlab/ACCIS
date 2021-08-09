#include "Landsat.hpp"

#include <cstdio>
#include <opencv2/imgcodecs.hpp>

cv::Mat Landsat::image(int widp, int lenp,
        double latc, double lonc, double dp) {

    static Landsat landsat;  

    landsat.click(widp, lenp, latc, lonc, dp);

    cv::Mat img = cv::imread("landsat_temp.png");

    remove("landsat_temp.png");

    return img;

}

void Landsat::click(int widp, int lenp, double latc, double lonc, double dp) {

    int flag;

    flag = env->CallStaticIntMethod(classLandsat, getImage, widp, lenp,
            latc, lonc, dp);   

    if (flag != 0)
        exit(EXIT_FAILURE);

}

Landsat::Landsat() {

    const char* cp = "-Djava.class.path=./build/Landsat.jar:./WorldWindJava.jar";

    JavaVMOption options[1];
    options[0].optionString = (char*) cp;
 
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = false; 
   
    jint rc = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
   
    if (rc != JNI_OK)
        exit(EXIT_FAILURE);

    classLandsat = env->FindClass("Landsat"); 

    getImage = env->GetStaticMethodID(classLandsat, "getImage", "(IIDDD)I"); 

}

Landsat::~Landsat() {

        jvm->DestroyJavaVM();

}

