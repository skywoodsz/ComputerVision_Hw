/*
 * 功能：photometric stereo
 * Created by ZhangTianlin on 2021/10/14.
 */
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/opencv.hpp"
#include <Eigen/Dense>
#include <Eigen/Core>
#include <iostream>
#include <vector>
#include <fstream>

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkPLYWriter.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImageViewer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkRenderer.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkTriangle.h>

using namespace std;
using namespace cv;

/*
 * 1. 加载图像
 * 2. 加载亮度值
 * 3. 加载光方向
 * 4. 连理方程计算法线
 * 5. 估计深度
 * 6. 绘制 法线图, 法线向量图, 深度重建图
 */
void displayMesh(int width, int height, cv::Mat Z) {

    /* creating visualization pipeline which basically looks like this:
     vtkPoints -> vtkPolyData -> vtkPolyDataMapper -> vtkActor -> vtkRenderer */
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyDataMapper> modelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> modelActor = vtkSmartPointer<vtkActor>::New();
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkCellArray> vtkTriangles = vtkSmartPointer<vtkCellArray>::New();

    /* insert x,y,z coords */
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            points->InsertNextPoint(x, y, Z.at<float>(y,x));
        }
    }

    /* setup the connectivity between grid points */
    vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
    triangle->GetPointIds()->SetNumberOfIds(3);
    for (int i=0; i<height-1; i++) {
        for (int j=0; j<width-1; j++) {
            triangle->GetPointIds()->SetId(0, j+(i*width));
            triangle->GetPointIds()->SetId(1, (i+1)*width+j);
            triangle->GetPointIds()->SetId(2, j+(i*width)+1);
            vtkTriangles->InsertNextCell(triangle);
            triangle->GetPointIds()->SetId(0, (i+1)*width+j);
            triangle->GetPointIds()->SetId(1, (i+1)*width+j+1);
            triangle->GetPointIds()->SetId(2, j+(i*width)+1);
            vtkTriangles->InsertNextCell(triangle);
        }
    }
    polyData->SetPoints(points);
    polyData->SetPolys(vtkTriangles);

    /* create two lights */
    vtkSmartPointer<vtkLight> light1 = vtkSmartPointer<vtkLight>::New();
    light1->SetPosition(-1, 1, 1);
    renderer->AddLight(light1);
    vtkSmartPointer<vtkLight> light2 = vtkSmartPointer<vtkLight>::New();
    light2->SetPosition(1, -1, -1);
    renderer->AddLight(light2);

    /* meshlab-ish background */
    modelMapper->SetInputData(polyData);
    renderer->SetBackground(.45, .45, .9);
    renderer->SetBackground2(.0, .0, .0);
    renderer->GradientBackgroundOn();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    modelActor->SetMapper(modelMapper);

    /* setting some properties to make it look just right */
    modelActor->GetProperty()->SetSpecularColor(1, 1, 1);
    modelActor->GetProperty()->SetAmbient(0.2);
    modelActor->GetProperty()->SetDiffuse(0.2);
    modelActor->GetProperty()->SetInterpolationToPhong();
    modelActor->GetProperty()->SetSpecular(0.8);
    modelActor->GetProperty()->SetSpecularPower(8.0);

    renderer->AddActor(modelActor);
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);

    /* export mesh */
    vtkSmartPointer<vtkPLYWriter> plyExporter = vtkSmartPointer<vtkPLYWriter>::New();
    plyExporter->SetInputData(polyData);
    plyExporter->SetFileName("export.ply");
    plyExporter->SetColorModeToDefault();
    plyExporter->SetArrayName("Colors");
    plyExporter->Update();
    plyExporter->Write();

    /* render mesh */
    renderWindow->Render();
    interactor->Start();
}

