## 运行说明
本工程仅在ubuntu1604+Opencv4.0下进行调试，其他运行系统未经测试. 


## 运行结果  
- Time use in 100 blur is: 16.995ms  
- Time use in 100 medianBlur is: 19.107ms  
- Time use in 100 GaussianBlur is: 25.322ms  

- 均值滤波 "./blur_3x3.bmp" & "./blur_5x5.bmp"  
- 中值滤波 "./medianBlur_3x3.bmp" & "./medianBlur_5x5.bmp"  
- 高斯滤波 "./GaussianBlur_3x3.bmp" & "./GaussianBlur_5x5.bmp"  

matlab
- Time use in 100 blur is: 42.6269ms
- Time use in 100 medianBlur is: 77.4960ms
- Time use in 100 GaussianBlur is: 162.4568ms
- 均值滤波 "./matlab/av_filter.bmp"
- 中值滤波 "./matlab/media_filter.bmp"  
- 高斯滤波 "./matlab/guass_filter.bmp" 
