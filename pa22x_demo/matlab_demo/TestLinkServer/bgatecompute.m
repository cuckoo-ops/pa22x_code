function [bgatex_start, bgatey_start, bgatex_end, bgatey_end,...
    bgpos_ini, bgwidth_ini, bgheight_ini] = bgatecompute(handles)
bgpos_ini = get(handles.bgpos, 'string');
bgpos_num = str2double(bgpos_ini);
bgwidth_ini = get(handles.bgwidth, 'string');
bgwidth_num = str2double(bgwidth_ini);
bgheight_ini = get(handles.bgheight, 'string');
bgheight_num = str2double(bgheight_ini);
bgatex_start = bgheight_num / handles.amp_pixelac;
bgatey_start = handles.imgrect_wave(4) - bgpos_num / handles.path_pixelac;
bgatex_end = bgatex_start;
bgatey_end = handles.imgrect_wave(4) - (bgpos_num + bgwidth_num) / handles.path_pixelac;