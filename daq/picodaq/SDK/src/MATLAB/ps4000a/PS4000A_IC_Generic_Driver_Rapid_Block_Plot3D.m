%PS4000A_IC_GENERIC_DRIVER_RAPID_BLOCK_PLOT3D Code for communicating with an instrument. 
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
%   PS4000A_IC_Generic_Driver_Rapid_Block_Plot3D, at the MATLAB command prompt.
% 
%   The file, PS4000A_IC_GENERIC_DRIVER_RAPID_BLOCK_PLOT3D.M must be on your MATLAB PATH. For additional information
%   on setting your MATLAB PATH, type 'help addpath' at the MATLAB command
%   prompt.
%
%   Example:
%       PS4000A_IC_Generic_Driver_Rapid_Block_Plot3D;
%   
%   Description:
%   Demonstrates how to call functions in order to capture rapid block
%   data from a PicoScope 4824 oscilloscope.
%   
%   See also ICDEVICE.
%
%   Copyright:  Pico Technology Limited 2014
%
%   Author: HSM

%   Creation time: 09-Jun-2014 13:34:14 
%   Updated: 14-11-2014 - modified to support PicoScope 4225 and 4425

%% Load Configuration Information
PS4000aConfig;

%% Device Connection

% Create a device object. 
% The serial number can be specified as a second input parameter.
ps4000aDeviceObj = icdevice('picotech_ps4000a_generic.mdd');

% Connect device object to hardware.
connect(ps4000aDeviceObj);

%% Set Channels
% Default driver settings applied to channels are listed below - 
% use ps4000aSetChannel to turn channels on or off and set voltage ranges, 
% coupling, as well as analogue offset.

% In this example, data is only collected on Channel A so default settings
% are used and channels B to H are switched off.

% Channels       : 1 - 7 (ps4000aEnuminfo.enPS4000AChannel.PS4000A_CHANNEL_B - PS4000A_CHANNEL_H)
% Enabled        : 0
% Type           : 1 (ps4000aEnuminfo.enPS4000ACoupling.PS4000A_DC)
% Range          : 8 (ps4000aEnuminfo.enPS4000ARange.PS4000A_5V)
% Analogue Offset: 0.0

% Execute device object function(s).
[status.setChB] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 1, 0, 1, 8, 0.0);

if(ps4000aDeviceObj.channelCount == PicoConstants.QUAD_SCOPE || ...
        ps4000aDeviceObj.channelCount == PicoConstants.OCTO_SCOPE)
    
    [status.setChC] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 2, 0, 1, 8, 0.0);
    [status.setChD] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 3, 0, 1, 8, 0.0);
    
end

if(ps4000aDeviceObj.channelCount == PicoConstants.OCTO_SCOPE)
    
    [status.setChE] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 4, 0, 1, 8, 0.0);
    [status.setChF] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 5, 0, 1, 8, 0.0);
    [status.setChG] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 6, 0, 1, 8, 0.0);
    [status.setChH] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 7, 0, 1, 8, 0.0);

end

%% Set Memory Segments
% Configure number of memory segments, ideally a power of 2, query
% ps4000aGetMaxSegments to find the maximum number of segments for the
% device.

% nSegments : 128

[status.memorySegments, nMaxSamples] = invoke(ps4000aDeviceObj, 'ps4000aMemorySegments', 128);

%% Query Timebase
% Use ps4000aGetTimebase2 to query the driver as to suitability of using a 
% particular timebase index then set the 'timebase' property if required.

% timebase      : 49 (625ns)
% segment index : 0

[status.getTimebase2, timeIntNs, maxSamples] = invoke(ps4000aDeviceObj, 'ps4000aGetTimebase2', 49, 0);

% Configure property value(s).
set(ps4000aDeviceObj, 'timebase', 49);

%% Set Simple Trigger 
% Set a trigger on Channel A, default values for delay and auto timeout are
% used.

% Trigger properties and functions are located in the Instrument
% Driver's Trigger group.

triggerGroupObj = get(ps4000aDeviceObj, 'Trigger');
triggerGroupObj = triggerGroupObj(1);

% Channel     : 0 (ps4000aEnuminfo.enPS4000AChannel.PS4000A_CHANNEL_A)
% Threshold   : 500 (mV)
% Direction   : 2 (ps4000aEnuminfo.enPS4000AThresholdDirection.PS4000A_RISING)

[status.setSimpleTrigger] = invoke(triggerGroupObj, 'setSimpleTrigger', 0, 500, 2);

%% Setup Rapid Block Parameters and Capture Data

% Rapid Block specific properties and functions are located in the Instrument
% Driver's Rapidblock group.

rapidBlockGroupObj = get(ps4000aDeviceObj, 'Rapidblock');
rapidBlockGroupObj = rapidBlockGroupObj(1);

% Set the number of waveforms to captures

% nCaptures : 16

[status.setNoOfCaptures] = invoke(rapidBlockGroupObj, 'ps4000aSetNoOfCaptures', 16);

% Set number of samples to collect pre- and post-trigger. Ensure that the
% total does not exceeed nMaxSamples above.

set(ps4000aDeviceObj, 'numPreTriggerSamples', 2500);
set(ps4000aDeviceObj, 'numPostTriggerSamples', 7500);

% Block specific properties and functions are located in the Instrument
% Driver's Block group.

blockGroupObj = get(ps4000aDeviceObj, 'Block');
blockGroupObj = blockGroupObj(1);

% Capture the blocks of data

% segmentIndex : 0 

[status.runBlock, timeIndisposedMs] = invoke(blockGroupObj, 'runBlock', 0);

% Retrieve Rapid Block Data

% numCaptures : 16
% ratio       : 1
% ratioMode   : 0 (ps4000aEnuminfo.enPS4000ARatioMode.PS4000A_RATIO_MODE_NONE)

[numSamples, overflow, chA] = invoke(rapidBlockGroupObj, 'getRapidBlockData', 16, 1, 0);

% Stop the Device
[status.stop] = invoke(ps4000aDeviceObj, 'ps4000aStop');

%% Process data

% Plot data values in 3D showing history.

% Calculate time (nanoseconds) and convert to milliseconds
% Use timeIntervalNanoSeconds output from ps4000aGetTimebase2 or calculate 
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
disconnect(ps4000aDeviceObj);
