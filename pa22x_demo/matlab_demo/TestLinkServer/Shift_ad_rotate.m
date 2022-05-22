function [pointx, pointy, v0, v1, v2, x0, distt1, distt2, num] = Shift_ad_rotate(beam, scale1, pixelac, d, beamnum_max)
for i = 1 : beamnum_max
    cos_v(i) = cos(str2double(beam{i}(2)) * pi / 180);
    sin_v(i) = sin(str2double(beam{i}(2)) * pi / 180);
    tan_v(i) = tan(str2double(beam{i}(2)) * pi / 180);
    x(i) = (str2double(beam{i}(3)) - scale1) / pixelac;
end
i_min = 1;
i_max = round(400 * cos_v(1));
j_min = round(x(1));
j_max = round(x(beamnum_max) + 400 * sin_v(beamnum_max));
num = 1;
for i = i_min : i_max
    for j = j_min : j_max
        for b = 1 : (beamnum_max - 1)
            if x(b) + (i - 1) * tan_v(b) <= j && j <= x(b + 1) + (i - 1) * tan_v(b + 1)
                x0(num) = (j - x(b)) * cos_v(b) - i * sin_v(b);
                y0(num) = round(i * cos_v(b) + (j - x(b)) * sin_v(b));
                x1(num) = (j - x(b + 1)) * cos_v(b + 1) - i * sin_v(b + 1);
                y1(num) = round(i * cos_v(b + 1) + (j - x(b + 1)) * sin_v(b + 1));
                if y0(num) < 400 || y1(num) < 400
                    pointx(num) = i;
                    pointy(num) = j;
                    dist(num) = x0(num) - x1(num);
                    v0(num) =  d(y0(num) + 1, b);
                    v1(num) =  d(y0(num) + 1, b + 1);
                    v2(num) = (v0(num) + v1(num)) / 2;
                    distt1(num) = 0.25 * dist(num);
                    distt2(num) = 0.75 * dist(num);
                    num = num + 1;
                end
            end
        end
    end
end