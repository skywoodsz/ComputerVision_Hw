filename = 'noiseimg.bmp';
f = imread(filename);
f = rgb2gray(f);
% imshow(filename);

%% 均值滤波 42.6269ms
tic;
for i =1:100
    av_filter = fspecial('average',3);   
    img_av = imfilter(f, av_filter, 'conv');
end
toc;
t = toc * 1000;
imshow(img_av);
imwrite(img_med_filter, 'av_filter.bmp');

%% 中值滤波 77.4960ms
tic;
for i =1:100
    img_med_filter=medfilt2(f);
end
toc;
t = toc * 1000;
imshow(img_med_filter);
imwrite(img_med_filter, 'media_filter.bmp');

%% Guass滤波 162.4568ms
tic;
for i =1:100
    Iblur = imgaussfilt(f, 2);
end
imshow(Iblur);
toc;
t = toc * 1000;
imwrite(Iblur, 'guass_filter.bmp');



