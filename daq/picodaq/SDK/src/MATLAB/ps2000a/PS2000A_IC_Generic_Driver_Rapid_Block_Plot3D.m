%TEST_PS2000A_IC_RAPID_BLOCK_PLOT3D Code for communicating with an instrument. 

clear;
clc;

%% Load Configuration Information
PS2000aConfig;

%% Device Connection

% Create a device object. 
% The serial number can be specified as a second input parameter.
ps2000aDeviceObj = icdevice('picotech_ps2000a_generic.mdd');

% Connect device object to hardware.
connect(ps2000aDeviceObj);

%% Set Channels
% Default driver settings applied to channels are listed below - 
% use ps2000aSetChannel to turn channels on or off and set voltage ranges, 
% coupling, as well as analogue offset.

% In this example, data is only collected on Channel A so default settings
% are used and channels B (and C to D for 4-channel devices) are switched off.

% Channels       : 1 - 3 (ps2000aEnuminfo.enPS2000AChannel.PS2000A_CHANNEL_B - PS2000A_CHANNEL_D)
% Enabled        : 0
% Type           : 1 (ps2000aEnuminfo.enPS2000ACoupling.PS2000A_DC)
% Range          : 8 (ps2000aEnuminfo.enPS2000ARange.PS2000A_5V)
% Analogue Offset: 0.0

% Execute device object function(s).
[status.setChB] = invoke(ps2000aDeviceObj, 'ps2000aSetChannel', 1, 0, 1, 8, 0.0);

if(ps2000aDeviceObj.channelCount == PicoConstants.QUAD_SCOPE)
    
    [status.setChC] = invoke(ps2000aDeviceObj, 'ps2000aSetChannel', 2, 0, 1, 8, 0.0);
    [status.setChD] = invoke(ps2000aDeviceObj, 'ps2000aSetChannel', 3, 0, 1, 8, 0.0);
    
end

if(ps2000aDeviceObj.digitalPortCount ~= 0)
    
    digitalObj = get(ps2000aDeviceObj,'Digital');
    invoke(digitalObj, 'ps2000aSetDigitalPort', evalin('base', 'ps2000aEnuminfo.enPS2000DigitalPort.PS2000A_DIGITAL_PORT0'), 0, 2.5);
    invoke(digitalObj, 'ps2000aSetDigitalPort', evalin('base', 'ps2000aEnuminfo.enPS2000DigitalPort.PS2000A_DIGITAL_PORT1'), 0, 0);
    
end

%% Set Memory Segments
% Configure number of memory segments, ideally a power of 2, query
% ps2000aGetMaxSegments to find the maximum number of segments for the
% device.

% nSegments : 128

[status.memorySegments, nMaxSamples] = invoke(ps2000aDeviceObj, 'ps2000aMemorySegments', 16);

%% Query Timebase
% Use ps2000aGetTimebase2 to query the driver as to suitability of using a 
% particular timebase index then set the 'timebase' property if required.

% timebase      : 42
% segment index : 0

set(ps2000aDeviceObj, 'numPostTriggerSamples', 1024);

[status.getTimebase2, timeIntNs, maxSamples] = invoke(ps2000aDeviceObj, 'ps2000aGetTimebase2', 42, 0);

% Configure property value(s).
set(ps2000aDeviceObj, 'timebase', 42);

%% Set Simple Trigger 
% Set a trigger on Channel A, default values for delay and auto timeout are
% used.

% Trigger properties and functions are located in the Instrument
% Driver's Trigger group.

triggerGroupObj = get(ps2000aDeviceObj, 'Trigger');
triggerGroupObj = triggerGroupObj(1);

% Channel     : 0 (ps2000aEnuminfo.enPS2000AChannel.PS2000A_CHANNEL_A)
% Threshold   : 500 (mV)
% Direction   : 2 (ps2000aEnuminfo.enPS2000AThresholdDirection.PS2000A_RISING)

[status.setSimpleTrigger] = invoke(triggerGroupObj, 'setSimpleTrigger', 0, 500, 2);

%% Setup Rapid Block Parameters and Capture Data

% Rapid Block specific properties and functions are located in the Instrument
% Driver's Rapidblock group.

rapidBlockGroupObj = get(ps2000aDeviceObj, 'Rapidblock');
rapidBlockGroupObj = rapidBlockGroupObj(1);

% Set the number of waveforms to captures

% nCaptures : 16

[status.setNoOfCaptures] = invoke(rapidBlockGroupObj, 'ps2000aSetNoOfCaptures', 16);

% Block specific properties and functions are located in the Instrument
% Driver's Block group.

blockGroupObj = get(ps2000aDeviceObj, 'Block');
blockGroupObj = blockGroupObj(1);

% Capture the blocks of data

% segmentIndex : 0 

[status.runBlock, timeIndisposedMs] = invoke(blockGroupObj, 'runBlock', 0);

% Retrieve Rapid Block Data

% numCaptures : 16
% ratio       : 1
% ratioMode   : 0 (ps2000aEnuminfo.enPS2000ARatioMode.PS2000A_RATIO_MODE_NONE)

[numSamples, overflow, chA] = invoke(rapidBlockGroupObj, 'getRapidBlockData', 16, 1, 0);

% Stop the Device
[status.stop] = invoke(ps2000aDeviceObj, 'ps2000aStop');

%% Process data

% Plot data values in 3D showing history.

% Calculate time (nanoseconds) and convert to milliseconds
% Use timeIntervalNanoSeconds output from ps2000aGetTimebase2 or calculate 
% from Programmer's Guide.

timeNs = double(timeIntNs) * double([0:numSamples - 1]);

% Channel A
figure1 = figure;
axes1 = axes('Parent', figure1);
view(axes1,[-15 24]);
grid(axes1,'on');
hold(axes1,'all');

for i = 1:16
    
    plot3(timeNs, i * (ones(numSamples, 1)), chA(:, i));
    
end

title('Rapid Block Data Acquisition - Channel A');
xlabel('Time (ns)');
ylabel('Capture');
zlabel('Voltage (mV)');

hold off;

%% Disconnect Device

% Disconnect device object from hardware.
disconnect(ps2000aDeviceObj);
