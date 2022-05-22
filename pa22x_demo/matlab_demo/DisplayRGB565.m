function [ rgb888 ] = DisplayRGB565( rgb565 )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

imgR = uint8((255/31)*bitshift(bitand(rgb565,63488),-11));  %# Red component
imgG = uint8((255/63)*bitshift(bitand(rgb565,2016),-5));    %# Green component
imgB = uint8((255/31)*bitand(rgb565,31));                   %# Blue component
rgb888 = cat(3, imgR, imgG, imgB);  %# Concatenate along the third dimension

rgb888 = flipdim(rgb888, 2);
rgb888 = imrotate(rgb888, 90);
%imwrite(imgRGB,'myImage.bmp');   %# Output the image to a file
imshow(rgb888);
end
