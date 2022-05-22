%
% 功能： 连接到相控阵仪器，并显示A形波和图像
% 参数： server_ip - 相控阵仪器的ip地址字符串，如：'192.168.5.22'
%
function [] = TestLinkServer(server_ip)

    [sc, sd] = ConnectToServer(server_ip);      % 连接到相控阵仪器，得到的命令端口的网络连接sc和数据端口的网络连接sd。
    %初始化相控阵板卡
%     SendCommandToServer(sc, 'set again 38');    % 通过命令端口sc向仪器发送命令，调整模拟增益
%     SendCommandToServer(sc, 'set dgain 28');    % 通过命令端口sc向仪器发送命令，调整数字增益
%     SendCommandToServer(sc, 'set prf 2000');    % 通过命令端口sc向仪器发送命令，调整重复频率
%     SendCommandToServer(sc, 'set data_start');  % 通过命令端口sc向仪器发送命令，令仪器开始发送数据

    %初始化常规超声板卡
    SendCommandToServer(sc, 'set dev_select 20');  % 通过命令端口sc向仪器发送命令，调整模拟增益
    SendCommandToServer(sc, 'set ch_select 1');    % 通过命令端口sc向仪器发送命令，调整模拟增益
    SendCommandToServer(sc, 'set range 220');      % 通过命令端口sc向仪器发送命令，调整模拟增益
    SendCommandToServer(sc, 'set prf 400');        % 通过命令端口sc向仪器发送命令，调整模拟增益
    SendCommandToServer(sc, 'set gain 38');        % 通过命令端口sc向仪器发送命令，调整模拟增益
    SendCommandToServer(sc, 'set data_start');     % 通过命令端口sc向仪器发送命令，令仪器开始发送数据
    
    while(true)
        [t, d, l, e] = ReadDataPackageFromServer(sd);   % 从数据端口sd仪器读取一个mtld数据包
        if (strcmpi(t, 'Tawavepe'))                     % 判断数据包类型，'Tawavepe'类型是常规超声A型波数据
            if ( d(1) == 20 && d(5) == 1 )              % 判断卡号为20，通道号为1
                 figure(1);
                 plot( d(129:(129+400)) );
            end
%         elseif (strcmpi(t, 'Tsimg512'))             % 判断数据包类型，'Tsimg512'类型的数据包包含一幅图像
%             figure(1);
%             image(d);
%         elseif (strcmpi(t, 'Tawavepa'))         % 判断数据包类型，'Tsimg512'类型的数据包包含A型波数据
%              figure(2);
%              plot( d(1:400, 6) );
        end
        pause(0.01);
    end
end
