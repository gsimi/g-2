%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS4000aConstants
%
% Copyright:   Pico Technology Limited 2014
%
% Author:      KPV
%
% Description:
%   This is a MATLAB script that contains reference information for the
%   PicoScope 4000a Instrument Control Driver - DO NOT EDIT.
%
%   Ensure that the file is on the MATLAB Path.		
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
classdef PS4000aConstants
    %PS4000aCONSTANTS Defines PicoScope 4000a Series constants from header
    %   file ps4000AApi.h
    %   The PS4000aConstants class defines a number of constant values that
    %   can be used to define the properties of an Oscilloscopes device or
    %   for passing as parameters to function calls.
    
    properties (Constant)
        
        PS4000A_MAX_VALUE               = 32767;
		PS4000A_MIN_VALUE               = -32767;
		
        PS4000A_EXT_MAX_VALUE           = 32767;
		PS4000A_EXT_MIN_VALUE           = -32767;

        MAX_PULSE_WIDTH_QUALIFIER_COUNT = 16777215;
		MAX_DELAY_COUNT 				= 8388607

		PS4000A_MAX_SIG_GEN_BUFFER_SIZE = 16384;   
        PS4000A_MIN_SIG_GEN_BUFFER_SIZE = 10;
        PS4000A_MIN_DWELL_COUNT         = 10;
        PS4000A_MAX_SWEEPS_SHOTS		= pow2(30) - 1; %1073741823
	
        PS4000A_MAX_ANALOGUE_OFFSET_50MV_200MV  = 0.250;
        PS4000A_MIN_ANALOGUE_OFFSET_50MV_200MV  = -0.250;
        PS4000A_MAX_ANALOGUE_OFFSET_500MV_2V    = 2.500;
        PS4000A_MIN_ANALOGUE_OFFSET_500MV_2V    = -2.500;
        PS4000A_MAX_ANALOGUE_OFFSET_5V_20V      = 20;
        PS4000A_MIN_ANALOGUE_OFFSET_5V_20V      = -20;

        % Frequencies
        
        PS4000A_SINE_MAX_FREQUENCY      = 1000000;
        PS4000A_SQUARE_MAX_FREQUENCY    = 1000000;
        PS4000A_TRIANGLE_MAX_FREQUENCY	= 1000000;
        PS4000A_SINC_MAX_FREQUENCY		= 1000000
        PS4000A_RAMP_MAX_FREQUENCY		= 1000000;
        PS4000A_HALF_SINE_MAX_FREQUENCY	= 1000000;
        PS4000A_GAUSSIAN_MAX_FREQUENCY  = 1000000;
        PS4000A_MIN_FREQUENCY           = 0.03;

        % PicoScope 4000 series Models (using A API)
        
        MODEL_NONE      = 'NONE';
        
        % Variants that can be used
        MODEL_PS4225   = '4225';
        MODEL_PS4425   = '4425';
		MODEL_PS4824   = '4824';

    end

end

