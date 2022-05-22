%
% 功能：从相控阵仪器读取一个mtld数据包。
% 参数：fid - 与仪器的数据端口网络连接ID。
% 返回：pkgtype - mtld数据包的类型。 data - 读取到的mtld数据包。 
%      length - 数据包的长度。err - 如果读取错误，则err返回错误字符串。
%
function [pkgtype, data, length, err] = ReadDataPackageFromServer(fid) 
    err = '';
    mark1 = fread(fid, 1, 'uchar');
    mark2 = fread(fid, 1, 'uchar');
    mark3 = fread(fid, 1, 'uchar');
    mark4 = fread(fid, 1, 'uchar');
    while(fid.BytesAvailable)
        if (mark1 == 85 & mark2 == 170 & mark3 == 85 & mark4 == 170)
            break;
        else
            mark1 = mark2;
            mark2 = mark3;
            mark3 = mark4;
            mark4 = fread(fid, 1, 'uchar');
        end
    end;
    
    if (~fid.BytesAvailable)
        pkgtype = '';
        length = 0;
        data = 0;
        err = 'eof';
    else
        pkgtype = sprintf('%s', fread(fid, 8, 'char'));
        length = fread(fid, 1, 'uint32');
        length = length - hex2dec('22000000');
        if (pkgtype(1) ~= 'T' | length < 0 | length > hex2dec('FFFFFF'))
            err = 'bad package';
            data = 0;
        elseif (strcmpi(pkgtype, 'Tsimg512'))
            data = fread(fid, [512, 400], 'uchar');
            data = imrotate(data, 90);
        elseif (strcmpi(pkgtype, 'Tawave__'))
            data = fread(fid, [1024, 128], 'uchar');
        else
            data = fread(fid, length, 'uchar');
        end
    end
end
