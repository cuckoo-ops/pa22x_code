function varargout = setwedge(varargin)
% SETWEDGE MATLAB code for setwedge.fig
%      SETWEDGE, by itself, creates a new SETWEDGE or raises the existing
%      singleton*.
%
%      H = SETWEDGE returns the handle to a new SETWEDGE or the handle to
%      the existing singleton*.
%
%      SETWEDGE('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SETWEDGE.M with the given input arguments.
%
%      SETWEDGE('Property','Value',...) creates a new SETWEDGE or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before setwedge_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to setwedge_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help setwedge

% Last Modified by GUIDE v2.5 09-Mar-2017 10:27:18

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @setwedge_OpeningFcn, ...
                   'gui_OutputFcn',  @setwedge_OutputFcn, ...
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


% --- Executes just before setwedge is made visible.
function setwedge_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to setwedge (see VARARGIN)

% Choose default command line output for setwedge
handles.output = hObject;

handles.sc = varargin{1};
handles.sd = varargin{2};

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes setwedge wait for user response (see UIRESUME)
% uiwait(handles.figure_wedge);


% --- Outputs from this function are returned to the command line.
function varargout = setwedge_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on selection change in enableset.
function enableset_Callback(hObject, eventdata, handles)
% hObject    handle to enableset (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns enableset contents as cell array
%        contents{get(hObject,'Value')} returns selected item from enableset


% --- Executes during object creation, after setting all properties.
function enableset_CreateFcn(hObject, eventdata, handles)
% hObject    handle to enableset (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in wedge_type.
function wedge_type_Callback(hObject, eventdata, handles)
% hObject    handle to wedge_type (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns wedge_type contents as cell array
%        contents{get(hObject,'Value')} returns selected item from wedge_type


% --- Executes during object creation, after setting all properties.
function wedge_type_CreateFcn(hObject, eventdata, handles)
% hObject    handle to wedge_type (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function wedgevel_Callback(hObject, eventdata, handles)
% hObject    handle to wedgevel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of wedgevel as text
%        str2double(get(hObject,'String')) returns contents of wedgevel as a double


% --- Executes during object creation, after setting all properties.
function wedgevel_CreateFcn(hObject, eventdata, handles)
% hObject    handle to wedgevel (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function wedgelangle_Callback(hObject, eventdata, handles)
% hObject    handle to wedgelangle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of wedgelangle as text
%        str2double(get(hObject,'String')) returns contents of wedgelangle as a double


% --- Executes during object creation, after setting all properties.
function wedgelangle_CreateFcn(hObject, eventdata, handles)
% hObject    handle to wedgelangle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function wedgeheight_Callback(hObject, eventdata, handles)
% hObject    handle to wedgeheight (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of wedgeheight as text
%        str2double(get(hObject,'String')) returns contents of wedgeheight as a double


% --- Executes during object creation, after setting all properties.
function wedgeheight_CreateFcn(hObject, eventdata, handles)
% hObject    handle to wedgeheight (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in okwedge.
function okwedge_Callback(hObject, eventdata, handles)
% hObject    handle to okwedge (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
if get(handles.enableset, 'Value') == 1
    str1 = 'enable';
else 
    str1 = 'disable';
end
if get(handles.wedge_type, 'Value') == 1
    str2 = 's';
else 
    str2 = 'l';
end
wedge_ini = ['set wedge', ' ', str1, ' ', str2, ' ', get(handles.wedgelangle, 'string'), ' ',... 
    get(handles.wedgevel, 'string'), ' ', get(handles.wedgeheight, 'string')]
SendCommandToServer(handles.sc, wedge_ini);
delete(handles.figure_wedge);
