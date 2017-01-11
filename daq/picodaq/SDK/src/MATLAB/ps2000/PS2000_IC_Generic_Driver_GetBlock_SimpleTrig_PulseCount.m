%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS2000_IC_Generic_Driver_GetBlock_SimpleTrig_PulseCount.m
%
% Copyright:   Pico Technology Limited 2013 - 2014
%
% Author:      HSM
%
% Description:
%   This is a MATLAB script that demonstrates how to use the
%   PicoScope 2000 series Instrument Control Toobox driver to collect
%   a block of samples and count the number of pulses captured.
%
%   A part of this script was generated using the Test and Measurement
%   Tool.
%
% Example:
%    Collect a block of samples after setting the parameters for the
%    channels, sampling interval and simple trigger. Counts pulses after
%    capture.
%
%	To run this application:
%		
%   Click 'Run' in the MATLAB Editor or type the name of the file,
%   PS2000_IC_Generic_Driver_GetBlock_SimpleTrig, at the MATLAB 
%   command prompt.
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

%% CONFIGURE DEVICE

% Execute device object function(s).

% Channel     : 0 (PS2000_CHANNEL_A)
% Enabled     : 1 (True)
% DC          : 1 (DC Coupling)
% Range       : 8 (PS2000_5V)
[status.setChA] = invoke(ps2000DeviceObj, 'ps2000SetChannel', 0, 1, 1, 8);    % 5V range

% Channel     : 0 (PS2000_CHANNEL_A)
% Enabled     : 1 (True)
% DC          : 1 (DC Coupling)
% Range       : 7 (PS2000_2V)
[status.setChB] = invoke(ps2000DeviceObj, 'ps2000SetChannel', 1, 0, 1, 7);    % 2V range

[samplingIntervalUs, maxBlockSamples] = invoke(blockGroupObj, 'setBlockIntervalUs', 100);

% Query property value(s).
timebaseIndex = get(blockGroupObj, 'timebase'); % Confirm the timebase index selected

% Configure property value(s).
set(ps2000DeviceObj, 'numberOfSamples', 4096);

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
plot(bufferTimes, bufferChA, 'b-');
title('Plot of Voltage vs. Time');
xlabel(timeLabel);
ylabel('Voltage (mv)');
legend('Channel A');
grid on;

% Count number of rising and falling edges
% Based on Counting Complex Events Using Analog Input Data - MATLAB & Simulink Example - MathWorks United Kingdom
% <http://www.mathworks.co.uk/help/daq/examples/counting-complex-events-using-analog-input-data.html Counting Complex Events Using Analog Input Data - MATLAB & Simulink Example - MathWorks United Kingdom>

disp('Counting number of rising and falling edges...')

% Set the threshold to 0 V.
threshold = 2.5;

% Create the offset data.  Need to append a NaN to the final sample since
% both vectors need to have the same length.
offsetData = [bufferChA(2:end); NaN];

% Find the rising edge.
risingEdge = find(bufferChA < threshold & offsetData > threshold);

% Find the falling edge.
fallingEdge = find(bufferChA > threshold & offsetData < threshold);

% Show the rising edges with red x's.
hold on

if(~isempty(risingEdge))
    
    plot(bufferTimes(risingEdge), threshold, 'rx');
    
end

% Show the falling edges with green o's.
if(~isempty(fallingEdge))

    plot(bufferTimes(fallingEdge), threshold, 'go');
    
end
hold off

fprintf(' Num. rising edges: %d\n', length(risingEdge));
fprintf('Num. falling edges: %d\n\n', length(fallingEdge));

%% DISCONNECT

% Disconnect device object from hardware.
disconnect(ps2000DeviceObj);

