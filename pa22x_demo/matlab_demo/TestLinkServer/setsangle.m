function varargout = setsangle(varargin)
% SETSANGLE MATLAB code for setsangle.fig
%      SETSANGLE, by itself, creates a new SETSANGLE or raises the existing
%      singleton*.
%
%      H = SETSANGLE returns the handle to a new SETSANGLE or the handle to
%      the existing singleton*.
%
%      SETSANGLE('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SETSANGLE.M with the given input arguments.
%
%      SETSANGLE('Property','Value',...) creates a new SETSANGLE or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before setsangle_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to setsangle_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help setsangle

% Last Modified by GUIDE v2.5 09-Mar-2017 16:53:31

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @setsangle_OpeningFcn, ...
                   'gui_OutputFcn',  @setsangle_OutputFcn, ...
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


% --- Executes just before setsangle is made visible.
function setsangle_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to setsangle (see VARARGIN)

% Choose default command line output for setsangle
handles.output = hObject;

handles.sc = varargin{1};
handles.sd = varargin{2};


% Update handles structure
guidata(hObject, handles);

% UIWAIT makes setsangle wait for user response (see UIRESUME)
% uiwait(handles.figure_sangle);


% --- Outputs from this function are returned to the command line.
function varargout = setsangle_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function anglestart_Callback(hObject, eventdata, handles)
% hObject    handle to anglestart (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of anglestart as text
%        str2double(get(hObject,'String')) returns contents of anglestart as a double


% --- Executes during object creation, after setting all properties.
function anglestart_CreateFcn(hObject, eventdata, handles)
% hObject    handle to anglestart (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function angleend_Callback(hObject, eventdata, handles)
% hObject    handle to angleend (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of angleend as text
%        str2double(get(hObject,'String')) returns contents of angleend as a double


% --- Executes during object creation, after setting all properties.
function angleend_CreateFcn(hObject, eventdata, handles)
% hObject    handle to angleend (see GCBO)
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
angle_ini = ['set angle', ' ', get(handles.anglestart, 'string'), ' ', get(handles.angleend, 'string')]
% varargout{1} = handles.angle_ini;
SendCommandToServer(handles.sc, angle_ini);
delete(handles.figure_sangle);
