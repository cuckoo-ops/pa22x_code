function dd = interpolation(pointx, pointy, v0, v1, v2, x0, distt1, distt2, num, dd)
for k = 1 : (num - 1)
    if x0(k) < distt1(k)
        dd(pointx(k), pointy(k)) = v0(k);
    elseif x0(k) < distt2(k)
        dd(pointx(k), pointy(k)) = v2(k);
    else
        dd(pointx(k), pointy(k)) = v1(k);
    end
end