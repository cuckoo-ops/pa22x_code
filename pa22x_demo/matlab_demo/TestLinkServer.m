%
% ���ܣ� ���ӵ����������������ʾA�β���ͼ��
% ������ server_ip - �����������ip��ַ�ַ������磺'192.168.5.22'
%
function [] = TestLinkServer(server_ip)

    [sc, sd] = ConnectToServer(server_ip);      % ���ӵ�������������õ�������˿ڵ���������sc�����ݶ˿ڵ���������sd��
    %��ʼ�������忨
%     SendCommandToServer(sc, 'set again 38');    % ͨ������˿�sc�����������������ģ������
%     SendCommandToServer(sc, 'set dgain 28');    % ͨ������˿�sc�������������������������
%     SendCommandToServer(sc, 'set prf 2000');    % ͨ������˿�sc������������������ظ�Ƶ��
%     SendCommandToServer(sc, 'set data_start');  % ͨ������˿�sc���������������������ʼ��������

    %��ʼ�����泬���忨
    SendCommandToServer(sc, 'set dev_select 20');  % ͨ������˿�sc�����������������ģ������
    SendCommandToServer(sc, 'set ch_select 1');    % ͨ������˿�sc�����������������ģ������
    SendCommandToServer(sc, 'set range 220');      % ͨ������˿�sc�����������������ģ������
    SendCommandToServer(sc, 'set prf 400');        % ͨ������˿�sc�����������������ģ������
    SendCommandToServer(sc, 'set gain 38');        % ͨ������˿�sc�����������������ģ������
    SendCommandToServer(sc, 'set data_start');     % ͨ������˿�sc���������������������ʼ��������
    
    while(true)
        [t, d, l, e] = ReadDataPackageFromServer(sd);   % �����ݶ˿�sd������ȡһ��mtld���ݰ�
        if (strcmpi(t, 'Tawavepe'))                     % �ж����ݰ����ͣ�'Tawavepe'�����ǳ��泬��A�Ͳ�����
            if ( d(1) == 20 && d(5) == 1 )              % �жϿ���Ϊ20��ͨ����Ϊ1
                 figure(1);
                 plot( d(129:(129+400)) );
            end
%         elseif (strcmpi(t, 'Tsimg512'))             % �ж����ݰ����ͣ�'Tsimg512'���͵����ݰ�����һ��ͼ��
%             figure(1);
%             image(d);
%         elseif (strcmpi(t, 'Tawavepa'))         % �ж����ݰ����ͣ�'Tsimg512'���͵����ݰ�����A�Ͳ�����
%              figure(2);
%              plot( d(1:400, 6) );
        end
        pause(0.01);
    end
end
