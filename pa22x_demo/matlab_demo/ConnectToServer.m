%
% 功能：连接到相控阵仪器。
% 参数：ipAddr - 仪器的ip地址字符串，如：'192.168.22.22'。
% 返回：cmdSock - 与仪器的命令端口网络连接ID。
%       dataSock - 与仪器的数据端口网络连接ID。
%
function [cmdSock, dataSock] = ConnectToServer(ipAddr)
    cmdSock = tcpip(ipAddr, 2202);
    cmdSock.InputBufferSize = 1024 * 8;
    fopen(cmdSock);

    dataSock = tcpip(ipAddr, 2201);
    dataSock.InputBufferSize = 512 * 400 * 4;
    fopen(dataSock);
end
