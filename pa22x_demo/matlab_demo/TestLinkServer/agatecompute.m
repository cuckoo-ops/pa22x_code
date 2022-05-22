function [agatex_start, agatey_start, agatex_end, agatey_end,...
    agpos_ini, agwidth_ini, agheight_ini] = agatecompute(handles)
agpos_ini = get(handles.agpos, 'string');
agpos_num = str2double(agpos_ini);
agwidth_ini = get(handles.agwidth, 'string');
agwidth_num = str2double(agwidth_ini);
agheight_ini = get(handles.agheight, 'string');
agheight_num = str2double(agheight_ini);
agatex_start = agheight_num / handles.amp_pixelac;
agatey_start = handles.imgrect_wave(4) - agpos_num / handles.path_pixelac;
agatex_end = agatex_start;
agatey_end = handles.imgrect_wave(4) - (agpos_num + agwidth_num) / handles.path_pixelac;