%
% 功能： 连接到CTS-PA22X仪器，并显示波形或图像
% 参数： server_ip - 仪器的ip地址字符串，如：'192.168.22.22'
%
function [] = Pa22xDemoUT(server_ip)
    
    % 定义我们要看波形的卡号和通道号
    BOARD = 21;     % 请修改为正确的板号，可能是80～84，或20～24
    CHNUM = 1;      % 请修改为所要的通道号，范围1～8
    
    % 初始化常规超声板卡
    [sc, sd] = ConnectToServer(server_ip);         % 连接到CTS-PA22X仪器，得到的命令端口的网络连接sc和数据端口的网络连接sd。
	cmd = sprintf('set dev_select %d', BOARD);
    SendCommandToServer(sc, cmd);                  % 通过命令端口sc向仪器发送命令，选择板卡
	cmd = sprintf('set ch_select %d', CHNUM);    
    SendCommandToServer(sc, cmd);                  % 通过命令端口sc向仪器发送命令，选择通道
    SendCommandToServer(sc, 'set range 220');      % 通过命令端口sc向仪器发送命令，调整范围
    SendCommandToServer(sc, 'set prf 400');        % 通过命令端口sc向仪器发送命令，调整重复频率
    SendCommandToServer(sc, 'set gain 38');        % 通过命令端口sc向仪器发送命令，调整增益
    SendCommandToServer(sc, 'set data_start');     % 通过命令端口sc向仪器发送命令，令仪器开始发送数据
    
    while(true)
        [t, d, l, e] = ReadDataPackageFromServer(sd);   % 从数据端口sd读取一个mtld数据包
        if (strcmpi(t, 'Tawavepe'))                     % 判断数据包类型，'Tawavepe'类型是常规超声A型波数据
            if ( d(1) == BOARD && d(5) == CHNUM )       % 判断卡和通道号
                 figure(1);
                 plot( d(129:(129+400)) );              % d的数据格式参见二次开发手册
            end
        pause(0.01);
    end
end
