%
% ���ܣ����ӵ������������
% ������ipAddr - ������ip��ַ�ַ������磺'192.168.22.22'��
% ���أ�cmdSock - ������������˿���������ID��
%       dataSock - �����������ݶ˿���������ID��
%
function [cmdSock, dataSock] = ConnectToServer(ipAddr)
    cmdSock = tcpip(ipAddr, 2202);
    cmdSock.InputBufferSize = 1024 * 8;
    fopen(cmdSock);

    dataSock = tcpip(ipAddr, 2201);
    dataSock.InputBufferSize = 512 * 400 * 4;
    fopen(dataSock);
end
