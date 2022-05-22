
function [] = TestReadFile(filename) 
    fid = fopen(filename, 'r');

    while(true)
        [t, d, l, e] = ReadDataPackageFromFile(fid);
        if (strcmpi(e, 'eof'))
            break;    
        elseif (strcmpi(t, 'Tcwave__'))
            figure(1);
            plot(d(: ,6));
        elseif (strcmpi(t, 'Tdimg___'))
            DisplayRGB565(d);
        end
        pause(0.01);
    end

    fprintf('finished!\r\n');
    fclose(fid);
end

