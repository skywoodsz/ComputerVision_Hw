//
// Created by skywoodsz on 2021/11/24.
//
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
/*
 * Cheng: 80, 138, 132, 217, 166, 163
 * Women: 113, 140, 137, 211, 211, 172
 * Men: 134, 130, 127, 220, 151, 154
 */


int main()
{

    Mat img = imread("../pic/3.jpg");
    Mat dst, img2;
    if(!img.data)
    {
        std::cout<<"error! can't get the img!"<<std::endl;
    }
    resize(img, img, Size(640, 640), 0, 0, INTER_LINEAR);
    cvtColor(img, dst, COLOR_BGR2Lab);
    inRange(dst, Scalar(134, 130, 127), Scalar(220, 151, 154), dst);

    img.copyTo(img2, dst);
    imshow("img",img2);
    imwrite("../result/Men.jpg", img2);
    waitKey(0);
    return 0;
}
