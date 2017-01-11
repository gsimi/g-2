classdef PS4000Constants
    %PS4000CONSTANTS Defines PicoScope 4000 Series constants from header
    %file ps4000Api.h
    %   The PS4000Constants class defines a number of constant values that
    %   can be used to define the properties of an Oscilloscopes device or
    %   for passing as parameters to function calls.
    
    properties (Constant)
        
        PS4262_MAX_VALUE	        = 32767;
		PS4262_MIN_VALUE	        = -32767;
        
        PS4000_MAX_VALUE            = 32764;
        PS4000_MIN_VALUE            = -32764;
        
        PS6000_MAX_ETS_CYCLES      = 400;		
        PS6000_MAX_ETS_INTERLEAVE  = 80;
        
        PS4262_SIGGEN_MAXPKTOPK = 2000000;
        PS4000_SIGGEN_MAXPKTOPK = 4000000;     

        MAX_PULSE_WIDTH_QUALIFIER_COUNT = 16777215;
		
		MIN_SIG_GEN_FREQ = 0.0;
        MAX_SIG_GEN_FREQ = 100000.0;
        MAX_SIG_GEN_FREQ_4262 = 20000.0;

        
        MIN_SIG_GEN_BUFFER_SIZE         = 1;
        MIN_DWELL_COUNT                 = 10;
        MIN_DWELL_COUNT_4262            = 3;
        MAX_SWEEPS_SHOTS				= pow2(30) - 1; 


        % Frequencies
        
        PS4000_SINE_MAX_FREQUENCY		= 20000000;
        PS4000_SQUARE_MAX_FREQUENCY	= 20000000;
        PS4000_TRIANGLE_MAX_FREQUENCY	= 20000000;
        PS4000_SINC_MAX_FREQUENCY		= 20000000
        PS4000_RAMP_MAX_FREQUENCY		= 20000000;
        PS4000_HALF_SINE_MAX_FREQUENCY	= 20000000;
        PS4000_GAUSSIAN_MAX_FREQUENCY  = 20000000;
        PS4000_PRBS_MAX_FREQUENCY		= 1000000;
        PS4000_PRBS_MIN_FREQUENCY		= 0.03;
        PS4000_MIN_FREQUENCY			= 0.03;

  
        
    end

end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS4000Constants
%
% Copyright:   Pico Technology Limited 2014 - 2015
%
% Author:      KPV
%
% Description:
%   This is a MATLAB script that contains reference information for the
%   PicoScope 4000 Instrument Control Driver - DO NOT EDIT.
%
% Ensure that the file is on the MATLAB Path.		
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

