/*
 * 功能：计算图像间的单应性矩阵
 * Created by ZhangTianlin on 2021/9/21.
 */
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main()
{
    // step 1: 检测棋盘角点
    cv::Size boardSize = cv::Size (6, 9);
    cv::Mat ImageInputTemplet = cv::imread("../pic/templet.png");
    cv::Mat ImageInputObject = cv::imread("../pic/left13.jpg");

    vector<cv::Point2f> TempletCornerPoint, ObjectCornerPoint; // 角点

    cv::findChessboardCorners(ImageInputTemplet, boardSize, TempletCornerPoint); //确定角点坐标
    cv::findChessboardCorners(ImageInputObject, boardSize, ObjectCornerPoint);

    cv::Mat TempletGray, ObjectGray;
    cv::cvtColor(ImageInputTemplet, TempletGray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(ImageInputObject, ObjectGray, cv::COLOR_BGR2GRAY);

    cv::find4QuadCornerSubpix(TempletGray, TempletCornerPoint,
                              cv::Size(5, 5)); //精细化角点坐标
    cv::find4QuadCornerSubpix(ObjectGray, ObjectCornerPoint,
                              cv::Size(5, 5));

    cv::drawChessboardCorners(TempletGray, boardSize,
                              TempletCornerPoint, true); // 绘制
//    cv::imshow("TempletGrayCorner", TempletGray);
//    cv::waitKey(0);

    cv::drawChessboardCorners(ObjectGray, boardSize,
                              ObjectCornerPoint, true);
//    cv::imshow("ObjectGrayCorner", ObjectGray);
//    cv::waitKey(0);

    // step 2: 测试匹配关系
    // Notes：角点匹配关系已确认，无需构造描述子寻找匹配关系
    for (int i = 0; i < ObjectCornerPoint.size(); i++)
    {
        if(i==2 || i==4)
        {
            cv::circle(TempletGray, TempletCornerPoint[i], 1, cv::Scalar(0, 255, 127), 2, 8, 0);
            cv::circle(ObjectGray, ObjectCornerPoint[i], 1, cv::Scalar(0, 255, 127), 2, 8, 0);
        }
    }
//    cv::imshow("0", TempletGray);
//    cv::waitKey(0);
//    cv::imshow("1", ObjectGray);
//    cv::waitKey(0);

    // step 3: 计算单应矩阵
    // Notes: Object = H.T * Templet
    Mat homograpy_matrix;
    homograpy_matrix = cv::findHomography(TempletCornerPoint, ObjectCornerPoint,
                                          cv::RANSAC, 3, cv::noArray(),
                                          2000, 0.99);

    cout<<"homograpy_matrix is"<<endl<<homograpy_matrix<<endl;
    return 0;
}