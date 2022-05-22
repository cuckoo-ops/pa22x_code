function varargout = setprobe(varargin)
% SETPROBE MATLAB code for setprobe.fig
%      SETPROBE, by itself, creates a new SETPROBE or raises the existing
%      singleton*.
%
%      H = SETPROBE returns the handle to a new SETPROBE or the handle to
%      the existing singleton*.
%
%      SETPROBE('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SETPROBE.M with the given input arguments.
%
%      SETPROBE('Property','Value',...) creates a new SETPROBE or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before setprobe_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to setprobe_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help setprobe

% Last Modified by GUIDE v2.5 09-Mar-2017 09:23:11

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @setprobe_OpeningFcn, ...
                   'gui_OutputFcn',  @setprobe_OutputFcn, ...
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


% --- Executes just before setprobe is made visible.
function setprobe_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to setprobe (see VARARGIN)

% Choose default command line output for setprobe
handles.output = hObject;

handles.sc = varargin{1};
handles.sd = varargin{2};

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes setprobe wait for user response (see UIRESUME)
% uiwait(handles.figure_probe);


% --- Outputs from this function are returned to the command line.
function varargout = setprobe_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function freq_Callback(hObject, eventdata, handles)
% hObject    handle to freq (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of freq as text
%        str2double(get(hObject,'String')) returns contents of freq as a double


% --- Executes during object creation, after setting all properties.
function freq_CreateFcn(hObject, eventdata, handles)
% hObject    handle to freq (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function elemnum_Callback(hObject, eventdata, handles)
% hObject    handle to elemnum (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of elemnum as text
%        str2double(get(hObject,'String')) returns contents of elemnum as a double


% --- Executes during object creation, after setting all properties.
function elemnum_CreateFcn(hObject, eventdata, handles)
% hObject    handle to elemnum (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function elemwidth_Callback(hObject, eventdata, handles)
% hObject    handle to elemwidth (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of elemwidth as text
%        str2double(get(hObject,'String')) returns contents of elemwidth as a double


% --- Executes during object creation, after setting all properties.
function elemwidth_CreateFcn(hObject, eventdata, handles)
% hObject    handle to elemwidth (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function delay_Callback(hObject, eventdata, handles)
% hObject    handle to delay (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of delay as text
%        str2double(get(hObject,'String')) returns contents of delay as a double


% --- Executes during object creation, after setting all properties.
function delay_CreateFcn(hObject, eventdata, handles)
% hObject    handle to delay (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in okprobe.
function okprobe_Callback(hObject, eventdata, handles)
% hObject    handle to okprobe (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
probe_ini = ['set probe', ' ', get(handles.freq, 'string'), ' ', get(handles.elemnum, 'string'),...
    ' ',  get(handles.elemwidth, 'string'), ' ',  get(handles.delay, 'string')]
SendCommandToServer(handles.sc, probe_ini);
delete(handles.figure_probe);
