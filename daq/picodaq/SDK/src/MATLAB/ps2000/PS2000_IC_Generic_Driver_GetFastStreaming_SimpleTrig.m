%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS2000_IC_Generic_Driver_GetFastStreaming_SimpleTrig.m
%
% Copyright:   Pico Technology Limited 2013 - 2014
%
% Author:      HSM
%
% Description:
%   This is a MATLAB script that demonstrates how to use the
%   PicoScope 2000 series Instrument Control Toobox driver to collect
%   a set of samples using Fast Streaming collection, with data returned 
%   upon completion of the collection.
%
% Example:
%    Collect a set of samples captured using fast streaming after setting 
%    the parameters for the channels and simple trigger.
%
%	To run this application:
%		
%   Click 'Run' in the MATLAB Editor or type 
%   PS2000_IC_Generic_Driver_GetFastStreaming_SimpleTrig at the MATLAB
%   command prompt.
%
% Update 06-Nov-2014 - modified after groups added to Instrument Driver.
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

% Streaming specific properties and functions are located in the Instrument
% Driver's Streaming group.

streamingGroupObj = get(ps2000DeviceObj, 'Streaming');
streamingGroupObj = streamingGroupObj(1);

% Trigger specific properties and functions are located in the Instrument
% Driver's Trigger group.

triggerGroupObj = get(ps2000DeviceObj, 'Trigger');
triggerGroupObj = triggerGroupObj(1);

%% CONFIGURE DEVICE

% Execute device object function(s).

% Channel     : 0 (PS2000_CHANNEL_A)
% Enabled     : 1 (True)
% DC          : 1 (DC Coupling)
% Range       : 6 (PS2000_1V)
[status.setChA] = invoke(ps2000DeviceObj, 'ps2000SetChannel', 0, 1, 1, 6);   

% Channel     : 1 (PS2000_CHANNEL_B)
% Enabled     : 1 (True)
% DC          : 1 (DC Coupling)
% Range       : 7 (PS2000_2V)
[status.setChB] = invoke(ps2000DeviceObj, 'ps2000SetChannel', 1, 1, 1, 7);    

% Set sampling interval - default sampling interval of 1us is used
% The driver will calculate the nearest sampling interval that is
% short if an exact match cannot be found.

% To set a new sampling interval e.g. 10 microseconds
% First find the closest sampling interval to 10 microsecond
[samplingIntervalMs, maxBlockSamples] = invoke(blockGroupObj, 'setBlockIntervalMs', 10e-3);

% Set the actual sampling interval
set(streamingGroupObj, 'streamingIntervalMs', samplingIntervalMs);

% Collect 1 million samples from the driver
set(ps2000DeviceObj, 'numberOfSamples', 1e6);

% Set the size of the overview buffer used for streaming capture
set(streamingGroupObj, 'overviewBufferSize', 50000);

% Execute device object function(s).
% Parameters taken from config file loaded into workspace
% Delay is set to 0
[simpleTriggerStatus] = invoke(triggerGroupObj, 'setSimpleTrigger', ...
    simpleTrigger.source, simpleTrigger.threshold, simpleTrigger.direction);

%% STREAMING DATA COLLECTION

disp('Collecting streaming data...')

startTime = 0.0;
[numDataValues, startTime, bufferChA, bufferChB, overflow, triggerAt, trigger] = invoke(streamingGroupObj, 'getFastStreamingData', startTime);

disp('Data collection complete.');

%% PROCESS DATA

disp('Processing data for plot...')

% Specify time axis in milliseconds
timeLabel = 'Time (ms)';

time = [0:1:numDataValues - 1] * get(streamingGroupObj, 'streamingIntervalMs');

% Plot data

figure;
plot(time, bufferChA, 'b-', time, bufferChB, 'r-');
grid on;
title('Plot of Voltage vs. Time');
xlabel(timeLabel);
ylabel('Voltage (mv)');
legend('Channel A', 'Channel B');
grid on;

%% DISCONNECT

% Disconnect device object from hardware.
disconnect(ps2000DeviceObj);