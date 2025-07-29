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


using namespace cv;
using namespace std;

Vec4f chose_standard_circle(bool polarity){
    // Chooses a standard circle if no circles are found
    // If polarity is 0, don't invert image, best to find light on dark circles, oposite for polarity = 1
    Vec4f standardCircle;
    if (polarity){
        standardCircle = Vec4f(682,555,80,1); // Standard circle for dark annulus
    }
    else{
        standardCircle = Vec4f(682,555,30,1); // Standard circle for light annulus
    }
    
    return standardCircle;
}

Vec4f findCentralCircles (Mat inputImg, bool polarity = 0){
    // Find the central circles in the images making up dark the annulus
    // If polarity is 0, don't invert image, best to find light on dark circles, oposite for polarity = 1
    // Needs to implement standard circle if no circle is found
    Mat img;
    int lowerRadiusSpan;
    int upperRadiusSpan;

    Point standardCenter = Point(682,555);

    // GaussianBlur(inputImg, inputImg, Size(9, 9), 2);

    if (polarity){
        lowerRadiusSpan = 70;
        upperRadiusSpan = 90;
        bitwise_not(inputImg,img); // Inverts the image, so that the dark circles are now light
    }
    else{ // If polarity is 0, don't invert image, best to find light on dark circles, oposite for polarity = 1
        lowerRadiusSpan = 24;
        upperRadiusSpan = 40;
        img = inputImg;
    }
    
    vector<Vec4f> circles; // Vector that handels the output values of HoughCircles

    HoughCircles(img, circles, HOUGH_GRADIENT_ALT, 1, 500, 300, 0.9, lowerRadiusSpan, upperRadiusSpan);

    size_t circlesMostVotesIndex = 0;
    float circlesVotes = 0;

    if (circles.empty()){
        cout << "No circles found" << endl;
        return chose_standard_circle(polarity); // If no circles are found, return the standard circle
    }
    


    for( size_t i = 0; i < circles.size(); i++ ){
        Point circleCenter = Point(cvRound(circles[i][0]), cvRound(circles[i][1]));
        if (norm(circleCenter - standardCenter)>20){
            return chose_standard_circle(polarity); // If the circle is not close to the standard center, return the standard circle
        }
        
        if (cvRound(circles[i][3])>circlesVotes){ // Keeps track of the best circle.
            circlesVotes = cvRound(circles[i][3]); 
            circlesMostVotesIndex = i;
        }
    }
    
    

    return circles[circlesMostVotesIndex];
} 



Mat find_large_area(Mat inputImg){
    return inputImg;
}

Mat resize_Image(Mat inputImg, int scaleFactor){
    // Resizes the image by a scale factor
    // Used to increase the accuracy of the HoughCircles function
    Mat resizedImg;
    Size newSize(inputImg.size().width/scaleFactor, inputImg.size().height/scaleFactor);
    resize(inputImg, resizedImg, newSize, 0, 0, INTER_LINEAR);
    return resizedImg;
}

Mat masking_image(Mat inputImg, Vec4f circleParmas, bool polarity = 0, int extraCutoff = 0){
    // Creates a mask for the image, so that only the area inside the circle is visible
    Mat mask = Mat::zeros(inputImg.size(), CV_8UC1);
    Point2f center(circleParmas[0], circleParmas[1]);
    int radius = cvRound(circleParmas[2]);
    Scalar maskType;

    if (!polarity){
        // If polarity is 1, invert the mask, so that the area outside the circle is visible
        mask = Mat::ones(inputImg.size(), CV_8UC1);
        maskType = Scalar(0); // Set the mask type to 1, so that the area inside the circle is black
        circle(mask, center, radius+extraCutoff, maskType, -1, LINE_AA);
    }
    else{
        // If polarity is 0, keep the mask as it is, so that the area inside the circle is visible
        mask = Mat::zeros(inputImg.size(), CV_8UC1);
        maskType = Scalar(255); // Set the mask type to 0, so that the area outside the circle is black
        circle(mask, center, radius-extraCutoff, maskType, -1, LINE_AA);
    }
    

    Mat maskedImg;
    inputImg.copyTo(maskedImg, mask);
    return maskedImg;
}