%PS4000A_IC_GENERIC_DRIVER_SIG_GEN Code for communicating with an instrument. 
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
%   PS4000A_IC_Generic_Driver_Sig_Gen, at the MATLAB command prompt.
% 
%   The file, PS4000A_IC_GENERIC_DRIVER_SIG_GEN.M must be on your MATLAB PATH. For additional information
%   on setting your MATLAB PATH, type 'help addpath' at the MATLAB command
%   prompt.
%
%   Example:
%       PS4000A_IC_Generic_Driver_Sig_Gen;
%
%   Description:
%       Demonstrates how to call functions to operate the
%       Function/Arbitrary Waveform Generator. Run this script one section
%       at a time.
%
%   See also ICDEVICE.
%
%   Copyright:  Pico Technology Limited 2014
%
%   Author: HSM

%   Creation time: 01-May-2014 14:14:02 

%% Load Configuration Information

PS4000aConfig;

%% Device Connection

% Create a device object. 
ps4000aDeviceObj = icdevice('picotech_ps4000a_generic.mdd');

% Connect device object to hardware.
connect(ps4000aDeviceObj);

%% Obtain Signal Generator Group Object
% Signal Generator properties and functions are located in the Instrument
% Driver's signalGenerator group.

sigGenGroupObj = get(ps4000aDeviceObj, 'Signalgenerator');
sigGenGroupObj = sigGenGroupObj(1);

%% Function Generator - Simple
% Output a Sine wave, 2000mVpp, 0mV offset, 1000Hz (uses preset values for 
% offset, peak to peak voltage and frequency)

[status.setSigGenBuiltInSimple] = invoke(sigGenGroupObj, 'setSigGenBuiltInSimple', 0);

%% Function Generator - Sweep Frequency

% Configure property value(s).
set(ps4000aDeviceObj.Signalgenerator(1), 'startFrequency', 50.0);
set(ps4000aDeviceObj.Signalgenerator(1), 'stopFrequency', 500.0);
set(ps4000aDeviceObj.Signalgenerator(1), 'offsetVoltage', 500.0);
set(ps4000aDeviceObj.Signalgenerator(1), 'peakToPeakVoltage', 2400.0);

% Execute device object function(s).

% Wavetype       : 1 (PS4000A_SQUARE) 
% Increment      : 50.0 (Hz)
% Dwell Time     : 1 (s)
% Sweep Type     : 1 (PS4000A_DOWN)
% Operation      : 0 (PS4000A_ES_OFF)
% Shots          : 0 
% Sweeps         : 0
% Trigger Type   : 0 (PS4000A_SIGGEN_RISING)
% Trigger Source : 0 (PS4000A_SIGGEN_NONE)
% Ext. Threshold : 0
[status.setSigGenBuiltIn] = invoke(sigGenGroupObj, 'setSigGenBuiltIn', 1, 50.0, 1, 1, 0, 0, 0, 0, 0, 0);

%% Turn Off Signal Generator
% Sets the output to 0V DC

[status.setSigGenOff] = invoke(sigGenGroupObj, 'setSigGenOff');

%% Arbitrary Waveform Generator - Set Parameters

% Configure property value(s).
set(ps4000aDeviceObj.Signalgenerator(1), 'startFrequency', 2000.0);
set(ps4000aDeviceObj.Signalgenerator(1), 'stopFrequency', 2000.0);
set(ps4000aDeviceObj.Signalgenerator(1), 'offsetVoltage', 0.0);
set(ps4000aDeviceObj.Signalgenerator(1), 'peakToPeakVoltage', 2000.0);

% Define Arbitrary Waveform - must be in range -1 to +1
% Arbitrary waveforms can also be read in from text and csv files using
% dlmread and csvread respectively.
% AWG Files created using PicoScope 6 can be read using the above method.

awgBufferSize = get(sigGenGroupObj, 'awgBufferSize');
x = [0:(2*pi)/(awgBufferSize - 1):2*pi];
y = normalise(sin(x) + sin(2*x));

%% Arbitrary Waveform Generator - Simple
% Output an arbitrary waveform with constant frequency

% Arb. Waveform : y (defined above)
[status.setSigGenArbitrarySimple] = invoke(sigGenGroupObj, 'setSigGenArbitrarySimple', y);

%% Turn Off Signal Generator
% Sets the output to 0V DC
[status.setSigGenOff] = invoke(sigGenGroupObj, 'setSigGenOff');

%% Arbitrary Waveform Generator - Output Shots

% Increment      : 0 (Hz)
% Dwell Time     : 1 (s)
% Arb. Waveform  : y (defined above)
% Sweep Type     : 0 (PS4000A_UP)
% Operation      : 0 (PS4000A_ES_OFF)
% Shots          : 2 
% Sweeps         : 0
% Trigger Type   : 0 (PS4000A_SIGGEN_RISING)
% Trigger Source : 4 (PS4000A_SIGGEN_SOFT_TRIG)
% Ext. Threshold : 0
[status.setSigGegnArbitrary] = invoke(sigGenGroupObj, 'setSigGenArbitrary', 0, 1, y, 0, 0, 0, 2, 0, 0, 4, 0);

% Trigger the AWG

% State : 1 (RISING)
[status.sigGenSoftwareControl] = invoke(sigGenGroupObj, 'ps4000aSigGenSoftwareControl', 1);

%% Turn Off Signal Generator
% Sets the output to 0V DC

[status.setSigGenOff] = invoke(sigGenGroupObj, 'setSigGenOff');

%% Disconnect
% Disconnect device object from hardware.
disconnect(ps4000aDeviceObj);
