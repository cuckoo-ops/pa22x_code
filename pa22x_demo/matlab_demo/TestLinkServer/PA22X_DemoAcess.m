function varargout = PA22X_DemoAcess(varargin)
% PA22X_DEMOACESS MATLAB code for PA22X_DemoAcess.fig
%      PA22X_DEMOACESS, by itself, creates a new PA22X_DEMOACESS or raises the existing
%      singleton*.
%
%      H = PA22X_DEMOACESS returns the handle to a new PA22X_DEMOACESS or the handle to
%      the existing singleton*.
%
%      PA22X_DEMOACESS('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in PA22X_DEMOACESS.M with the given input arguments.
%
%      PA22X_DEMOACESS('Property','Value',...) creates a new PA22X_DEMOACESS or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before PA22X_DemoAcess_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to PA22X_DemoAcess_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help PA22X_DemoAcess

% Last Modified by GUIDE v2.5 21-Mar-2017 15:03:53

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
    'gui_Singleton',  gui_Singleton, ...
    'gui_OpeningFcn', @PA22X_DemoAcess_OpeningFcn, ...
    'gui_OutputFcn',  @PA22X_DemoAcess_OutputFcn, ...
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


% --- Executes just before PA22X_DemoAcess is made visible.
function PA22X_DemoAcess_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to PA22X_DemoAcess (see VARARGIN)

% Choose default command line output for PA22X_DemoAcess
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes PA22X_DemoAcess wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = PA22X_DemoAcess_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function ip_address_Callback(hObject, eventdata, handles)
% hObject    handle to ip_address (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of ip_address as text
%        str2double(get(hObject,'String')) returns contents of ip_address as a double


% --- Executes during object creation, after setting all properties.
function ip_address_CreateFcn(hObject, eventdata, handles)
% hObject    handle to ip_address (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
PA22X_ip = get(handles.ip_address,'string');
[sc, sd] = ConnectToServer(PA22X_ip);
PA22X_Demo(sc,sd);
delete(handles.figure1);
