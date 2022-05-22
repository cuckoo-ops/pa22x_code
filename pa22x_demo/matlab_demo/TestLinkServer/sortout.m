function [beamnum_max, beam, scale1, scale2, pixelac, software_flag,...
    analyticalx_start, analyticaly_start, analyticalx_end, analyticaly_end,...
    agatex_start, agatey_start, agatex_end, agatey_end,...
    bgatex_start, bgatey_start, bgatex_end, bgatey_end, beam_num, range_value, d] = sortout(inputsc, handles)
%获取初始化法则
scanrule = SendCommandToServer(inputsc, 'get scan_rule')
scanrule_splited = regexp(scanrule, ' ', 'split');
beamnum_max = (length(scanrule_splited) - 2) / 3;
for i = 1 : beamnum_max
    beam{i} = scanrule_splited((i * 3) : (i * 3 + 2));
end
%获取水平标尺scale1,scale2
scale_str = SendCommandToServer(inputsc, 'get img_scale');
scale_str_splited = regexp(scale_str, ' ', 'split');
scale1 = round(str2double(scale_str_splited{3}));
scale2 = round(str2double(scale_str_splited{4}));
imgrect_img = get(handles.axes_Tsimg, 'position');
pixelac = (scale2 - scale1) / (imgrect_img(3) - 1);
SendCommandToServer(inputsc, 'set data_start');
software_flag = get(handles.software_img, 'value');
%初始化分析线为第一条声束
beam_num = str2double(get(handles.waveindex, 'string'));
analyticalx_start =  (str2double(beam{beam_num}(3)) - scale1) / pixelac;
analyticaly_start = 0;
analyticalx_end = analyticalx_start + 400 * sin(str2double(beam{beam_num}(2)) * pi / 180);
analyticaly_end = analyticaly_start + 400 * cos(str2double(beam{beam_num}(2)) * pi / 180);
% 初始化闸门输入门位、门高、门宽，取gui控件的初始化默认值
%更新闸门输入
range_value = str2double(get(handles.range, 'string'));
imgrect_wave = get(handles.axes_Tawave, 'position');
handles.path_pixelac = range_value / (imgrect_wave(4) - 1);
handles.amp_pixelac = 100 / (imgrect_wave(3));
%闸门A
agpos_ini = get(handles.agpos, 'string');
agwidth_ini = get(handles.agwidth, 'string');
agheight_ini = get(handles.agheight, 'string');
agpos_num = str2double(agpos_ini);
agwidth_num = str2double(agwidth_ini);
agheight_num = str2double(agheight_ini);
agatex_start = agheight_num / handles.amp_pixelac;
agatey_start = imgrect_wave(4) - agpos_num / handles.path_pixelac;
agatex_end = agatex_start;
agatey_end = imgrect_wave(4) - (agpos_num + agwidth_num) / handles.path_pixelac;
SendCommandToServer(inputsc, 'set gate_ctl a peek false');
agate_ini = ['set gate_pos a', ' ', agpos_ini, ' ', agwidth_ini, ' ', agheight_ini];
SendCommandToServer(inputsc, agate_ini);
%闸门B
bgpos_ini = get(handles.bgpos, 'string');
bgwidth_ini = get(handles.bgwidth, 'string');
bgheight_ini = get(handles.bgheight, 'string');
bgpos_num = str2double(bgpos_ini);
bgwidth_num = str2double(bgwidth_ini);
bgheight_num = str2double(bgheight_ini);
bgatex_start = bgheight_num / handles.amp_pixelac;
bgatey_start = imgrect_wave(4) - bgpos_num / handles.path_pixelac;
bgatex_end = bgatex_start;
bgatey_end = imgrect_wave(4) - (bgpos_num + bgwidth_num) / handles.path_pixelac;
SendCommandToServer(inputsc, 'set gate_ctl b peek false');
bgate_ini = ['set gate_pos b', ' ', bgpos_ini, ' ', bgwidth_ini, ' ', bgheight_ini];
SendCommandToServer(inputsc, bgate_ini);
ycount = 0;
for i = 1 : 100
    [t, d, l, e] = ReadDataPackageFromServer(handles.sd);%可在此设置断点产生mat数据并保存作为分析备用
    if (strcmpi(t, 'Tawave__'))
        ycount = ycount + 1;
        if ycount > 20
            break
        end
    end
end
