function [ f, h ] = stopButton( )
%STOPBUTTON Displays a stop button
% [F, H] = STOPBUTTON() displays a Stop button to check abort data collection - 
% based on Mathworks solution 1-15JIQ and MATLAB Central forum.
% F is the handle to the button's figure, H is the handle to the uicontrol
% object.

    f = figure('Name', 'Stop Button', 'menubar','none',...
              'units','pix',...
              'pos',[400 400 100 50]);
          
    h = uicontrol('string', 'STOP', ...
                'callback', 'setappdata(gcf, ''run'', 0)', 'units','pixels',...
                'position',[10 10 80 30]);

end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    stopButton.m
%
% Copyright:   Pico Technology Limited 2014 - 2015
%
% Author:      HSM
%
% Description:
%   
%		This is a MATLAB script that displays a 'Stop' button for use
%       when collecting data in Streaming mode with a Pico Technology
%       product.
%
%       Ensure that the location of this file is in your MATLAB Path.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

