%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS4000A_IC_Generic_Sig_Gen
%
% Copyright:   Pico Technology Limited 2014
%
% Author:      KPV
%
% Description:
%   This is a MATLAB script that demonstrates how to use the
%   PicoScope 6000 Series Instrument Control Toobox driver to control the
%   signal generator/AWG
%
%	To run this application:
%		Ensure that the following files/folders are located either in the 
%       same directory or define the path in the PS6000Config.m file:
%       
%       - picotech_ps6000_generic.mdd
%       - PS6000Constants
%       - ps6000.dll & ps6000Wrap.dll 
%       - ps6000MFile.m & ps6000WrapMFile.m
%       - PicoConstants.m
%       - PicoStatus.m
%       - Functions
%
%   Device used to generated example: PicoScope 6404D
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Load Configuration Information

PS6000Config;

%% Device Connection

% Create a device object. 
ps6000DeviceObj = icdevice('picotech_ps6000_generic.mdd');

% Connect device object to hardware.
connect(ps6000DeviceObj);

%% Obtain Signal Generator Group Object
% Signal Generator properties and functions are located in the Instrument
% Driver's signalGenerator group.

sigGenGroupObj = get(ps6000DeviceObj, 'Signalgenerator');
sigGenGroupObj = sigGenGroupObj(1);

%% Function Generator - Simple
% Output a Sine wave, 2000mVpp, 0mV offset, 1000Hz (uses preset values for 
% offset, peak to peak voltage and frequency)

[status.setSigGenBuiltInSimple] = invoke(sigGenGroupObj, 'setSigGenBuiltInSimple', 0);

%% Function Generator - Sweep Frequency

% Configure property value(s).
set(ps6000DeviceObj.Signalgenerator(1), 'startFrequency', 50.0);
set(ps6000DeviceObj.Signalgenerator(1), 'stopFrequency', 500.0);
set(ps6000DeviceObj.Signalgenerator(1), 'offsetVoltage', 500.0);
set(ps6000DeviceObj.Signalgenerator(1), 'peakToPeakVoltage', 2400.0);

% Execute device object function(s).

% Wavetype       : 1 (PS6000_SQUARE) 
% Increment      : 50.0 (Hz)
% Dwell Time     : 1 (s)
% Sweep Type     : 1 (PS6000_DOWN)
% Operation      : 0 (PS6000_ES_OFF)
% Shots          : 0 
% Sweeps         : 0
% Trigger Type   : 0 (PS6000_SIGGEN_RISING)
% Trigger Source : 0 (PS6000_SIGGEN_NONE)
% Ext. Threshold : 0
[status.setSigGenBuiltIn] = invoke(sigGenGroupObj, 'setSigGenBuiltIn', 1, 50.0, 1, 1, 0, 0, 0, 0, 0, 0);

%% Turn Off Signal Generator
% Sets the output to 0V DC

[status.setSigGenOff] = invoke(sigGenGroupObj, 'setSigGenOff');

%% Arbitrary Waveform Generator - Set Parameters

% Configure property value(s).
set(ps6000DeviceObj.Signalgenerator(1), 'startFrequency', 2000.0);
set(ps6000DeviceObj.Signalgenerator(1), 'stopFrequency', 2000.0);
set(ps6000DeviceObj.Signalgenerator(1), 'offsetVoltage', 0.0);
set(ps6000DeviceObj.Signalgenerator(1), 'peakToPeakVoltage', 2000.0);

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
% Sweep Type     : 0 (PS6000_UP)
% Operation      : 0 (PS6000_ES_OFF)
% Shots          : 2 
% Sweeps         : 0
% Trigger Type   : 0 (PS6000_SIGGEN_RISING)
% Trigger Source : 4 (PS6000_SIGGEN_SOFT_TRIG)
% Ext. Threshold : 0
[status] = invoke(sigGenGroupObj, 'setSigGenArbitrary', 0, 1, y, 0, 0, 0, 2, 0, 0, 4, 0);

% Trigger the AWG

% State : 1 (RISING)
[status] = invoke(sigGenGroupObj, 'ps6000SigGenSoftwareControl', 1);

%% Turn Off Signal Generator
% Sets the output to 0V DC

[status.setSigGenOff] = invoke(sigGenGroupObj, 'setSigGenOff');

%% Disconnect
% Disconnect device object from hardware.
disconnect(ps6000DeviceObj);
