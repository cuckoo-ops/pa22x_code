%
% ���ܣ� ���ӵ�CTS-PA22X����������ʾ���λ�ͼ��
% ������ server_ip - ������ip��ַ�ַ������磺'192.168.22.22'
%
function [] = Pa22xDemoUT(server_ip)
    
    % ��������Ҫ�����εĿ��ź�ͨ����
    BOARD = 21;     % ���޸�Ϊ��ȷ�İ�ţ�������80��84����20��24
    CHNUM = 1;      % ���޸�Ϊ��Ҫ��ͨ���ţ���Χ1��8
    
    % ��ʼ�����泬���忨
    [sc, sd] = ConnectToServer(server_ip);         % ���ӵ�CTS-PA22X�������õ�������˿ڵ���������sc�����ݶ˿ڵ���������sd��
	cmd = sprintf('set dev_select %d', BOARD);
    SendCommandToServer(sc, cmd);                  % ͨ������˿�sc�������������ѡ��忨
	cmd = sprintf('set ch_select %d', CHNUM);    
    SendCommandToServer(sc, cmd);                  % ͨ������˿�sc�������������ѡ��ͨ��
    SendCommandToServer(sc, 'set range 220');      % ͨ������˿�sc�������������������Χ
    SendCommandToServer(sc, 'set prf 400');        % ͨ������˿�sc������������������ظ�Ƶ��
    SendCommandToServer(sc, 'set gain 38');        % ͨ������˿�sc���������������������
    SendCommandToServer(sc, 'set data_start');     % ͨ������˿�sc���������������������ʼ��������
    
    while(true)
        [t, d, l, e] = ReadDataPackageFromServer(sd);   % �����ݶ˿�sd��ȡһ��mtld���ݰ�
        if (strcmpi(t, 'Tawavepe'))                     % �ж����ݰ����ͣ�'Tawavepe'�����ǳ��泬��A�Ͳ�����
            if ( d(1) == BOARD && d(5) == CHNUM )       % �жϿ���ͨ����
                 figure(1);
                 plot( d(129:(129+400)) );              % d�����ݸ�ʽ�μ����ο����ֲ�
            end
        pause(0.01);
    end
end
