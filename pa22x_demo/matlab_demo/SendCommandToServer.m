%
% ���ܣ����������������һ����������ʽ�μ����ο����ֲᡣ
% ������fid - ������������˿���������ID�� command - Ҫ���͵������ַ�����
% ���أ�result - �������ص��������ַ�����
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
