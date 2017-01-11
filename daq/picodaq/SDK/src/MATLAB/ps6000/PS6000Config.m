%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS6000Config.m
%
% Copyright:   Pico Technology Limited 2014
%
% Author:      KPV
%
% Description:
%   
%   Contains configuration data for setting parameters for a PicoScope 6000
%   Series Oscilloscope device using the 'A' API.
%
%   Run this script in the MATLAB environment prior to connecting to the 
%   device.
%
%   This file can be edited to suit the application requirements.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% SETUP PATH
% Set paths according to the operating system and architecture.
% For Windows and Mac-based versions of MATLAB, it is recommended to copy 
% the driver files into the the sub-folder corresponding to the 
% architecture e.g. win32 for 32-bit versions of MATLAB.
% For Linux versions of MATLAB, please follow the instructions to install
% the libps6000 and libpswrappers packages from http://www.picotech.com/linux.html

% Identify architecture

archStr = computer('arch');

try

    addpath(strcat('./', archStr, '/'));
    
catch err
    
    error('Operating system not supported please contact support@picotech.com');
    
end

% Set path to the Functions folder, PicoStatus.m and PicoConstants.m files
% Path to drivers on Linux operating systems will also be set.

if(isunix)
    
    addpath('../Functions');        % Common functions
	    
    if(~ismac) % Linux
        
        addpath('/opt/picoscope/lib/'); %Edit to specify location of .so files or place .so files in same directory
        
    end
    
else
    
    % Microsoft Windows operating system
    
    addpath(''); % Edit to specify location of dll files or place dlls in the same directory
	addpath('..\Functions');    % Common functions
    
end

addpath('..');  % PicoStatus.m & PicoConstants.m

%% LOAD ENUMS AND STRUCTURES

% Load in enumerations and structure information - DO NOT EDIT THIS SECTION
[ps6000Methodinfo, ps6000Structs, ps6000Enuminfo, ps6000ThunkLibName] = ps6000MFile; 

%% PICOSCOPE SETTINGS
% Define Settings for PicoScope 6000 series - if using tmtool, these can be
% accessed from the MATLAB environment by calling:
% evalin('base', 'variable_name');

