#include "generator.hpp"

#include "format.hpp"
#include "rando.hpp"
#include "sifter.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp> 
#include <opencv2/imgproc.hpp>

void generator::run() {

    // Random number generator
    rando rand(seed);

    // Generate & process pairs of images
    for (int i = 0; i < max_pairs; i++) {

        sat_state s1;
        sat_state s2;

        // TO DO -- Generate s1 & s2
        
        // Get images
        cv::Mat img1 = cam.real_image(0, s1); 
        cv::Mat img2 = cam.real_image(0, s2); 

        // Save images
        cv::imwrite("images/pic_query_" + int2str0(i, 6) + ".png", img1);
        cv::imwrite("images/pic_train_" + int2str0(i, 6) + ".png", img2);

        // Percentages of black pixels
        cv::Mat gray1;
        cv::Mat gray2;
        cv::cvtColor(img1, gray1, cv::COLOR_RGB2GRAY);
        cv::cvtColor(img2, gray2, cv::COLOR_RGB2GRAY);
        double blp1 = 100.0 - (100.0 * cv::countNonZero(gray1)) / gray1.total();
        double blp2 = 100.0 - (100.0 * cv::countNonZero(gray2)) / gray2.total();

        // Apply SIFT & match         
        if (blp1 < max_blp && blp2 < max_blp) {

            // SIFT key points
            sifter::points pts1 = sifter::sift(img1, num_pts);
            sifter::points pts2 = sifter::sift(img2, num_pts);

            // Match SIFT key points
            sifter::matches sm = sifter::match(pts1, pts2, dmax); 

            // TO DO -- Save points

        }

    }

}
