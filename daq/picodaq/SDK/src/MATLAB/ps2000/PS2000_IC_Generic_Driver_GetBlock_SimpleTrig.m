%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS2000_IC_Generic_Driver_GetBlock_SimpleTrig.m
%
% Copyright:   Pico Technology Limited 2013 - 2014
%
% Author:      HSM
%
% Description:
%   This is a MATLAB script that demonstrates how to use the
%   PicoScope 2000 series Instrument Control Toobox driver to collect
%   a block of samples.
%
%   A part of this script was generated using the Test and Measurement
%   Tool.
%
% Example:
%    Collect a block of samples after setting the parameters for the
%    channels, sampling interval and simple trigger.
%
%	To run this script:
%		
%   Click 'Run' in the MATLAB Editor or type the name of the file,
%   PS2000_IC_Generic_Driver_GetBlock_SimpleTrig, at the MATLAB command 
%   prompt.
%
%   The file, PS2000_IC_GENERIC_DRIVER_GETBLOCK_SIMPLETRIG.M must be on 
%   your MATLAB PATH. For additional information on setting your MATLAB 
%   PATH, type 'help addpath' at the MATLAB command prompt.
%
%   See also ICDEVICE.
%
%   Creation time: 04-Jan-2013 17:01:11 
%   Update 01-Nov-2013 for new PicoScope 2204A & 2205A
%   Update 06-Nov-2013 - modified after groups added to Instrument Driver. 
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

% Trigger specific properties and functions are located in the Instrument
% Driver's Trigger group.

triggerGroupObj = get(ps2000DeviceObj, 'Trigger');
triggerGroupObj = triggerGroupObj(1);

% Block specific properties and functions are located in the Instrument
% Driver's Block group.

blockGroupObj = get(ps2000DeviceObj, 'Block');
blockGroupObj = blockGroupObj(1);

%% CONFIGURE DEVICE

% Execute device object function(s).

% Channel     : 0 (PS2000_CHANNEL_A)
% Enabled     : 1 (True)
% DC          : 1 (DC Coupling)
% Range       : 6 (PS2000_1V)
[status.setChA] = invoke(ps2000DeviceObj, 'ps2000SetChannel', 0, 1, 1, 6);    % 1V range

% Channel     : 0 (PS2000_CHANNEL_A)
% Enabled     : 1 (True)
% DC          : 1 (DC Coupling)
% Range       : 7 (PS2000_2V)
[status.setChB] = invoke(ps2000DeviceObj, 'ps2000SetChannel', 1, 1, 1, 7);    % 2V range

% Set sampling interval
[samplingIntervalUs, maxBlockSamples] = invoke(blockGroupObj, 'setBlockIntervalUs', 100);

% Query property value(s).
timebaseIndex = get(blockGroupObj, 'timebase'); % Confirm the timebase index selected

% Configure property value(s).
set(ps2000DeviceObj, 'numberOfSamples', 2048);

% Trigger specific properties and functions are located in the Instrument
% Driver's Trigger group.

triggerGroupObj = get(ps2000DeviceObj, 'Trigger');
triggerGroupObj = triggerGroupObj(1);

% Set the delay to 50% (samples will be shown either side of the trigger
% point.
set(triggerGroupObj, 'delay', -50.0);

% Execute device object function(s).
% Parameters taken from config file loaded into workspace

[simpleTriggerStatus] = invoke(triggerGroupObj, 'setSimpleTrigger', ...
    simpleTrigger.source, simpleTrigger.threshold, simpleTrigger.direction);

%% DATA COLLECTION

disp('Collecting block of data...');

[bufferTimes, bufferChA, bufferChB, numDataValues, timeIndisposedMs] = invoke(blockGroupObj, 'getBlockData');

disp('Data collection complete.');

%% STOP THE DEVICE
% Additional blocks can be captured prior to stopping the device.

stopStatus = invoke(ps2000DeviceObj, 'ps2000Stop');

%% PROCESS DATA

disp('Processing data for plot...')

% Find the time units used by the driver
timesUnits = adcunits(get(blockGroupObj, 'timeUnits'));

% Append to string
timeLabel = strcat('Time (', timesUnits, ')');

% Plot

figure;
plot(bufferTimes, bufferChA, 'b-', bufferTimes, bufferChB, 'r-');
title('Plot of Voltage vs. Time');
xlabel(timeLabel);
ylabel('Voltage (mv)');
legend('Channel A', 'Channel B');
grid on;

%% DISCONNECT

% Disconnect device object from hardware.
disconnect(ps2000DeviceObj);

