function varargout = PA22X_Demo(varargin)
% PA22X_DEMO MATLAB code for PA22X_Demo.fig
%      PA22X_DEMO, by itself, creates a new PA22X_DEMO or raises the existing
%      singleton*.
%
%      H = PA22X_DEMO returns the handle to a new PA22X_DEMO or the handle to
%      the existing singleton*.
%
%      PA22X_DEMO('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in PA22X_DEMO.M with the given input arguments.
%
%      PA22X_DEMO('Property','Value',...) creates a new PA22X_DEMO or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before PA22X_Demo_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to PA22X_Demo_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help PA22X_Demo

% Last Modified by GUIDE v2.5 27-Mar-2017 09:59:04

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
    'gui_Singleton',  gui_Singleton, ...
    'gui_OpeningFcn', @PA22X_Demo_OpeningFcn, ...
    'gui_OutputFcn',  @PA22X_Demo_OutputFcn, ...
    'gui_LayoutFcn',  [] , ...
    'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Outputs from this function are returned to the command line.
function varargout = PA22X_Demo_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes just before PA22X_Demo is made visible.
function PA22X_Demo_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to PA22X_Demo (see VARARGIN)

% Choose default command line output for PA22X_Demo
global beam beamnum_max scale1 scale2 pixelac pointx pointy v0 v1 v2 x0 distt1 distt2 num...
    agatex_start agatey_start agatex_end agatey_end bgatex_start bgatey_start...
    bgatex_end bgatey_end analyticalx_start analyticaly_start analyticalx_end...
    analyticaly_end range_value software_flag beam_num;
handles.output = hObject;
% 连接到相控阵仪器，得到的命令端口的网络连接sc和数据端口的网络连接sd。
handles.sc = varargin{1};
handles.sd = varargin{2};
SendCommandToServer(handles.sc, 'set open_dev');
%%服务端执行法则初始化，可根据提供给用户的实际探头、楔块参数在下面进行修改好之后，再提供给客户；
SendCommandToServer(handles.sc, 'Set scan_type sscan');
SendCommandToServer(handles.sc, 'Set velocity 5900 3230');
SendCommandToServer(handles.sc, 'Set probe 2.5 16 0.55 0');
SendCommandToServer(handles.sc, 'Set wedge enable s 35.5 2337 10.5');
SendCommandToServer(handles.sc, 'Set aperture 1 16');
SendCommandToServer(handles.sc, 'Set angle 40 70');
SendCommandToServer(handles.sc, 'Set focus 50');
SendCommandToServer(handles.sc, 'Set scan_rule');
SendCommandToServer(handles.sc, 'set data_start');
%收发、增益以及范围等参数的初始化
again_ini = ['set again', ' ', get(handles.again, 'string')];
SendCommandToServer(handles.sc, again_ini);
dgain_ini = ['set dgain', ' ', get(handles.dgain, 'string')];
SendCommandToServer(handles.sc, dgain_ini);
hgain_value = str2double(get(handles.again, 'string')) + str2double(get(handles.dgain, 'string'));
set(handles.hgain, 'string', num2str(hgain_value));
high_voltage_ini = ['set high_voltage', ' ', get(handles.high_voltage, 'string')];
SendCommandToServer(handles.sc, high_voltage_ini);
pulse_width_ini = ['set pulse_width', ' ', get(handles.pulse_width, 'string')];
SendCommandToServer(handles.sc, pulse_width_ini);
prf_ini = ['set prf', ' ', get(handles.prf, 'string')];
SendCommandToServer(handles.sc, prf_ini);
range_ini = ['set range', ' ', get(handles.range, 'string')];
SendCommandToServer(handles.sc, range_ini);
[beamnum_max, beam, scale1, scale2, pixelac, software_flag,...
    analyticalx_start, analyticaly_start, analyticalx_end, analyticaly_end,...
    agatex_start, agatey_start, agatex_end, agatey_end,...
    bgatex_start, bgatey_start, bgatex_end, bgatey_end, beam_num, range_value, d] = sortout(handles.sc, handles);
handles.imgrect_wave = get(handles.axes_Tawave, 'position');
handles.path_pixelac = range_value / (handles.imgrect_wave(4) - 1);
handles.amp_pixelac = 100 / (handles.imgrect_wave(3));
%初始化计算软件成像的旋转平移操作
[pointx, pointy, v0, v1, v2, x0, distt1, distt2, num] = Shift_ad_rotate(beam, scale1, pixelac, d, beamnum_max);
%初始化计算闸门系数，为了减轻定时器内部运算量
handles.gatecof = 16 * 5.9 * 10^(-3) / 2;
%定义一个定时器，添加到handles结构体中，方便后面使用
handles.t_pa22x = timer;
%ExecutionMode执行的模式
set(handles.t_pa22x, 'ExecutionMode', 'FixedRate');
%时钟周期
set(handles.t_pa22x, 'Period', 0.08);
%定时器的执行函数，定时器开始，handles执行定时器的TimerFcn函数
set(handles.t_pa22x, 'TimerFcn', {@TimerFcn, handles});
%启动定时器
start(handles.t_pa22x);
% 设置窗口关闭的回调函数
%（该回调函数是以定时器“t”为参数的自定义DeleteFcn函数）
%关闭窗口即停止定时器
set(handles.figure_PA, 'DeleteFcn', {@figure_PA_DeleteFcn, handles});
% Update handles structure
guidata(hObject, handles);
% UIWAIT makes PA22X_Demo wait for user response (see UIRESUME)
% uiwait(handles.figure_PA);


% --- Executes during object deletion, before destroying properties.
function figure_PA_DeleteFcn(hObject, eventdata, handles)
% hObject    handle to figure_PA (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
stop(handles.t_pa22x);
delete(handles.t_pa22x);
SendCommandToServer(handles.sc, 'Set data_stop');
SendCommandToServer(handles.sc, 'Set close_dev disconnect');


%定时器的执行函数
function TimerFcn(hObject, eventdata, handles)
global scale1 scale2 pointx pointy v0 v1 v2 x0 distt1 distt2 num...
    agatex_start agatey_start agatex_end agatey_end bgatex_start bgatey_start...
    bgatex_end bgatey_end analyticalx_start analyticaly_start analyticalx_end...
    analyticaly_end range_value beam_num software_flag;
%SendCommandToServer(handles.sc, 'set data_start');
% 从数据端口sd仪器读取一个mtld数据包
[t, d, l, e] = ReadDataPackageFromServer(handles.sd);
if software_flag == 0
    % 判断数据包类型，'Tsimg512'类型的数据包包含一幅图像
    %画图（图像，波形，分析线）
    if (strcmpi(t, 'Tsimg512'))
        %     axes(handles.axes_Tsimg);
        set(handles.figure_PA, 'CurrentAxes', handles.axes_Tsimg);
        image(d);
        hold on
        plot([analyticalx_start analyticalx_end], [analyticaly_start analyticaly_end], 'g')
        hold off
        ys_axis(400, 0, range_value)
        x_axis(512, scale1, scale2)
    elseif (strcmpi(t, 'Tawave__'))
        %更新闸门输出
        % 闸门A
        patha = d(449:450, beam_num);
        patha_point = 255 * patha(2) + patha(1);
        patha_mm = round(10 * patha_point * handles.gatecof) / 10;
        ampa = round(10 * d(451:451, beam_num) / 255 * 100) / 10;
        set(handles.agwave_pos, 'string', [num2str(patha_mm), ' ', 'mm']);
        set(handles.agwave_ssty, 'string', [num2str(ampa), ' ', '%']);
        % 闸门B
        pathb = d(453:454, beam_num);
        pathb_point = 255 * pathb(2) + pathb(1);
        pathb_mm = round(10 * pathb_point * handles.gatecof) / 10;
        ampb = round(10 * d(455:455, beam_num) / 255 * 100) / 10;
        set(handles.bgwave_pos, 'string', [num2str(pathb_mm), ' ', 'mm']);
        set(handles.bgwave_ssty, 'string', [num2str(ampb), ' ', '%']);
        %画波形
        %     axes(handles.axes_Tawave);
        set(handles.figure_PA, 'CurrentAxes', handles.axes_Tawave);
        plot(d(1:400, beam_num), 400: -1: 1);
        hold on
        %更新画闸门
        plot([agatex_start agatex_end], [agatey_start agatey_end], 'r')
        hold off
        hold on
        plot([bgatex_start bgatex_end], [bgatey_start bgatey_end], 'g')
        hold off
        y_axis(400, 0, range_value)
        x_axis(250, 0, 100)
    end
elseif software_flag == 1
    if strcmpi(t, 'Tawave__')
        %画波形
        %更新闸门输出
        % 闸门A
        patha = d(449:450, beam_num);
        patha_point = 255 * patha(2) + patha(1);
        patha_mm = round(10 * patha_point * handles.gatecof) / 10;
        ampa = round(10 * d(451:451, beam_num) / 255 * 100) / 10;
        set(handles.agwave_pos, 'string', [num2str(patha_mm), ' ', 'mm']);
        set(handles.agwave_ssty, 'string', [num2str(ampa), ' ', '%']);
        % 闸门B
        pathb = d(453:454, beam_num);
        pathb_point = 255 * pathb(2) + pathb(1);
        pathb_mm = round(10 * pathb_point * handles.gatecof) / 10;
        ampb = round(10 * d(455:455, beam_num) / 255 * 100) / 10;
        set(handles.bgwave_pos, 'string', [num2str(pathb_mm), ' ', 'mm']);
        set(handles.bgwave_ssty, 'string', [num2str(ampb), ' ', '%']);
        %画波形
        %     axes(handles.axes_Tawave);
        set(handles.figure_PA, 'CurrentAxes', handles.axes_Tawave);
        plot(d(1:400, beam_num), 400: -1: 1);
        hold on
        %更新画闸门
        plot([agatex_start agatex_end], [agatey_start agatey_end], 'r')
        hold off
        hold on
        plot([bgatex_start bgatex_end], [bgatey_start bgatey_end], 'g')
        hold off
        y_axis(400, 0, range_value)
        x_axis(250, 0, 100)
        %画图像
        set(handles.figure_PA, 'CurrentAxes', handles.axes_Tsimg);
        dd = zeros(400, 512);
        dd(:, :) = 255;
        dd = interpolation(pointx, pointy, v0, v1, v2, x0, distt1, distt2, num, dd);
        image(dd)
        hold on
        plot([analyticalx_start analyticalx_end], [analyticaly_start analyticaly_end], 'g')
        hold off
        ys_axis(400, 0, range_value)
        x_axis(512, scale1, scale2)
    end
end


% --- Executes on scroll wheel click while the figure is in focus.
function figure_PA_WindowScrollWheelFcn(hObject, eventdata, handles)
% hObject    handle to figure_PA (see GCBO)
% eventdata  structure with the following fields (see FIGURE)
%	VerticalScrollCount: signed integer indicating direction and number of clicks
%	VerticalScrollAmount: number of lines scrolled for each click
% handles    structure with handles and user data (see GUIDATA)
global beam scale1 pixelac ...
    agatex_start agatey_start agatex_end agatey_end bgatex_start bgatey_start...
    bgatex_end bgatey_end analyticalx_start analyticaly_start analyticalx_end...
    analyticaly_end range_value beam_num beamnum_max;
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节模拟增益，并把模拟增益值通过命令端口sc，以字符串形式传送给服务端￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
%获取handles.again控件在handles.figure1上的坐标
again_position = get(handles.again, 'Position');
%获取鼠标在handles.figure1上的坐标
mouse_position = get(handles.figure_PA, 'CurrentPoint');
%获取handles.again的初始字符串并转化为double型，赋值给again_valueold
again_valueold = str2double(get(handles.again, 'string'));
%获取handles.again_step按钮控件的字符串并转化为double型，赋值给again_valuestep
again_valuestep = str2double(get(handles.again_step, 'string'));
%判断鼠标是否在handles.again控件上
if mouse_position(1) > again_position(1) && mouse_position(1) < again_position(1) + again_position(3) &&...
        mouse_position(2) > again_position(2) && mouse_position(2) < again_position(2) + again_position(4)
    %鼠标在handles.again控件上，用again_valueold、again_valuestep以及滚轮滚动次数eventdata.VerticalScrollCount更新handles.again
    again_valuenew = again_valueold + again_valuestep * eventdata.VerticalScrollCount;
    if again_valuenew >= 0 && again_valuenew <= 55
        set(handles.again, 'string', num2str(again_valuenew));
    end
    %采用字符串连接的方式，形成'set again 16'形式的字符串，并赋值给again_valuenew
    again_valuestr = ['set again', ' ', get(handles.again, 'string')];
    %通过命令端口sc把again_valuenew传给服务端（仪器），更新模拟增益
    SendCommandToServer(handles.sc, again_valuestr);
    hgain_value = str2double(get(handles.again, 'string')) + str2double(get(handles.dgain, 'string'));
    set(handles.hgain, 'string', num2str(hgain_value));
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节数字增益，并把数字增益值通过命令端口sc，以字符串形式传送给服务端￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
dgain_position = get(handles.dgain, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
dgain_valueold = str2double(get(handles.dgain, 'string'));
dgain_valuestep = str2double(get(handles.dgain_step, 'string'));
if mouse_position(1) > dgain_position(1) && mouse_position(1) < dgain_position(1) + dgain_position(3) &&...
        mouse_position(2) > dgain_position(2) && mouse_position(2) < dgain_position(2) + dgain_position(4)
    dgain_valuenew = dgain_valueold + dgain_valuestep * eventdata.VerticalScrollCount;
    if dgain_valuenew >= 0 && dgain_valuenew <= 36
        set(handles.dgain, 'string', num2str(dgain_valuenew));
    end
    dgain_valuestr = ['set dgain', ' ', get(handles.dgain, 'string')];
    SendCommandToServer(handles.sc, dgain_valuestr);
    hgain_value = str2double(get(handles.again, 'string')) + str2double(get(handles.dgain, 'string'));
    set(handles.hgain, 'string', num2str(hgain_value));
end

%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节电压，并把电压值通过命令端口sc，以字符串形式传送给服务端￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
high_voltage_position = get(handles.high_voltage, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
high_voltage_valueold = str2double(get(handles.high_voltage, 'string'));
high_voltage_valuestep = 5;
if mouse_position(1) > high_voltage_position(1) && mouse_position(1) < high_voltage_position(1) + high_voltage_position(3) &&...
        mouse_position(2) > high_voltage_position(2) && mouse_position(2) < high_voltage_position(2) + high_voltage_position(4)
    high_voltage_valuenew = high_voltage_valueold + high_voltage_valuestep * eventdata.VerticalScrollCount;
    if high_voltage_valuenew >= 0 && high_voltage_valuenew <= 100
        set(handles.high_voltage, 'string', num2str(high_voltage_valuenew));
    end
    high_voltage_valuestr = ['set high_voltage', ' ', get(handles.high_voltage, 'string')];
    SendCommandToServer(handles.sc, high_voltage_valuestr);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节脉冲宽度，并把脉冲宽度值通过命令端口sc，以字符串形式传送给服务端￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
pulse_width_position = get(handles.pulse_width, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
pulse_width_valueold = str2double(get(handles.pulse_width, 'string'));
pulse_width_valuestep = 10;
if mouse_position(1) > pulse_width_position(1) && mouse_position(1) < pulse_width_position(1) + pulse_width_position(3) &&...
        mouse_position(2) > pulse_width_position(2) && mouse_position(2) < pulse_width_position(2) + pulse_width_position(4)
    pulse_width_valuenew = pulse_width_valueold + pulse_width_valuestep * eventdata.VerticalScrollCount;
    if pulse_width_valuenew >= 25 && pulse_width_valuenew <= 600
        set(handles.pulse_width, 'string', num2str(pulse_width_valuenew));
    end
    pulse_width_valuestr = ['set pulse_width', ' ', get(handles.pulse_width, 'string')];
    SendCommandToServer(handles.sc, pulse_width_valuestr);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节重复频率，并把重复频率值通过命令端口sc，以字符串形式传送给服务端￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
prf_position = get(handles.prf, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
prf_valueold = str2double(get(handles.prf, 'string'));
prf_valuestep = str2double(get(handles.prf_step, 'string'));
if mouse_position(1) > prf_position(1) && mouse_position(1) < prf_position(1) + prf_position(3) &&...
        mouse_position(2) > prf_position(2) && mouse_position(2) < prf_position(2) + prf_position(4)
    prf_valuenew = prf_valueold + prf_valuestep * eventdata.VerticalScrollCount;
    if prf_valuenew >= 25 && prf_valuenew <= 10000
        set(handles.prf, 'string', num2str(prf_valuenew));
    end
    prf_valuestr = ['set prf', ' ', get(handles.prf, 'string')];
    SendCommandToServer(handles.sc, prf_valuestr);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节声束￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
wvindex_position = get(handles.waveindex, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
wvindex_valueold = str2double(get(handles.waveindex, 'string'));
wvindex_valuestep = 1;
if mouse_position(1) > wvindex_position(1) && mouse_position(1) < wvindex_position(1) + wvindex_position(3) &&...
        mouse_position(2) > wvindex_position(2) && mouse_position(2) < wvindex_position(2) + wvindex_position(4)
    wvindex_valuenew = wvindex_valueold + wvindex_valuestep * eventdata.VerticalScrollCount;
    if wvindex_valuenew >= 1 && wvindex_valuenew <= beamnum_max
        set(handles.waveindex, 'string', num2str(wvindex_valuenew))
    end
    %更新分析线
    beam_num = str2double(get(handles.waveindex, 'string'));
    analyticalx_start =  (str2double(beam{beam_num}(3)) - scale1) / pixelac;
    analyticaly_start = 0;
    analyticalx_end = analyticalx_start + 400 * sin(str2double(beam{beam_num}(2)) * pi / 180);
    analyticaly_end = analyticaly_start + 400 * cos(str2double(beam{beam_num}(2)) * pi / 180);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节范围，并把范围值通过命令端口sc，以字符串形式传送给服务端￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
range_position = get(handles.range, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
range_valueold = str2double(get(handles.range, 'string'));
range_valuestep = str2double(get(handles.range_step, 'string'));
if mouse_position(1) > range_position(1) && mouse_position(1) < range_position(1) + range_position(3) &&...
        mouse_position(2) > range_position(2) && mouse_position(2) < range_position(2) + range_position(4)
    range_valuenew = range_valueold + range_valuestep * eventdata.VerticalScrollCount;
    if range_valuenew >= 0 && range_valuenew <= 500
        set(handles.range, 'string', num2str(range_valuenew));
    end
    range_valuestr = ['set range', ' ', get(handles.range, 'string')];
    SendCommandToServer(handles.sc, range_valuestr);
    range_value = str2double(get(handles.range, 'string'));
    handles.path_pixelac = range_value / (handles.imgrect_wave(4) - 1);
    [agatex_start, agatey_start, agatex_end, agatey_end,...
        agpos_ini, agwidth_ini, agheight_ini] = agatecompute(handles);
    
    [bgatex_start, bgatey_start, bgatex_end, bgatey_end,...
        bgpos_ini, bgwidth_ini, bgheight_ini] = bgatecompute(handles);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节闸门A门位￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
agpos_position = get(handles.agpos, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
agpos_valueold = str2double(get(handles.agpos, 'string'));
agpos_valuestep = str2double(get(handles.agpos_step, 'string'));
if mouse_position(1) > agpos_position(1) && mouse_position(1) < agpos_position(1) + agpos_position(3) &&...
        mouse_position(2) > agpos_position(2) && mouse_position(2) < agpos_position(2) + agpos_position(4)
    agpos_valuenew = agpos_valueold + agpos_valuestep * eventdata.VerticalScrollCount;
    if agpos_valuenew >= 0 && agpos_valuenew <= range_valueold
        set(handles.agpos, 'string', num2str(agpos_valuenew));
    end
    range_value = str2double(get(handles.range, 'string'));
    handles.path_pixelac = range_value / (handles.imgrect_wave(4) - 1);
    [agatex_start, agatey_start, agatex_end, agatey_end,...
        agpos_ini, agwidth_ini, agheight_ini] = agatecompute(handles);
    SendCommandToServer(handles.sc, 'set gate_ctl a peek false');
    agate_ini = ['set gate_pos a', ' ', agpos_ini, ' ', agwidth_ini, ' ', agheight_ini];
    SendCommandToServer(handles.sc, agate_ini);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节闸门A门宽￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
agwidth_position = get(handles.agwidth, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
agwidth_valueold = str2double(get(handles.agwidth, 'string'));
agwidth_valuestep = str2double(get(handles.agwidth_step, 'string'));
if mouse_position(1) > agwidth_position(1) && mouse_position(1) < agwidth_position(1) + agwidth_position(3) &&...
        mouse_position(2) > agwidth_position(2) && mouse_position(2) < agwidth_position(2) + agwidth_position(4)
    agwidth_valuenew = agwidth_valueold + agwidth_valuestep * eventdata.VerticalScrollCount;
    if agwidth_valuenew >= 0 && agwidth_valuenew <= range_valueold
        set(handles.agwidth, 'string', num2str(agwidth_valuenew));
    end
    range_value = str2double(get(handles.range, 'string'));
    handles.path_pixelac = range_value / (handles.imgrect_wave(4) - 1);
    [agatex_start, agatey_start, agatex_end, agatey_end,...
        agpos_ini, agwidth_ini, agheight_ini] = agatecompute(handles);
    SendCommandToServer(handles.sc, 'set gate_ctl a peek false');
    agate_ini = ['set gate_pos a', ' ', agpos_ini, ' ', agwidth_ini, ' ', agheight_ini];
    SendCommandToServer(handles.sc, agate_ini);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节闸门A门高￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
agheight_position = get(handles.agheight, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
agheight_valueold = str2double(get(handles.agheight, 'string'));
agheight_valuestep = str2double(get(handles.agheight_step, 'string'));
if mouse_position(1) > agheight_position(1) && mouse_position(1) < agheight_position(1) + agheight_position(3) &&...
        mouse_position(2) > agheight_position(2) && mouse_position(2) < agheight_position(2) + agheight_position(4)
    agheight_valuenew = agheight_valueold + agheight_valuestep * eventdata.VerticalScrollCount;
    if agheight_valuenew >= 0 && agheight_valuenew <= 100
        set(handles.agheight, 'string', num2str(agheight_valuenew));
    end
    range_value = str2double(get(handles.range, 'string'));
    handles.path_pixelac = range_value / (handles.imgrect_wave(4) - 1);
    [agatex_start, agatey_start, agatex_end, agatey_end,...
        agpos_ini, agwidth_ini, agheight_ini] = agatecompute(handles);
    SendCommandToServer(handles.sc, 'set gate_ctl a peek false');
    agate_ini = ['set gate_pos a', ' ', agpos_ini, ' ', agwidth_ini, ' ', agheight_ini];
    SendCommandToServer(handles.sc, agate_ini);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节闸门B门位￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
bgpos_position = get(handles.bgpos, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
bgpos_valueold = str2double(get(handles.bgpos, 'string'));
bgpos_valuestep = str2double(get(handles.bgpos_step, 'string'));
if mouse_position(1) > bgpos_position(1) && mouse_position(1) < bgpos_position(1) + bgpos_position(3) &&...
        mouse_position(2) > bgpos_position(2) && mouse_position(2) < bgpos_position(2) + bgpos_position(4)
    bgpos_valuenew = bgpos_valueold + bgpos_valuestep * eventdata.VerticalScrollCount;
    if bgpos_valuenew >= 0 && bgpos_valuenew <= range_valueold
        set(handles.bgpos, 'string', num2str(bgpos_valuenew));
    end
    range_value = str2double(get(handles.range, 'string'));
    handles.path_pixelac = range_value / (handles.imgrect_wave(4) - 1);
    [bgatex_start, bgatey_start, bgatex_end, bgatey_end,...
        bgpos_ini, bgwidth_ini, bgheight_ini] = bgatecompute(handles);
    SendCommandToServer(handles.sc, 'set gate_ctl b peek false');
    bgate_ini = ['set gate_pos b', ' ', bgpos_ini, ' ', bgwidth_ini, ' ', bgheight_ini];
    SendCommandToServer(handles.sc, bgate_ini);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节闸门B门宽￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
bgwidth_position = get(handles.bgwidth, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
bgwidth_valueold = str2double(get(handles.bgwidth, 'string'));
bgwidth_valuestep = str2double(get(handles.bgwidth_step, 'string'));
if mouse_position(1) > bgwidth_position(1) && mouse_position(1) < bgwidth_position(1) + bgwidth_position(3) &&...
        mouse_position(2) > bgwidth_position(2) && mouse_position(2) < bgwidth_position(2) + bgwidth_position(4)
    bgwidth_valuenew = bgwidth_valueold + bgwidth_valuestep * eventdata.VerticalScrollCount;
    if bgwidth_valuenew >= 0 && bgwidth_valuenew <= range_valueold
        set(handles.bgwidth, 'string', num2str(bgwidth_valuenew));
    end
    range_value = str2double(get(handles.range, 'string'));
    handles.path_pixelac = range_value / (handles.imgrect_wave(4) - 1);
    [bgatex_start, bgatey_start, bgatex_end, bgatey_end,...
        bgpos_ini, bgwidth_ini, bgheight_ini] = bgatecompute(handles);
    SendCommandToServer(handles.sc, 'set gate_ctl b peek false');
    bgate_ini = ['set gate_pos b', ' ', bgpos_ini, ' ', bgwidth_ini, ' ', bgheight_ini];
    SendCommandToServer(handles.sc, bgate_ini);
end
%%%%%%%%%%%%%%￥￥￥￥￥￥￥￥滚轮调节闸门B门高￥￥￥￥￥￥￥￥%%%%%%%%%%%%%
bgheight_position = get(handles.bgheight, 'Position');
mouse_position = get(handles.figure_PA, 'CurrentPoint');
bgheight_valueold = str2double(get(handles.bgheight, 'string'));
bgheight_valuestep = str2double(get(handles.bgheight_step, 'string'));
if mouse_position(1) > bgheight_position(1) && mouse_position(1) < bgheight_position(1) + bgheight_position(3) &&...
        mouse_position(2) > bgheight_position(2) && mouse_position(2) < bgheight_position(2) + bgheight_position(4)
    bgheight_valuenew = bgheight_valueold + bgheight_valuestep * eventdata.VerticalScrollCount;
    if bgheight_valuenew >= 0 && bgheight_valuenew <= 100
        set(handles.bgheight, 'string', num2str(bgheight_valuenew));
    end
    range_value = str2double(get(handles.range, 'string'));
    handles.path_pixelac = range_value / (handles.imgrect_wave(4) - 1);
    [bgatex_start, bgatey_start, bgatex_end, bgatey_end,...
        bgpos_ini, bgwidth_ini, bgheight_ini] = bgatecompute(handles);
    SendCommandToServer(handles.sc, 'set gate_ctl b peek false');
    bgate_ini = ['set gate_pos b', ' ', bgpos_ini, ' ', bgwidth_ini, ' ', bgheight_ini];
    SendCommandToServer(handles.sc, bgate_ini);
end


% --- Executes on button press in again_step.
function again_step_Callback(hObject, eventdata, handles)
% hObject    handle to again_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if eval(get(handles.again_step, 'string')) == 0.1
    set(handles.again_step, 'string', '1');
elseif eval(get(handles.again_step, 'string')) == 1
    set(handles.again_step, 'string', '2');
elseif eval(get(handles.again_step, 'string')) == 2
    set(handles.again_step, 'string', '6');
else
    set(handles.again_step, 'string', '0.1');
end


% --- Executes on button press in dgain_step.
function dgain_step_Callback(hObject, eventdata, handles)
% hObject    handle to dgain_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if eval(get(handles.dgain_step, 'string')) == 0.1
    set(handles.dgain_step, 'string', '1');
elseif eval(get(handles.dgain_step, 'string')) == 1
    set(handles.dgain_step, 'string', '2');
elseif eval(get(handles.dgain_step, 'string')) == 2
    set(handles.dgain_step, 'string', '6');
else
    set(handles.dgain_step, 'string', '0.1');
end


% --- Executes on button press in prf_step.
function prf_step_Callback(hObject, eventdata, handles)
% hObject    handle to prf_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if eval(get(handles.prf_step, 'string')) == 50
    set(handles.prf_step, 'string', '100');
elseif eval(get(handles.prf_step, 'string')) == 100
    set(handles.prf_step, 'string', '50');
end


% --- Executes on button press in range_step.
function range_step_Callback(hObject, eventdata, handles)
% hObject    handle to range_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% --- Executes on button press in edit2_step.
if eval(get(handles.range_step, 'string')) == 0.1
    set(handles.range_step, 'string', '1');
elseif eval(get(handles.range_step, 'string')) == 1
    set(handles.range_step, 'string', '10');
elseif eval(get(handles.range_step, 'string')) == 10
    set(handles.range_step, 'string', '100');
else
    set(handles.range_step, 'string', '0.1');
end


% --- Executes during object creation, after setting all properties.
function again_CreateFcn(hObject, eventdata, handles)
% hObject    handle to again (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.

if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function range_CreateFcn(hObject, eventdata, handles)
% hObject    handle to range (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function dgain_CreateFcn(hObject, eventdata, handles)
% hObject    handle to dgain (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function high_voltage_CreateFcn(hObject, eventdata, handles)
% hObject    handle to high_voltage (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function pulse_width_CreateFcn(hObject, eventdata, handles)
% hObject    handle to pulse_width (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function prf_CreateFcn(hObject, eventdata, handles)
% hObject    handle to prf (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in filter.
function filter_Callback(hObject, eventdata, handles)
% hObject    handle to filter (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if get(handles.filter,'Value') == 1
    filter_str = ['set filter', ' ', '0'];
elseif get(handles.filter,'Value') == 2
    filter_str = ['set filter', ' ', '1'];
else
    filter_str = ['set filter', ' ', '2'];
end
SendCommandToServer(handles.sc, filter_str);


% --- Executes during object creation, after setting all properties.
function filter_CreateFcn(hObject, eventdata, handles)
% hObject    handle to filter (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function agpos_CreateFcn(hObject, eventdata, handles)
% hObject    handle to agpos (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in agpos_step.
function agpos_step_Callback(hObject, eventdata, handles)
% hObject    handle to agpos_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if eval(get(handles.agpos_step, 'string')) == 0.1
    set(handles.agpos_step, 'string', '1');
elseif eval(get(handles.agpos_step, 'string')) == 1
    set(handles.agpos_step, 'string', '10');
else
    set(handles.agpos_step, 'string', '0.1');
end


% --- Executes during object creation, after setting all properties.
function agwidth_CreateFcn(hObject, eventdata, handles)
% hObject    handle to agwidth (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in agwidth_step.
function agwidth_step_Callback(hObject, eventdata, handles)
% hObject    handle to agwidth_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if eval(get(handles.agwidth_step, 'string')) == 0.1
    set(handles.agwidth_step, 'string', '1');
elseif eval(get(handles.agwidth_step, 'string')) == 1
    set(handles.agwidth_step, 'string', '10');
else
    set(handles.agwidth_step, 'string', '0.1');
end


% --- Executes during object creation, after setting all properties.
function agheight_CreateFcn(hObject, eventdata, handles)
% hObject    handle to agheight (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in agheight_step.
function agheight_step_Callback(hObject, eventdata, handles)
% hObject    handle to agheight_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if eval(get(handles.agheight_step, 'string')) == 0.1
    set(handles.agheight_step, 'string', '1');
else
    set(handles.agheight_step, 'string', '0.1');
end


% --- Executes during object creation, after setting all properties.
function bgpos_CreateFcn(hObject, eventdata, handles)
% hObject    handle to bgpos (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in bgpos_step.
function bgpos_step_Callback(hObject, eventdata, handles)
% hObject    handle to bgpos_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if eval(get(handles.bgpos_step, 'string')) == 0.1
    set(handles.bgpos_step, 'string', '1');
elseif eval(get(handles.bgpos_step, 'string')) == 1
    set(handles.bgpos_step, 'string', '10');
else
    set(handles.bgpos_step, 'string', '0.1');
end


% --- Executes during object creation, after setting all properties.
function bgwidth_CreateFcn(hObject, eventdata, handles)
% hObject    handle to bgwidth (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function bgheight_CreateFcn(hObject, eventdata, handles)
% hObject    handle to bgheight (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in bgwidth_step.
function bgwidth_step_Callback(hObject, eventdata, handles)
% hObject    handle to bgwidth_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if eval(get(handles.bgwidth_step, 'string')) == 0.1
    set(handles.bgwidth_step, 'string', '1');
elseif eval(get(handles.bgwidth_step, 'string')) == 1
    set(handles.bgwidth_step, 'string', '10');
else
    set(handles.bgwidth_step, 'string', '0.1');
end


% --- Executes on button press in bgheight_step.
function bgheight_step_Callback(hObject, eventdata, handles)
% hObject    handle to bgheight_step (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if eval(get(handles.bgheight_step, 'string')) == 0.1
    set(handles.bgheight_step, 'string', '1');
else
    set(handles.bgheight_step, 'string', '0.1');
end


% --- Executes during object creation, after setting all properties.
function hgain_CreateFcn(hObject, eventdata, handles)
% hObject    handle to hgain (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function agwave_pos_CreateFcn(hObject, eventdata, handles)
% hObject    handle to agwave_pos (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function agwave_ssty_CreateFcn(hObject, eventdata, handles)
% hObject    handle to agwave_ssty (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function bgwave_pos_CreateFcn(hObject, eventdata, handles)
% hObject    handle to bgwave_pos (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function bgwave_ssty_CreateFcn(hObject, eventdata, handles)
% hObject    handle to bgwave_ssty (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.
function waveindex_CreateFcn(hObject, eventdata, handles)
% hObject    handle to waveindex (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in scantype.
function scantype_Callback(hObject, eventdata, handles)
% hObject    handle to scantype (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns scantype contents as cell array
%        contents{get(hObject,'Value')} returns selected item from scantype
if get(handles.scantype,'Value') == 1
    scantype_str = ['set scan_type', ' ', 'sscan']
else
    scantype_str = ['set scan_type', ' ', 'lscan']
end
SendCommandToServer(handles.sc, scantype_str);


% --- Executes during object creation, after setting all properties.
function scantype_CreateFcn(hObject, eventdata, handles)
% hObject    handle to scantype (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in velocity_set.
function velocity_set_Callback(hObject, eventdata, handles)
% hObject    handle to velocity_set (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% clear axes
setvel(handles.sc, handles.sd);

% --- Executes on button press in probe_set.
function probe_set_Callback(hObject, eventdata, handles)
% hObject    handle to probe_set (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
setprobe(handles.sc, handles.sd);

% --- Executes on button press in wedge_set.
function wedge_set_Callback(hObject, eventdata, handles)
% hObject    handle to wedge_set (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
setwedge(handles.sc, handles.sd);

% --- Executes on button press in apecture_set.
function apecture_set_Callback(hObject, eventdata, handles)
% hObject    handle to apecture_set (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if get(handles.scantype,'Value') == 1
    setsapecture(handles.sc, handles.sd);
else
    setlapecture(handles.sc, handles.sd);
end


% --- Executes on button press in angle_set.
function angle_set_Callback(hObject, eventdata, handles)
% hObject    handle to angle_set (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if get(handles.scantype,'Value') == 1
    setsangle(handles.sc, handles.sd);
else
    setlangle(handles.sc, handles.sd);
end

% --- Executes on button press in scanrule_set.
function scanrule_set_Callback(hObject, eventdata, handles)
% hObject    handle to scanrule_set (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global beam beamnum_max scale1 scale2 pixelac pointx pointy v0 v1 v2 x0 distt1 distt2 num...
    agatex_start agatey_start agatex_end agatey_end bgatex_start bgatey_start...
    bgatex_end bgatey_end analyticalx_start analyticaly_start analyticalx_end...
    analyticaly_end range_value software_flag beam_num;
focus_ini = ['set foucs', ' ', get(handles.focus_set, 'string')];
SendCommandToServer(handles.sc, focus_ini);
scanrule_ini = 'Set scan_rule';
SendCommandToServer(handles.sc, scanrule_ini);
set(handles.waveindex, 'string', '1');

%暂停定时器
stop(handles.t_pa22x)
SendCommandToServer(handles.sc, 'set data_stop');
[beamnum_max, beam, scale1, scale2, pixelac, software_flag,...
    analyticalx_start, analyticaly_start, analyticalx_end, analyticaly_end,...
    agatex_start, agatey_start, agatex_end, agatey_end,...
    bgatex_start, bgatey_start, bgatex_end, bgatey_end, beam_num, range_value, d] = sortout(handles.sc, handles);

[pointx, pointy, v0, v1, v2, x0, distt1, distt2, num] = Shift_ad_rotate(beam, scale1, pixelac, d, beamnum_max);
%启动定时器
start(handles.t_pa22x)


% --- Executes during object creation, after setting all properties.
function focus_set_CreateFcn(hObject, eventdata, handles)
% hObject    handle to focus_set (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in software_img.
function software_img_Callback(hObject, eventdata, handles)
global software_flag;
% hObject    handle to software_img (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
software_flag = get(handles.software_img, 'value');
% Hint: get(hObject,'Value') returns toggle state of software_img



function focus_set_Callback(hObject, eventdata, handles)
% hObject    handle to focus_set (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of focus_set as text
%        str2double(get(hObject,'String')) returns contents of focus_set as a double
