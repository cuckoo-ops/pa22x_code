
function [pkgtype, data, length, err] = ReadDataPackageFromFile(fid) 
    err = '';
    mark1 = fread(fid, 1, 'uchar');
    mark2 = fread(fid, 1, 'uchar');
    mark3 = fread(fid, 1, 'uchar');
    mark4 = fread(fid, 1, 'uchar');
    while(~feof(fid))
        if (mark1 == 85 & mark2 == 170 & mark3 == 85 & mark4 == 170)
            break;
        else
            mark1 = mark2;
            mark2 = mark3;
            mark3 = mark4;
            mark4 = fread(fid, 1, 'uchar');
        end
    end;
    
    if (feof(fid))
        pkgtype = '';
        length = 0;
        data = 0;
        err = 'eof';
    else
        pkgtype = sprintf('%s', fread(fid, 8, '*char'));
        length = fread(fid, 1, 'uint32', 'b');
        length = length - hex2dec('22000000');
        if (pkgtype(1) ~= 'T' | length < 0 | length > hex2dec('FFFFFF'))
            err = 'bad package';
            data = 0;
        else if (strcmpi(pkgtype, 'Tcwave__'))
            data = fread(fid, [400, 128], 'uchar');
        elseif (strcmpi(pkgtype, 'Tcimage_'))
            data = fread(fid, [180, 3136], 'uchar');
        elseif (strcmpi(pkgtype, 'Tdimg___'))
            data = fread(fid, [800, 600], '*uint16', 'l');
        else
            data = fread(fid, length, 'uchar');
        end
    end
end
