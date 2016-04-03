/*
 * * File:   main.cpp
 * * Author: sagar
 * *
 * * Created on 10 September, 2012, 7:48 PM
 * */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
using namespace cv;
using namespace std;

RNG rng(12345);
const int thresh = 100;
const int max_thresh = 255;

Mat yellowFilter(const Mat& src) {
    assert(src.type() == CV_8UC3);

    Mat colorOnly;
    inRange(src, Scalar(25, 220, 127), Scalar(50, 255, 255), colorOnly);

    return colorOnly;
}

Mat equalizeIntensity(const Mat& inputImage)
{
    if(inputImage.channels() >= 3)
    {
        Mat hsv;
        cvtColor(inputImage,hsv,CV_BGR2HSV);

        vector<Mat> channels;
        split(hsv,channels);

        equalizeHist(channels[1], channels[1]);

        Mat result;
        merge(channels,result);


        return result;
    }
    return Mat();
}

Vector<float> findObject(const Mat& inputImage) {
    Mat normFrame = equalizeIntensity(inputImage);
    Mat filtered = yellowFilter(normFrame);       
    
    Mat canny_output;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    // Blug Image
    blur(filtered, filtered, Size(3,3));

    // Detect edges using canny
    Canny( filtered, canny_output, thresh, thresh*2, 3 );

    // Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    // Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    float cx, cy;
    bool found = false;
    double maxarea = -1;
    for( int i = 0; i< contours.size(); i++ )
    {
        double area = contourArea(contours[i]); 
        if(area > maxarea) {
            maxarea = area;
            Moments M = moments(contours[i]);
            cx = (((M.m10/M.m00)/inputImage.size().width)*2)-1;
            cy = (((M.m01/M.m00)/inputImage.size().width)*2)-1;
            found = true;
        }
    }
    //Show in a window
    if(found) {
        cerr << cx << ", " << cy << endl;
        return vector<float>(cx,cy);
    } else
        return vector<float>();
}

Vector<Point> findObjects(const Mat& inputImage) {
    Mat normFrame = equalizeIntensity(inputImage);
    Mat filtered = yellowFilter(normFrame);       
    
    Vector<Point> objects;
    Mat canny_output;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    // Blug Image
    blur(filtered, filtered, Size(3,3));

    // Detect edges using canny
    Canny( filtered, canny_output, thresh, thresh*2, 3 );

    // Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    // Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
        double area = contourArea(contours[i]); 
        if(area > 45) {
            Moments M = moments(contours[i]);
            int cx = (int)(M.m10/M.m00);
            int cy = (int)(M.m01/M.m00);
            Point c = Point(cx,cy);
            cout << c << "|" << area << endl;
            objects.push_back(c);
        }
    }
    //Show in a window
    return objects;
}