cv::Mat globalHeights(cv::Mat Pgrads, cv::Mat Qgrads) {

    cv::Mat P(Pgrads.rows, Pgrads.cols, CV_32FC2, cv::Scalar::all(0));
    cv::Mat Q(Pgrads.rows, Pgrads.cols, CV_32FC2, cv::Scalar::all(0));
    cv::Mat Z(Pgrads.rows, Pgrads.cols, CV_32FC2, cv::Scalar::all(0));

    float lambda = 1.0f;
    float mu = 1.0f;

    cv::dft(Pgrads, P, cv::DFT_COMPLEX_OUTPUT);
    cv::dft(Qgrads, Q, cv::DFT_COMPLEX_OUTPUT);
    for (int i=0; i<Pgrads.rows; i++) {
        for (int j=0; j<Pgrads.cols; j++) {
            if (i != 0 || j != 0) {
                float u = sin((float)(i*2*CV_PI/Pgrads.rows));
                float v = sin((float)(j*2*CV_PI/Pgrads.cols));

                float uv = pow(u,2)+pow(v,2);
                float d = (1.0f + lambda)*uv + mu*pow(uv,2);
                Z.at<cv::Vec2f>(i, j)[0] = (u*P.at<cv::Vec2f>(i, j)[1] + v*Q.at<cv::Vec2f>(i, j)[1]) / d;
                Z.at<cv::Vec2f>(i, j)[1] = (-u*P.at<cv::Vec2f>(i, j)[0] - v*Q.at<cv::Vec2f>(i, j)[0]) / d;
            }
        }
    }

    /* setting unknown average height to zero */
    Z.at<cv::Vec2f>(0, 0)[0] = 0.0f;
    Z.at<cv::Vec2f>(0, 0)[1] = 0.0f;

    cv::dft(Z, Z, cv::DFT_INVERSE | cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);

    return Z;
}

void computeNormals(vector<cv::Mat> &img_seq,
                    Eigen::Matrix<double, 4, 3> &light_dir,
                    vector<double> &ryo_seq,
                    vector<Eigen::Matrix<double, 3,1>> &normal_seq
                    )
{
    Eigen::Matrix<double, 4, 1> I_pixel;
    Eigen::Matrix<double, 3,1> normal;
    double ryo;

    for (int i = 0; i < img_seq[0].rows; ++i)
    {
        for (int j = 0; j < img_seq[0].cols; ++j)
        {
            for (int k = 0; k < img_seq.size(); ++k)
            {
                // (y, x)
                I_pixel(k, 0) = img_seq[k].at<uchar>(Point(i, j));
            }

            normal = (light_dir.transpose() * light_dir).inverse()
                    * light_dir.transpose() * I_pixel;
            ryo = normal.norm();
            //    normal /= ryo;
            normal.normalize();

            normal_seq.push_back(normal);
            ryo_seq.push_back(ryo);

//            std::cout<<"ryo: \n"<<ryo<<std::endl;
//            std::cout<<"normal: \n"<<normal<<std::endl;
        }
    }
        ofstream fout("../normal_result.txt");

        for (int i = 0; i < normal_seq.size(); ++i)
        {
            fout<<normal_seq[i].transpose()<<endl;
        }
        fout<<"\n"<<endl<<endl;

}

void Load_Image(ifstream &fin, vector<cv::Mat> &img_seq)
{
    std::cout<<"load imgs ..."<< std::endl;
    string  filename;
    while (getline(fin, filename))
    {
        // step1: img I/O
        cv::Mat ImageInput= cv::imread(filename);
        std::cout<<"image read"<<filename<<std::endl;
        if(!ImageInput.data)
        {
            std::cout<<"error! don't find the pic"<<std::endl;
            exit(0);
        }
        cv::cvtColor(ImageInput, ImageInput, cv::COLOR_BGR2GRAY);
//        std::cout<<ImageInput.channels()<<std::endl;
        img_seq.push_back(ImageInput);
    }
}

