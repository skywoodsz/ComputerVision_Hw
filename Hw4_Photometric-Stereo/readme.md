## 运行说明
本工程仅在ubuntu1604+Opencv4.0+vtk下进行调试，其他运行系统未经测试. 

## 配置
执行文件 ./main.cpp 

图片读取配置文件: ./load_img.txt 	
图片存储：./homework_images 	
光源向量文件: ./homework_images/light_directions.txt

*Notes:* 本工程假设光线方向已知, 深度恢复程序与Mesh显示程序参考 
<https://github.com/NewProggie/Photometric-Stereo>

## 求解结果 
法线向量结果文件: ./normal_result.txt 
法向量图文件: ./normal_img.png 
3D重建Mesh文件：./Mesh 
3D重建结果图: ./Mesh.png
