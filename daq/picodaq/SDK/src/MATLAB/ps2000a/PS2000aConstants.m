%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS2000aConstants
%
% Copyright:   Pico Technology Limited 2013
%
% Author:      HSM
%
% Description:
%   This is a MATLAB script that contains reference information for the
%   PicoScope 2000 (A API) Instrument Control Driver - DO NOT EDIT.
%
% Ensure that the file is on the MATLAB Path.		
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
classdef PS2000aConstants
    %PS2000ACONSTANTS Defines PicoScope 2000 Series constants from header
    %file ps2000aApi.h
    %   The PS2000AConstants class defines a number of constant values that
    %   can be used to define the properties of an Oscilloscopes device or
    %   for passing as parameters to function calls.
    
    properties (Constant)
        
        PS2208_MAX_ETS_CYCLES	= 500;
        PS2208_MAX_INTERLEAVE	= 20;

        PS2207_MAX_ETS_CYCLES	= 500;
        PS2207_MAX_INTERLEAVE	= 20;

        PS2206_MAX_ETS_CYCLES	= 250;
        PS2206_MAX_INTERLEAVE	= 10;

        % External Channel ADC Count
        PS2000A_EXT_MAX_VALUE   = 32767;
        PS2000A_EXT_MIN_VALUE   = -32767;
        
        PS2000A_EXT_MAX_VOLTAGE = 5;
        PS2000A_EXT_MIN_VOLTAGE = -5;
        
        PS2000A_MAX_LOGIC_LEVEL	= 32767;
        PS2000A_MIN_LOGIC_LEVEL = -32767;
        
        % Function/Arbitrary Waveform Parameters
        MIN_SIG_GEN_FREQ = 0.0;
        MAX_SIG_GEN_FREQ = 20000000.0;

        PS2000A_MAX_SIG_GEN_BUFFER_SIZE = PicoConstants.AWG_BUFFER_8KS;
        PS2000A_MIN_SIG_GEN_BUFFER_SIZE = 1;
        
        MIN_DWELL_COUNT                 = 3;
        MAX_SWEEPS_SHOTS				= pow2(30) - 1; %1073741823

        MAX_ANALOGUE_OFFSET_50MV_200MV = 0.250;
        MIN_ANALOGUE_OFFSET_50MV_200MV = -0.250;
        MAX_ANALOGUE_OFFSET_500MV_2V   = 2.500;
        MIN_ANALOGUE_OFFSET_500MV_2V   = -2.500;
        MAX_ANALOGUE_OFFSET_5V_20V     = 20;
        MIN_ANALOGUE_OFFSET_5V_20V	   = -20; 

        % Supported by the PS2206, PS2207, PS2208 only
        PS2000A_SHOT_SWEEP_TRIGGER_CONTINUOUS_RUN = hex2dec('FFFFFFFF');

        % Frequencies
        
        PS2000A_SINE_MAX_FREQUENCY		= 1000000;
        PS2000A_SQUARE_MAX_FREQUENCY	= 1000000;
        PS2000A_TRIANGLE_MAX_FREQUENCY	= 1000000;
        PS2000A_SINC_MAX_FREQUENCY		= 1000000;
        PS2000A_RAMP_MAX_FREQUENCY		= 1000000;
        PS2000A_HALF_SINE_MAX_FREQUENCY	= 1000000;
        PS2000A_GAUSSIAN_MAX_FREQUENCY  = 1000000;
        PS2000A_PRBS_MAX_FREQUENCY		= 1000000;
        PS2000A_PRBS_MIN_FREQUENCY		= 0.03;
        PS2000A_MIN_FREQUENCY			= 0.03;

        % PicoScope 2000 series Models using the 'A' API
        
        MODEL_NONE      = 'NONE';
        
        % 2 channel variants
        MODEL_PS2205MSO     = '2205MSO';
        MODEL_PS2206        = '2206';
        MODEL_PS2206A       = '2206A';
        MODEL_PS2207        = '2207';
        MODEL_PS2207A       = '2207A';
        MODEL_PS2208        = '2208';
        MODEL_PS2208A       = '2208A';
        
    end

end

