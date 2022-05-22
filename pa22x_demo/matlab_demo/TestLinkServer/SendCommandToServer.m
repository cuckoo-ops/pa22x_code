%
% 功能：向相控阵仪器发送一个命令。命令格式参见二次开发手册。
% 参数：fid - 与仪器的命令端口网络连接ID。 command - 要发送的命令字符串。
% 返回：result - 仪器返回的命令结果字符串。
%
function [result] = SendCommandToServer(fid, command) 
    cmd = sprintf('%s\r\n\r\n', command);
    fwrite(fid, cmd);

    [result, count] = fgets(fid);
    while (fid.BytesAvailable > 0)
        [line, count] = fgets(fid);
        result = strcat(result, line); 
    end
end
