function varargout = setlapecture(varargin)
% SETLAPECTURE MATLAB code for setlapecture.fig
%      SETLAPECTURE, by itself, creates a new SETLAPECTURE or raises the existing
%      singleton*.
%
%      H = SETLAPECTURE returns the handle to a new SETLAPECTURE or the handle to
%      the existing singleton*.
%
%      SETLAPECTURE('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SETLAPECTURE.M with the given input arguments.
%
%      SETLAPECTURE('Property','Value',...) creates a new SETLAPECTURE or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before setlapecture_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to setlapecture_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help setlapecture

% Last Modified by GUIDE v2.5 09-Mar-2017 16:37:49

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @setlapecture_OpeningFcn, ...
                   'gui_OutputFcn',  @setlapecture_OutputFcn, ...
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


% --- Executes just before setlapecture is made visible.
function setlapecture_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to setlapecture (see VARARGIN)

% Choose default command line output for setlapecture
handles.output = hObject;

handles.sc = varargin{1};
handles.sd = varargin{2};

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes setlapecture wait for user response (see UIRESUME)
% uiwait(handles.figure_lapecture);


% --- Outputs from this function are returned to the command line.
function varargout = setlapecture_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function elestart_Callback(hObject, eventdata, handles)
% hObject    handle to elestart (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of elestart as text
%        str2double(get(hObject,'String')) returns contents of elestart as a double


% --- Executes during object creation, after setting all properties.
function elestart_CreateFcn(hObject, eventdata, handles)
% hObject    handle to elestart (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function apecture_Callback(hObject, eventdata, handles)
% hObject    handle to apecture (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of apecture as text
%        str2double(get(hObject,'String')) returns contents of apecture as a double


% --- Executes during object creation, after setting all properties.
function apecture_CreateFcn(hObject, eventdata, handles)
% hObject    handle to apecture (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function eleend_Callback(hObject, eventdata, handles)
% hObject    handle to eleend (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of eleend as text
%        str2double(get(hObject,'String')) returns contents of eleend as a double


% --- Executes during object creation, after setting all properties.
function eleend_CreateFcn(hObject, eventdata, handles)
% hObject    handle to eleend (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in okapecturel.
function okapecturel_Callback(hObject, eventdata, handles)
% hObject    handle to okapecturel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
lapecture_ini = ['set aperture', ' ', get(handles.elestart, 'string'), ' ', get(handles.apecture, 'string')]
lscan_eleini = ['set lscan_ele', ' ', get(handles.elestart, 'string'), ' ', get(handles.eleend, 'string')]
SendCommandToServer(handles.sc, lapecture_ini);
SendCommandToServer(handles.sc, lscan_eleini);
delete(handles.figure_lapecture);
