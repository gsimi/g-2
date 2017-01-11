%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS2000_IC_Generic_Driver_SigGen.m
%
% Copyright:   Pico Technology Limited 2013 - 2014
%
% Author:      HSM
%
% Description:
%   This is a MATLAB script that demonstrates how to use the
%   PicoScope 2000 series Instrument Control Toobox driver to generate
%   signals using the function generator (applies to PicoScope 2203, 2204
%   2204A, 2205 & 2205A ONLY).
%
%   A part of this script was generated using the Test and Measurement
%   Tool.
%
% Examples:
%   Generate a waveform of constant frequency with/without offset.
%   Generate a waveform and sweep between the specified start/stop frequency.
%   Generate an arbitrary waveform of constant frequency with/without offset.
%   Generate an arbitrary waveform sweep between the specified start/stop frequency.
%   Turn off the function generator.
%
%	To run this application:
%		
%   Click 'Run' in the MATLAB Editor or type the name of the file,
%   PS2000_IC_Generic_Driver_SigGen_BuiltIn, at the MATLAB command prompt.
% 
%   The file, PS2000_IC_GENERIC_DRIVER_SIGGEN_BUILTIN.M must be on your 
%   MATLAB PATH. For additional information on setting your MATLAB PATH, 
%   type 'help addpath' at the MATLAB command prompt.
%
%   Example:
%       PS2000_IC_Generic_Driver_SigGen;
%
%   See also ICDEVICE.
%
%
%   Creation time: 18-Jan-2013 15:08:14 
%   Update 06-Nov-2014 - modified after groups added to Instrument Driver.
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

sigGenGroupObj = get(ps2000DeviceObj, 'Signalgenerator');
sigGenGroupObj = sigGenGroupObj(1);

%% BUILT-IN SIGNAL GENERATOR

% Execute device object function(s).

% Use the evalin command in the input arguments line in the Test and
% Measurement Tool using the 'base' workspace to load in variables from the
% MATLAB environment.
% When writing a script, the 'evalin' command is not required.

% Output sine wave
%
% offsetVoltage and pkToPk are loaded in from the PS2000Config file.
%
% frequency : 500 Hz

set(sigGenGroupObj, 'startFrequency', 500);
set(sigGenGroupObj, 'stopFrequency', 500);

[sigGenSimpleStatus] = invoke(sigGenGroupObj, 'setSigGenBuiltInSimple', ...
    evalin('base', 'ps2000Enuminfo.enPS2000WaveType.PS2000_SINE'));

pause(5);

% Change to square wave
%
% peak to peak   : 2000mV
% wave type      : Square (PS2000_SQUARE)
% frequency      : 1000Hz
%

set(sigGenGroupObj, 'peakToPeakVoltage', 2000);
set(sigGenGroupObj, 'startFrequency', 1000);
set(sigGenGroupObj, 'stopFrequency', 1000);

[sigGenSimpleStatus] = invoke(sigGenGroupObj, 'setSigGenBuiltInSimple', 1);

pause(5);

% Output a triangle waveform and sweep up from start to stop frequency
%
% This shows how to pass in parameters loaded from PS2000Config without
% using 'evalin'
%
% startFrequency : 250
% stopFrequency  : 3000
% wave type      : Triangle (PS2000_TRIANGLE)

set(sigGenGroupObj, 'startFrequency', 250);
set(sigGenGroupObj, 'stopFrequency', 3000);

[sigGenBuiltInStatus] = invoke(sigGenGroupObj, 'ps2000SetSigGenBuiltIn', ...
    ps2000Enuminfo.enPS2000WaveType.PS2000_TRIANGLE, sigGen.builtIn.increment, ...
    sigGen.builtIn.dwellTime, sigGen.sweepType, sigGen.sweeps);

pause(5);

% Output a sine wave and sweep up and down from start to stop frequency in 
% 500Hz steps 5 times

% offset voltage : 0V
% peak to peak   : 2000mV
% wave type      : Sine (PS2000_SINE)
% start frequency: 500Hz
% stop frequency : 5000Hz
% increment      : 500Hz
% dwell time     : 2 seconds
% sweep type     : 2 (PS2000_UPDOWN)
% num. sweeps    : 5

set(sigGenGroupObj, 'offsetVoltage', 0);
set(sigGenGroupObj, 'peakToPeakVoltage', 2000);
set(sigGenGroupObj, 'startFrequency', 500);
set(sigGenGroupObj, 'stopFrequency', 5000);

[sigGenBuiltInStatus1] = invoke(sigGenGroupObj, 'ps2000SetSigGenBuiltIn', ...
    ps2000Enuminfo.enPS2000WaveType.PS2000_SINE, 500, 2, 2, 5);

%% ARBITRARY WAVEFORM GENERATOR

% Output an arbitrary waveform of constant frequency
%
% offsetVoltage and pkToPk are loaded in from the PS2000Config file.
%
% offset voltage : 0V
% peak to peak   : 2000mV
% frequency      : 500 Hz

set(sigGenGroupObj, 'offsetVoltage', 0);
set(sigGenGroupObj, 'peakToPeakVoltage', 2000);
set(sigGenGroupObj, 'startFrequency', 500);

[sigGenArbitrarySimpleStatus] = invoke(sigGenGroupObj, 'setSigGenArbitrarySimple', sigGen.awg.waveform);

pause(5);

disp('Sweep AWG...');

% Output a sine wave and sweep up and down from start to stop frequency in 
% 500Hz steps 5 times

% offset voltage : 0V
% peak to peak   : 2000mV
% start frequency: 500Hz
% stop frequency : 1000Hz
% increment      : 50Hz
% dwell time     : 10 milliseconds
% sweep type     : 0 (PS2000_UP)
% num. sweeps    : 1

set(sigGenGroupObj, 'offsetVoltage', 0);
set(sigGenGroupObj, 'peakToPeakVoltage', 2000);
set(sigGenGroupObj, 'startFrequency', 500);
set(sigGenGroupObj, 'stopFrequency', 1000);

[sigGenArbitraryStatus] = invoke(sigGenGroupObj, 'ps2000SetSigGenArbitrary', 50, 0.01, sigGen.awg.waveform, 0, 1);

pause(5);

%% TURN OFF SIGNAL GENERATOR

% Turn off the signal generator
[sigGenOffStatus] = invoke(sigGenGroupObj, 'setSigGenOff');

%% DISCONNECT

% Disconnect device object from hardware.
disconnect(ps2000DeviceObj);