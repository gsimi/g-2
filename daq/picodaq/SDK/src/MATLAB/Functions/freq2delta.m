function [ delta ] = freq2delta( frequency, waveformSize, maxBufferSize,  dacFrequency )
%FREQ2DELTA Convert frequency to delta value for PicoScope AWG.
%   freq2delta(F, W, B, D) converts a frequency value F in Hertz into a 
%   uint32 delta value using the waveform size W, maximum buffer size of
%   the PicoScope device B and DAC Frequency of the Arbitrary Waveform
%   Generator, D. 
%
%   F can be any numeric value, such as DOUBLE while W can be a signed 
%   16-bit or 32-bit integer. B and D will be defined by the properties of 
%   the PicoScope device being used when using the Instrument Control
%   driver, otherwise they should be signed 32-bit integers.
%
%   The delta value is returned as an unsigned 32-bit integer.

    delta = uint32(((frequency * waveformSize) / maxBufferSize) * ...
        PicoConstants.AWG_PHASE_ACCUMULATOR * (1/dacFrequency));

end

