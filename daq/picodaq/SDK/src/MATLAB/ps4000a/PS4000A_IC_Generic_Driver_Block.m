%PS4000A_IC_GENERIC_DRIVER_BLOCK Code for communicating with an instrument. 
%  
%   This is the machine generated representation of an instrument control 
%   session using a device object. The instrument control session comprises  
%   all the steps you are likely to take when communicating with your  
%   instrument. These steps are:
%       
%       1. Create a device object   
%       2. Connect to the instrument 
%       3. Configure properties 
%       4. Invoke functions 
%       5. Disconnect from the instrument 
%  
%   To run the instrument control session, type the name of the file,
%   PS4000A_IC_Generic_Driver_Block, at the MATLAB command prompt.
% 
%   The file, PS4000A_IC_GENERIC_DRIVER_BLOCK.M must be on your MATLAB PATH. For additional information
%   on setting your MATLAB PATH, type 'help addpath' at the MATLAB command
%   prompt.
%
%   Example:
%       PS4000A_IC_Generic_Driver_Block;
%
%   Description:
%       Demonstrates how to call functions in order to capture a block of
%       data from a PicoScope 4000 series oscilloscope using the underlying
%       'A' API.
%
%   See also ICDEVICE.
%
%   Copyright:  Pico Technology Limited 2014
%
%   Author: HSM

%   Creation time: 30-May-2014 15:31:24 

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

if(ps4000aDeviceObj.channelCount == PicoConstants.QUAD_SCOPE || ps4000aDeviceObj.channelCount == PicoConstants.OCTO_SCOPE)

    [status.setChC] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 2, 0, 1, 8, 0.0);
    [status.setChD] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 3, 0, 1, 8, 0.0);

end

if(ps4000aDeviceObj.channelCount == PicoConstants.OCTO_SCOPE)

    [status.setChE] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 4, 0, 1, 8, 0.0);
    [status.setChF] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 5, 0, 1, 8, 0.0);
    [status.setChG] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 6, 0, 1, 8, 0.0);
    [status.setChH] = invoke(ps4000aDeviceObj, 'ps4000aSetChannel', 7, 0, 1, 8, 0.0);

end

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

[status.SimpleTrigger] = invoke(triggerGroupObj, 'setSimpleTrigger', 0, 500, 2);

%% Get Timebase
% Driver default timebase index used - use ps4000aGetTimebase2 to query the
% driver as to suitability of using a particular timebase index then set 
% the 'timebase' property if required.

% timebase     : 79 (default)
% segment index: 0

[status.getTimebase, timeIntervalNanoSeconds, maxSamples] = invoke(ps4000aDeviceObj, 'ps4000aGetTimebase2', 79, 0);

%% Set Block Parameters and Capture Data
% Capture a block of data and retrieve data values for Channel A.

% Block data acquisition properties and functions are located in the 
% Instrument Driver's Block group.

blockGroupObj = get(ps4000aDeviceObj, 'Block');
blockGroupObj = blockGroupObj(1);

% Set pre-trigger and post-trigger samples as required
% The default of 0 pre-trigger and 1 million post-trigger samples is used
% in this example.

% set(ps4000aDeviceObj, 'numPreTriggerSamples', 0);
% set(ps4000aDeviceObj, 'numPostTriggerSamples', 2e6);

% Capture a block of data:
%
% segment index: 0 (The buffer memory is not segmented in this example)

[status.runBlock] = invoke(blockGroupObj, 'runBlock', 0);

% Retrieve data values:
%
% start index       : 0
% segment index     : 0
% downsampling ratio: 1
% downsampling mode : 0 (ps4000aEnuminfo.enPS4000ARatioMode.PS4000A_RATIO_MODE_NONE)

[numSamples, overflow, chA] = invoke(blockGroupObj, 'getBlockData', 0, 0, 1, 0);

% Stop the device
[status.stop] = invoke(ps4000aDeviceObj, 'ps4000aStop');

%% Process Data

% Plot data values.

figure;

% Calculate time (nanoseconds) and convert to milliseconds
% Use timeIntervalNanoSeconds output from ps4000aGetTimebase2 
% or calculate from the main Programmer's Guide.

timeNs = double(timeIntervalNanoSeconds) * double([0:numSamples - 1]);
timeMs = timeNs / 1e6;

% Channel A
plot(timeMs, chA, 'b');
ylim([-2000 2000]);

title('Block Data Acquisition');
xlabel('Time (ms)');
ylabel('Voltage (mV)');

grid on;
legend('Channel A');

%% Disconnect Device

% Disconnect device object from hardware.
disconnect(ps4000aDeviceObj);
