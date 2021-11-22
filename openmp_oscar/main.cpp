#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <omp.h>


using namespace cv;
using namespace std;


cv::Mat transformToGrayScale(int count){
    Mat src = imread("/home/zinobe/CLionProjects/test6/base/" + to_string(count) + ".png", IMREAD_GRAYSCALE);
    Mat dst;
    Mat kernel = (Mat_<char>(3, 3) << 0,  1, 0, 1, -4, 1,  0,  1, 0);
    cv::filter2D(src, dst, src.depth(), kernel);
    imwrite("/home/zinobe/CLionProjects/test6/gray/" + to_string(count) + ".png", src);
    return src;

}

cv::Mat scaleFrame(int count){
    int width = 300;
    int height = 200;
    Mat resized;
    Mat src = imread("/home/zinobe/CLionProjects/test6/base/" + to_string(count) + ".png", IMREAD_GRAYSCALE);
    resize(src, resized, Size(width, height), INTER_LINEAR);
    imwrite("/home/zinobe/CLionProjects/test6/resize/" + to_string(count) + ".png", resized);
    cout << "Original Height and Width :" << src.rows << "x" << src.cols << endl;
    cout << "Resized Height and Width :" << resized.rows << "x" << resized.cols << endl;
    return resized;
}



void pixelToGrayScale(int count){
    omp_set_num_threads(4);
    String imagen = "/home/zinobe/CLionProjects/test6/base/" + to_string(count) + ".png";
    uchar threshhold = 127;
    Mat src = imread(imagen);
    imshow("Display Image", src);

    Mat dst;
    Mat kernel = (Mat_<char>(3, 3) << 0,  1, 0,
            1, -4, 1,
            0,  1, 0);

    cv::filter2D(src, dst, src.depth(), kernel);

    cout << to_string(src.channels()) << endl;
    cout << to_string(src.rows) << endl;
    cout << to_string(src.cols) << endl;
    cv::MatIterator_<cv::Vec3b> it;


    #pragma omp parallel for
    for(int r = 0; r < src.rows; r++) {
        cv::Vec3b* ptr = src.ptr<cv::Vec3b>(r);
        for(int c = 0; c < src.cols; c++) {
            ptr[c] = cv::Vec3b(ptr[c][0], ptr[c][2], ptr[c][1]);

        }
        printf("i = %d, THREAD:  %d\n", r, omp_get_thread_num());
    }

    for(it = src.begin<cv::Vec3b>(); it != src.end<cv::Vec3b>(); ++it)
    {
        (*it)[2] = 0;
        (*it)[1] = 1;
    }

    imwrite("/home/zinobe/CLionProjects/test6/gray_by_pixel/" + to_string(count) + ".png", src);

}

int main(int argc, char* argv[])
{
    int count = 1;
    Ptr<BackgroundSubtractor> pBackSub;
    pBackSub = createBackgroundSubtractorKNN();
    VideoCapture capture( samples::findFile( "/home/zinobe/CLionProjects/test6/oceans.mp4" ) );

    Mat frame, fgMask;
    while (true) {
        capture >> frame;

        pBackSub->apply(frame, fgMask);
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);
        stringstream fs;
        fs << capture.get(CAP_PROP_POS_FRAMES);
        string frames = fs.str();
        putText(frame, "Processing Video Frames:", cv::Point(25, 25), FONT_HERSHEY_SIMPLEX, 0.8 , cv::Scalar(0,0,0));

        putText(frame, frames.c_str(), cv::Point(365, 25), FONT_HERSHEY_SIMPLEX, 0.8 , cv::Scalar(0,0,0));

        imshow("Original", frame);
        imshow("Gray Scale", transformToGrayScale(count));
        imshow("Resize", scaleFrame(count));
        count = count + 1,
        imwrite("/home/zinobe/CLionProjects/test6/base/" + to_string(count) + ".png", frame);
        pixelToGrayScale(count);

        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
    }

    return 0;
}


