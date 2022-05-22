function varargout = setlangle(varargin)
% SETLANGLE MATLAB code for setlangle.fig
%      SETLANGLE, by itself, creates a new SETLANGLE or raises the existing
%      singleton*.
%
%      H = SETLANGLE returns the handle to a new SETLANGLE or the handle to
%      the existing singleton*.
%
%      SETLANGLE('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SETLANGLE.M with the given input arguments.
%
%      SETLANGLE('Property','Value',...) creates a new SETLANGLE or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before setlangle_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to setlangle_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help setlangle

% Last Modified by GUIDE v2.5 09-Mar-2017 16:57:28

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @setlangle_OpeningFcn, ...
                   'gui_OutputFcn',  @setlangle_OutputFcn, ...
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


% --- Executes just before setlangle is made visible.
function setlangle_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to setlangle (see VARARGIN)

% Choose default command line output for setlangle
handles.output = hObject;

handles.sc = varargin{1};
handles.sd = varargin{2};

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes setlangle wait for user response (see UIRESUME)
% uiwait(handles.figure_langle);


% --- Outputs from this function are returned to the command line.
function varargout = setlangle_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function langle_Callback(hObject, eventdata, handles)
% hObject    handle to langle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of langle as text
%        str2double(get(hObject,'String')) returns contents of langle as a double


% --- Executes during object creation, after setting all properties.
function langle_CreateFcn(hObject, eventdata, handles)
% hObject    handle to langle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in okangle.
function okangle_Callback(hObject, eventdata, handles)
% hObject    handle to okangle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
angle_ini = ['set angle', ' ', get(handles.langle, 'string'), ' ', '90']
SendCommandToServer(handles.sc, angle_ini);
delete(handles.figure_langle);
