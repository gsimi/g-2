%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS4000_IC_Generic_Driver_Rapid_Block_Plot3D
%
% Copyright:   Pico Technology Limited 2014
%
% Author:      KPV
%
% Description:
%   This is a MATLAB script that demonstrates how to use the
%   PicoScope 4000 Series Instrument Control Toobox driver to 
%   collect data in rapid block mode with a trigger
%
%	To run this application:
%		Ensure that the following files/folders are located either in the 
%       same directory or define the path in the PS4000Config.m file:
%       
%       - picotech_ps4000_generic.mdd
%       - PS4000Constants
%       - ps4000.dll & ps4000Wrap.dll 
%       - ps4000MFile.m & ps4000WrapMFile.m
%       - PicoConstants.m
%       - PicoStatus.m
%       - Functions
%
%   Device used to generated example: PicoScope 4423 & 4262
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Load Configuration Information
PS4000Config;

%% Device Connection

% Create a device object. 
% The serial number can be specified as a second input parameter.
ps4000DeviceObj = icdevice('picotech_ps4000_generic.mdd');

% Connect device object to hardware.
connect(ps4000DeviceObj);

%% Set Channels
% Default driver settings applied to channels are listed below - 
% use ps4000SetChannel to turn channels on or off and set voltage ranges, 
% coupling, as well as analogue offset.

% In this example, data is only collected on Channel A so default settings
% are used and channels B to D are switched off.

% Channels       : 1 - 3 (ps4000Enuminfo.enPS4000Channel.PS4000_CHANNEL_B - PS4000_CHANNEL_D)
% Enabled        : 0
% Type           : 1 (DC)
% Range          : 8 (ps4000Enuminfo.enPS4000Range.PS4000_5V)

% Execute device object function(s).
[status.setChB] = invoke(ps4000DeviceObj, 'ps4000SetChannel', 1, 0, 1, 8, 0.0,0);

if (ps4000DeviceObj.channelCount == 4)

	[status.setChC] = invoke(ps4000DeviceObj, 'ps4000SetChannel', 2, 0, 1, 8, 0.0,0);
	[status.setChD] = invoke(ps4000DeviceObj, 'ps4000SetChannel', 3, 0, 1, 8, 0.0,0);
	
end

%% Set Memory Segments
% Configure number of memory segments, ideally a power of 2, query
% ps4000GetMaxSegments to find the maximum number of segments for the
% device.

% nSegments : 10

[status.memorySegments, nMaxSamples] = invoke(ps4000DeviceObj, 'ps4000MemorySegments', 10);

%% Query Timebase
% Use ps4000GetTimebase2 to query the driver as to suitability of using a 
% particular timebase index then set the 'timebase' property if required.

% timebase      : 8
% segment index : 0

[status.getTimebase2, timeIntNs, maxSamples] = invoke(ps4000DeviceObj, 'ps4000GetTimebase2', 8, 0);

% Configure property value(s).
set(ps4000DeviceObj, 'timebase', 8);

%% Set Simple Trigger 
% Set a trigger on Channel A, default values for delay and auto timeout are
% used.

% Trigger properties and functions are located in the Instrument
% Driver's Trigger group.

triggerGroupObj = get(ps4000DeviceObj, 'Trigger');
triggerGroupObj = triggerGroupObj(1);

% Channel     : 0 (ps4000Enuminfo.enPS4000Channel.PS4000_CHANNEL_A)
% Threshold   : 500 (mV)
% Direction   : 2 (ps4000Enuminfo.enPS4000ThresholdDirection.PS4000_RISING)

[status.setSimpleTrigger] = invoke(triggerGroupObj, 'setSimpleTrigger', 0, 500, 2);

%% Setup Rapid Block Parameters and Capture Data

% Rapid Block specific properties and functions are located in the Instrument
% Driver's Rapidblock group.

rapidBlockGroupObj = get(ps4000DeviceObj, 'Rapidblock');
rapidBlockGroupObj = rapidBlockGroupObj(1);

% Set the number of waveforms to captures

% nCaptures : 16

[status.setNoOfCaptures] = invoke(rapidBlockGroupObj, 'ps4000SetNoOfCaptures', 10);

% Set number of samples to collect pre- and post-trigger. Ensure that the
% total does not exceeed nMaxSamples above.

set(ps4000DeviceObj, 'numPreTriggerSamples', 2500);
set(ps4000DeviceObj, 'numPostTriggerSamples', 7500);

% Block specific properties and functions are located in the Instrument
% Driver's Block group.

blockGroupObj = get(ps4000DeviceObj, 'Block');
blockGroupObj = blockGroupObj(1);

% Capture the blocks of data

% segmentIndex : 0 

[status.runBlock, timeIndisposedMs] = invoke(blockGroupObj, 'runBlock', 0);

% Retrieve Rapid Block Data

% numCaptures : 16
% ratio       : 1
% ratioMode   : 0 (ps4000Enuminfo.enPS4000RatioMode.PS4000_RATIO_MODE_NONE)

[numSamples, overflow, chA] = invoke(rapidBlockGroupObj, 'getRapidBlockData', 10, 1, 0);

% Stop the Device
[status.stop] = invoke(ps4000DeviceObj, 'ps4000Stop');

%% Process data

% Plot data values in 3D showing history.

% Calculate time (nanoseconds) and convert to milliseconds
% Use timeIntervalNanoSeconds output from ps4000GetTimebase2 or calculate 
% from Programmer's Guide.

timeNs = double(timeIntNs) * double([0:numSamples - 1]);

% Channel A
figure1 = figure;
axes1 = axes('Parent', figure1);
view(axes1,[-15 24]);
grid(axes1,'on');
hold(axes1,'all');

for i = 1:10
    
    plot3(timeNs, i * (ones(numSamples, 1)), chA(:, i));
    
end

title('Rapid Block Data Acquisition - Channel A');
xlabel('Time (ns)');
ylabel('Capture');
zlabel('Voltage (mV)');

hold off;

%% Disconnect Device

% Disconnect device object from hardware.
disconnect(ps4000DeviceObj);
