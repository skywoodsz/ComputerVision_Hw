/*
 * 功能：Camera calibration
 * Created by ZhangTianlin on 2021/10/12.
 */
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;
using namespace cv;
int main()
{
    ifstream fin("../calibdata.txt");
    ofstream fout("../calib_result.txt");

    int image_count = 0;
    int count = -1;
    cv::Size image_size, board_size;
    board_size = cv::Size (6, 9);
    vector<cv::Point2f> image_points_buf;
    vector<vector<cv::Point2f>> image_points_seq;
    string  filename;

    std::cout<<"image read ..."<<std::endl;
    while(getline(fin,filename))
    {
        image_count++;

        // step1: img I/O
        cv::Mat ImageInput= cv::imread(filename);
        std::cout<<"image read"<<filename<<std::endl;
        if(!ImageInput.data)
        {
            std::cout<<"error! don't find the pic"<<std::endl;
            exit(0);
        }
        if(image_count == 1) // get pic size
        {
            image_size.width = ImageInput.cols;
            image_size.height = ImageInput.rows;
        }

        // step2: exact coner point
        cv::findChessboardCorners(ImageInput, board_size, image_points_buf);

        cv::Mat ImageGray;
        cv::cvtColor(ImageInput, ImageGray, cv::COLOR_BGR2GRAY);

        cv::find4QuadCornerSubpix(ImageGray, image_points_buf,
                                  cv::Size(5, 5)); //精细化角点坐标

        image_points_seq.push_back(image_points_buf); // 加入至全局

        cv::drawChessboardCorners(ImageGray, board_size,
          image_points_buf, true); // 绘制
        cv::imshow("ImageGrayCorner", ImageGray);
        // cv::waitKey(100); // 0.5s
    }

    // step3: 創建世界坐標系
    cv::Size square_size = cv::Size(30, 30); // grid length 30mm
    vector<vector<Point3f>> object_points; // 世界坐標

    for(int k = 0; k < image_count; k++)
    {
        vector<cv::Point3f> tempPoint;
        for (int i = 0; i < board_size.height; i++)
        {
            for(int j = 0; j < board_size.width; j++)
            {
                cv::Point3f realPoint;
                realPoint.x = i * square_size.width;
                realPoint.y = j * square_size.height;
                realPoint.z = 0;
                tempPoint.push_back(realPoint);
            }
        }
        object_points.push_back(tempPoint);
    }

    // step4: 標定
    std::cout<<"strat to calibrate"<< std::endl;

    cv::Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0)); // 內參
    vector<int> point_counts;
    cv::Mat distCoeffs = cv::Mat(1, 5, CV_32FC1, Scalar::all(0)); // 畸變

    vector<cv::Mat> tvecsMat;
    vector<cv::Mat> rvecsMat;

    calibrateCamera(object_points,image_points_seq,
                    image_size,cameraMatrix,distCoeffs,
                    rvecsMat,tvecsMat,0);
    std::cout<<"calibrate finish!"<<std::endl;

    std::cout<<"Camera Matrix："<<std::endl;
    std::cout<<cameraMatrix<<endl<<std::endl;
    std::cout<<"Camera distCoeffs：\n";
    std::cout<<distCoeffs<<std::endl;

    // I/O
    fout<<"Camera Matrix：\n"<<endl;
    fout<<cameraMatrix<<endl<<endl;
    fout<<"Camera distCoeffs：\n";
    fout<<distCoeffs<<endl<<endl<<endl;

    // reprojection
    for (int i=0;i<image_count;i++)
    {
        point_counts.push_back(board_size.width*board_size.height);
    }

    double total_err = 0.0;
    double err = 0.0;
    vector<Point2f> image_points2;
    cout<<"\t每幅图像的标定误差：\n";
    for (int i=0;i<image_count;i++)
    {
        vector<Point3f> tempPointSet=object_points[i];

        projectPoints(tempPointSet,rvecsMat[i],
                      tvecsMat[i],cameraMatrix,
                      distCoeffs,image_points2);
        cout<<"reprojection:"<<endl;

        vector<Point2f> tempImagePoint = image_points_seq[i];
        Mat tempImagePointMat = Mat(1,tempImagePoint.size(),CV_32FC2);
        Mat image_points2Mat = Mat(1,image_points2.size(), CV_32FC2);
        cout<<"tempImagePoint.size():"<<tempImagePoint.size()<<endl;
        for (int j = 0 ; j < tempImagePoint.size(); j++)
        {
            image_points2Mat.at<Vec2f>(0,j) = Vec2f(image_points2[j].x, image_points2[j].y);
            tempImagePointMat.at<Vec2f>(0,j) = Vec2f(tempImagePoint[j].x, tempImagePoint[j].y);
        }

        err = norm(image_points2Mat, tempImagePointMat, NORM_L2);
        cout<<"err"<<err<<endl;
        total_err += err/=  point_counts[i];
        cout<<"total_err"<<total_err<<endl;
        std::cout<<"第"<<i+1<<"幅图像的平均误差："<<err<<"像素"<<endl;

    }
    std::cout<<"总体平均误差："<<total_err/image_count<<"像素"<<endl;
    std::cout<<"评价完成！"<<endl;

    return 0;
};

