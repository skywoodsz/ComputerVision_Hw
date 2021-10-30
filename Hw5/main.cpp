/*
 * 功能：Image filter
 * Created by Zhang Tianlin on 2021/10/30.
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

int main()
{
    cv::Mat ImageInput= cv::imread("../noiseimg.bmp");
    if(!ImageInput.data)
    {
        std::cout<<"Can't find the image!"<<std::endl;
        exit(0);
    }
//    cv::imshow("ImageInput", ImageInput);
//    cv::waitKey(0);

    // 均值滤波
    cv::Mat blurIm1, blurIm2;
    blur(ImageInput, blurIm1, cv::Size(3, 3));
    blur(ImageInput, blurIm2, cv::Size(5, 5));
    imshow("blur 3x3", blurIm1);
    imshow("blur 5x5", blurIm2);
    cv::waitKey(0);
    imwrite("../blur_3x3.bmp", blurIm1);
    imwrite("../blur_5x5.bmp", blurIm2);

    // 中值滤波

    cv::Mat medianBlurIm1, medianBlurIm2;
    cv::medianBlur(ImageInput,medianBlurIm1,3);
    cv::medianBlur(ImageInput,medianBlurIm2,5);
    imshow("medianBlur_3x3", medianBlurIm1);
    imshow("medianBlur_5x5", medianBlurIm2);
    cv::waitKey(0);
    imwrite("../medianBlur_3x3.bmp", medianBlurIm1);
    imwrite("../medianBlur_5x5.bmp", medianBlurIm2);

    // Guass filter
    cv::Mat GuassBlurIm1, GuassBlurIm2;
    cv::GaussianBlur(ImageInput, GuassBlurIm1, cv::Size(3, 3), 3, 3);
    cv::GaussianBlur(ImageInput, GuassBlurIm2, cv::Size(5, 5), 3, 3);

    imshow("GaussianBlur_3x3", GuassBlurIm1);
    imshow("GaussianBlur_5x5", GuassBlurIm2);
    cv::waitKey(0);
    imwrite("../GaussianBlur_3x3.bmp", GuassBlurIm1);
    imwrite("../GaussianBlur_5x5.bmp", GuassBlurIm2);

    // 计时测试 100
    clock_t time_stt = clock();
    for (int i = 0; i < 100; ++i)
    {
        cv::blur(ImageInput, blurIm1, cv::Size(3, 3));
    }
    std::cout<<"Time use in 100 blur is: "<<
    1000 * (clock() - time_stt) / (double)CLOCKS_PER_SEC
    <<"ms"<<std::endl;

    time_stt = clock();
    for (int i = 0; i < 100; ++i)
    {
        cv::medianBlur(ImageInput,medianBlurIm1,3);
    }
    std::cout<<"Time use in 100 medianBlur is: "<<
    1000 * (clock() - time_stt) / (double)CLOCKS_PER_SEC
    <<"ms"<<std::endl;

    time_stt = clock();
    for (int i = 0; i < 100; ++i)
    {
        cv::GaussianBlur(ImageInput, GuassBlurIm1, cv::Size(3, 3), 3, 3);
    }
    std::cout<<"Time use in 100 GaussianBlur is: "<<
    1000 * (clock() - time_stt) / (double)CLOCKS_PER_SEC
    <<"ms"<<std::endl;
    return 0;
};
