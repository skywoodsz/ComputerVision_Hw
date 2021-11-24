/*
 * 功能：Skin extractor
 * Created by Zhang Tianlin on 2021/11/24.
 */
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;
using namespace cv;
int l1, l2, l3;
int u1 = 256, u2 = 256, u3 = 256;
Mat img;
void inRange_callback(int ,void *)
{
    Mat dst;
    inRange(img, Scalar(l1, l2, l3),
            Scalar(u1, u2, u3), dst);
    imshow("img",dst);
}

int main() {

    img = imread("../pic/3.jpg");
    Mat dst1;
    if(!img.data)
    {
        std::cout<<"error! can't get the img!"<<std::endl;
    }
    resize(img, img, Size(640, 640), 0, 0, INTER_LINEAR);
    cvtColor(img, img, COLOR_BGR2Lab);
    namedWindow("img",WINDOW_AUTOSIZE);

    cv::createTrackbar("l1：","img",&l1,256,inRange_callback);
    cv::createTrackbar("u1：","img",&u1,256,inRange_callback);
    cv::createTrackbar("l2：","img",&l2,256,inRange_callback);
    cv::createTrackbar("u2：","img",&u2,256,inRange_callback);
    cv::createTrackbar("l3：","img",&l3,256,inRange_callback);
    cv::createTrackbar("u3：","img",&u3,256,inRange_callback);

    waitKey(0);
    std::cout<<l1<<", "<<l2<<", "<<l3<<","<<u1<<", "<<u2<<", "<<u3<<std::endl;
    return 0;
}
