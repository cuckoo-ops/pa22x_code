function varargout = setvel(varargin)
% SETVEL MATLAB code for setvel.fig
%      SETVEL, by itself, creates a new SETVEL or raises the existing
%      singleton*.
%
%      H = SETVEL returns the handle to a new SETVEL or the handle to
%      the existing singleton*.
%
%      SETVEL('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SETVEL.M with the given input arguments.
%
%      SETVEL('Property','Value',...) creates a new SETVEL or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before setvel_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to setvel_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help setvel

% Last Modified by GUIDE v2.5 09-Mar-2017 09:42:42

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @setvel_OpeningFcn, ...
                   'gui_OutputFcn',  @setvel_OutputFcn, ...
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


% --- Executes just before setvel is made visible.
function setvel_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to setvel (see VARARGIN)

% Choose default command line output for setvel
handles.output = hObject;

handles.sc = varargin{1};
handles.sd = varargin{2};

% set(gcf,'WindowStyle','modal')
% set(gcf,'WindowStyle','normal')
% Update handles structure
guidata(hObject, handles);

% UIWAIT makes setvel wait for user response (see UIRESUME)
% uiwait(handles.figure_vel);


% --- Outputs from this function are returned to the command line.
function varargout = setvel_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function l_vel_Callback(hObject, eventdata, handles)
% hObject    handle to l_vel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of l_vel as text
%        str2double(get(hObject,'String')) returns contents of l_vel as a double


% --- Executes during object creation, after setting all properties.
function l_vel_CreateFcn(hObject, eventdata, handles)
% hObject    handle to l_vel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function s_vel_Callback(hObject, eventdata, handles)
% hObject    handle to s_vel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of s_vel as text
%        str2double(get(hObject,'String')) returns contents of s_vel as a double


% --- Executes during object creation, after setting all properties.
function s_vel_CreateFcn(hObject, eventdata, handles)
% hObject    handle to s_vel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in okvel.
function okvel_Callback(hObject, eventdata, handles)
% hObject    handle to okvel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% set velocity 5900 3230
velocity_ini = ['set velocity', ' ', get(handles.l_vel, 'string'), ' ', get(handles.s_vel, 'string')]
SendCommandToServer(handles.sc, velocity_ini);
delete(handles.figure_vel);