int main() {
    // step 1, 2: load img and light
    ifstream  fin("../load_img.txt");
    int image_size = 0;
    vector<cv::Mat> img_seq;
    cv::Size img_row_col;

    Load_Image(fin, img_seq);

    image_size = img_seq.size();
    const int height = img_seq[0].rows;
    const int width = img_seq[0].cols;
    img_row_col = img_seq[0].size();

    std::cout<<"img_row_col: "<<img_row_col<<std::endl;
    std::cout<<"The count of img is: "<<image_size<<std::endl;
    //    cv::Point3d s1(0.0, 0.0, -1.0), s2(0.0, 0.2, -1.0),
    //    s3(0.0, -0.2, -1.0), s4(0.2, 0.0, -1.0);
    //    vector<cv::Point3d> light_dir = {s1, s2, s3, s4};
    Eigen::Vector3d s1, s2, s3, s4;
    s1 << 0.0, 0.0, -1.0;
    s2 << 0.0, 0.2, -1.0;
    s3 << 0.0, -0.2, -1.0;
    s4 << 0.2, 0.0, -1.0;
    Eigen::Matrix<double, 4, 3> light_dir;
    light_dir << s1.transpose(), s2.transpose(), s3.transpose(), s4.transpose();


    // step 4: 计算法线
    vector<Eigen::Matrix<double, 3,1>> normal_seq;
    vector<double> ryo_seq;

    computeNormals(img_seq, light_dir, ryo_seq, normal_seq);

    cv::Mat Normals(height, width, CV_32FC3, cv::Scalar::all(0));

    for (int i = 0; i < img_row_col.width; ++i)
    {
        for (int j = 0; j < img_row_col.height; ++j)
        {
            double sx, sy, sz;
            cv::Mat n;
            sx = normal_seq[i * img_row_col.height + j](0,0);
//                    * ryo_seq[i * img_row_col.height + j];
            sy = normal_seq[i * img_row_col.height + j](1,0);
//                    * ryo_seq[i * img_row_col.height + j];
            sz = normal_seq[i * img_row_col.height + j](2,0);
//                    * ryo_seq[i * img_row_col.height + j];
//            std::cout<<i * img_row_col.height + j<<std::endl;
//            std::cout<<"sx, sy, sz: "<<cv::Vec3f(sx, sy, sz)<<std::endl;
            Normals.at<cv::Vec3f>(cv::Point(i,j)) = cv::Vec3f(sx, sy, sz);
        }
    }

//    ofstream fout("../normal_result3.txt");
//    fout<<Normals<<endl;
//    fout<<"\n"<<endl<<endl;

    std::cout<<"NORMAL GET"<<std::endl;
    cv::Mat Normalmap;
    cv::cvtColor(Normals, Normalmap, COLOR_BGR2RGB);

    Normalmap.convertTo(Normalmap, CV_8U, 255.0);
//    std::cout<<Normalmap.type()<<std::endl;
    imwrite("../normal_img.png", Normalmap);
    cv::imshow("Normalmap", Normalmap);
//    cv::waitKey(0);

    // step 4: 计算Z
    cv::Mat Pgrads(height, width, CV_32F, cv::Scalar::all(0));
    cv::Mat Qgrads(height, width, CV_32F, cv::Scalar::all(0));

    for (int i = 0; i < img_row_col.width; ++i)
    {
        for (int j = 0; j < img_row_col.height; ++j)
        {
            cv::Vec3f n;
            n =  Normals.at<Vec3f>(j, i);
            Pgrads.at<float>(cv::Point(i, j)) = n[0] / n[2];

            Qgrads.at<float>(cv::Point(i, j)) = n[1] / n[2];
        }
    }
//    ofstream fout1("../normal_result_Pgrads.txt");
//    fout1<<Pgrads<<endl;
//    fout1<<"\n"<<endl<<endl;
//
//    ofstream fout2("../normal_result_Qgrads.txt");
//    fout2<<Qgrads<<endl;
//    fout2<<"\n"<<endl<<endl;

    cv::Mat Z = globalHeights(Pgrads, Qgrads);

    // step 5: 绘制
    displayMesh(Pgrads.cols, Pgrads.rows, Z);

    cv::waitKey();

    return 0;

}
