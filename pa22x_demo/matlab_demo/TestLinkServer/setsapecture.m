function varargout = setsapecture(varargin)
% SETSAPECTURE MATLAB code for setsapecture.fig
%      SETSAPECTURE, by itself, creates a new SETSAPECTURE or raises the existing
%      singleton*.
%
%      H = SETSAPECTURE returns the handle to a new SETSAPECTURE or the handle to
%      the existing singleton*.
%
%      SETSAPECTURE('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SETSAPECTURE.M with the given input arguments.
%
%      SETSAPECTURE('Property','Value',...) creates a new SETSAPECTURE or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before setsapecture_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to setsapecture_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help setsapecture

% Last Modified by GUIDE v2.5 09-Mar-2017 16:37:59

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @setsapecture_OpeningFcn, ...
                   'gui_OutputFcn',  @setsapecture_OutputFcn, ...
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


% --- Executes just before setsapecture is made visible.
function setsapecture_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to setsapecture (see VARARGIN)

% Choose default command line output for setsapecture
handles.output = hObject;

handles.sc = varargin{1};
handles.sd = varargin{2};

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes setsapecture wait for user response (see UIRESUME)
% uiwait(handles.figure_sapecture);


% --- Outputs from this function are returned to the command line.
function varargout = setsapecture_OutputFcn(hObject, eventdata, handles) 
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


% --- Executes on button press in okapecture.
function okapecture_Callback(hObject, eventdata, handles)
% hObject    handle to okapecture (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
apecture_ini = ['set aperture', ' ', get(handles.elestart, 'string'), ' ', get(handles.apecture, 'string')]
SendCommandToServer(handles.sc, apecture_ini);
delete(handles.figure_sapecture);
