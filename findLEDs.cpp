// Code to find LEDs in BLC images



// C++ standard libraries
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <math.h>

// OpenCV libraries
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp> // Needed for the GIF images 
#include <math.h>

// Own helpers
#include "helper.hpp"

using namespace cv;
using namespace std;



const int SCALE = 4; // Scale factor for the image, used to increase the accuracy of the HoughCircles functions



int main(int argc, char** argv)
{   
    Mat img, gray; // Representing the image, gray is the grayscale version of the image 

    VideoCapture cap; // Needed to process tje gif immages

    // Open the video file or camera

    string filename = argv[1];
    cap.open(filename);

    if (!cap.isOpened()) {
    std::cerr << "Error: Could not open GIF file." << std::endl;
    return -1;
    }

    cap.read(img); // Converts the frame in the .GIF to an image Mat
    if (img.empty()) {
        std::cerr << "Error: Could not read image from GIF file." << std::endl;
        return -1;
    }
    Mat originalImage = img.clone(); // Saving the original image for later use

    cvtColor(img, gray, COLOR_BGR2GRAY); // Converts the image to grayscale, needed for the HoughCircles function

    Mat originalGray = gray.clone(); // Clones the grayscale image, so that we can use it later 

    Size imgSize = img.size(); // Gets the size of the image

    int width = imgSize.width; // Image width in pixels
    int height = imgSize.height; // Image height in pixels


    if (width != 1360 && height != 1024){ // Checks if the image is the correct size
        cout << "Wrong size of the image, should be (1360x1024)"<< endl;
    }

    
    // Find the outer edge of the dark annulus and masks everything outside of it
    Vec4f innerDarkCircle = findCentralCircles(gray,1); // Finds the inner dark circle

    Point2f darkCenter(innerDarkCircle[0], innerDarkCircle[1]); // Center of the dark circle
    float darkRadius = innerDarkCircle[2]; // Radius of the dark circle
    cout << std::setprecision(10)<<"Dark circle center: " << darkCenter << ", radius: " << darkRadius << endl;

    // Draw the dark circle on the original image
    

    Mat maskedImage1 = masking_image(originalGray, innerDarkCircle, 1,15); // Creates a mask for the image, so that only the area inside the dark circle is visible
    
     // Show the image with the dark circle


    // Find the inner edge of the dark annulus and masks everything inside of it

    Vec4f innerLightCircle = findCentralCircles(maskedImage1,0); // Finds the inner light circle

    Point2f lightCenter(innerLightCircle[0], innerLightCircle[1]); // Center of the light circle
    float lightRadius = innerLightCircle[2]; // Radius of the light circle

    cout << std::setprecision(10)<<"Light circle center: " << lightCenter << ", radius: " << lightRadius << endl;


    
    Mat maskedImage = masking_image(maskedImage1, innerLightCircle, 0, 7); // Creates a mask for the image, so that only the area inside the light circle is visible

    circle(originalImage, darkCenter, darkRadius, Scalar(0, 255, 0), 2, LINE_AA);
    circle(originalImage, lightCenter, lightRadius, Scalar(0, 255, 0), 2, LINE_AA);

    imshow("Dark Circle", maskedImage);


    waitKey(0); // Wait for a key press

    return 0;
}