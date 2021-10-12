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
        cv::waitKey(100); // 0.5s
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
    return 0;
};