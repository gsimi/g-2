%PS2000_IC_GENERIC_DRIVER_GETBLOCK Code for communicating with an instrument. 
%  
%   This is a modified version of the machine generated representation of 
%   an instrument control session using a device object. The instrument 
%   control session comprises all the steps you are likely to take when 
%   communicating with your instrument. These steps are:
%       
%       1. Create a device object   
%       2. Connect to the instrument 
%       3. Configure properties 
%       4. Invoke functions 
%       5. Disconnect from the instrument 
%  
%   To run the instrument control session, type the name of the file,
%   PS2000_IC_Generic_Driver_GetBlock, at the MATLAB command prompt.
% 
%   The file, PS2000_IC_GENERIC_DRIVER_GETBLOCK.M must be on your MATLAB 
%   PATH. For additional information on setting your MATLAB PATH, type 
%   'help addpath' at the MATLAB command prompt.
%
%   Example:
%       PS2000_IC_Generic_Driver_GetBlock;
%
%   Description:
%       Demonstrates how to call functions in order to collect a block of 
%       samples.
%
%   See also ICDEVICE.
%
%   Copyright:   Pico Technology Limited 2013 - 2014
%
%   Author:      HSM
%
%   Creation time: 04-Jan-2013 15:28:07 
%
%   History :
%
%   08-Nov-2013: Modification to script
%   30-Oct-2014: Added in use of groups
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CLEAR WORKSPACE

clear all;
clc;

%% LOAD CONFIGURATION INFORMATION

PS2000Config;

%% DEVICE CONNECTION

% Create a device object. 
ps2000DeviceObj = icdevice('picotech_ps2000_generic.mdd');

% Connect device object to hardware.
connect(ps2000DeviceObj);

%% OBTAIN DEVICE GROUPS
% Obtain references to device groups to access their respective properties
% and functions.

% Block specific properties and functions are located in the Instrument
% Driver's Block group.

blockGroupObj = get(ps2000DeviceObj, 'Block');
blockGroupObj = blockGroupObj(1);

%% DEVICE CONFIGURATION
% Default channel setup, sampling interval and number of samples for the 
% device are used.

%% DATA COLLECTION
% Data for channels is returned in milliVolts
disp('Collecting block of data...');

% Execute device object function(s).
[bufferTimes, bufferChA, bufferChB, numDataValues, timeIndisposedMs] = invoke(blockGroupObj, 'getBlockData');

disp('Data collection complete.');

%% STOP THE DEVICE
% Additional blocks can be captured prior to stopping the device.

stopStatus = invoke(ps2000DeviceObj, 'ps2000Stop');

%% PROCESS DATA

disp('Plotting data...')

% Find the time units used by the driver
timesUnits = adcunits(get(blockGroupObj, 'timeUnits'));

% Append to string
timeLabel = strcat('Time (', timesUnits, ')');

% Plot

figure;
plot(bufferTimes, bufferChA, '-b', bufferTimes, bufferChB, '-r');
title('Plot of Voltage vs. Time');
xlabel(timeLabel);
ylabel('Voltage (mv)');
legend('Channel A', 'Channel B');
grid on;

%% DISCONNECT

% Disconnect device object from hardware.
disconnect(ps2000DeviceObj);
