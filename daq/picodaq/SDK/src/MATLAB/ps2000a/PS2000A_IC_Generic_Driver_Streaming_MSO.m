%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Filename:    PS2000A_IC_Generic_Driver_Streaming
%
% Copyright:   Pico Technology Limited 2014
%
% Author:      KPV
%
% Description:
%   This is a MATLAB script that demonstrates how to use the
%   PicoScope 2000 Series (A API) Instrument Control Toobox driver to
%   collect data in streaming mode for 2 channels without aggregation or a
%   trigger.
%
%	To run this application:
%		Ensure that the following files/folders are located either in the
%       same directory or define the path in the PS2000aConfig.m file:
%
%       - picotech_ps2000a_generic.mdd
%       - ps2000a.dll & ps2000aWrap.dll
%       - ps2000aMFile.m & ps2000aWrapMFile.m
%       - PicoConstants.m
%       - PicoStatus.m
%       - Functions
%
%   Device used to generated example: PicoScope 3206D MSO
%
%   History:
%
%   Feb-15 - Initial example
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Clear Workspace

clear;
clc;

%% Load Configuration Information

PS2000aConfig;

%% Parameter Definitions
% Define any parameters that might be required throughout the script.

channelA = ps2000aEnuminfo.enPS2000AChannel.PS2000A_CHANNEL_A;
channelB = ps2000aEnuminfo.enPS2000AChannel.PS2000A_CHANNEL_B;
portD0 = ps2000aEnuminfo.enPS2000DigitalPort.PS2000A_DIGITAL_PORT0;

%% Device Connection

% Create device -  specify serial number if required
% Specify serial number as 2nd argument if required.
PS2000aDeviceObj = icdevice('picotech_PS2000a_generic', '');

% Connect device
connect(PS2000aDeviceObj);

%% Display Unit Information

[infoStatus, unitInfo] = invoke(PS2000aDeviceObj, 'getUnitInfo');

unitInfo

%% Channel Setup
% All channels are enabled by default - switch off all except Channels A and B.

% Channel A
channelSettings(1).enabled = PicoConstants.TRUE;
channelSettings(1).coupling = ps2000aEnuminfo.enPS2000ACoupling.PS2000A_DC;
channelSettings(1).range = ps2000aEnuminfo.enPS2000ARange.PS2000A_2V;
channelSettings(1).analogueOffset = 0.0;

channelARangeMV = PicoConstants.SCOPE_INPUT_RANGES(channelSettings(1).range + 1);

% Channel B
channelSettings(2).enabled = PicoConstants.TRUE;
channelSettings(2).coupling = ps2000aEnuminfo.enPS2000ACoupling.PS2000A_DC;
channelSettings(2).range = ps2000aEnuminfo.enPS2000ARange.PS2000A_2V;
channelSettings(2).analogueOffset = 0.0;

channelBRangeMV = PicoConstants.SCOPE_INPUT_RANGES(channelSettings(2).range + 1);

if(PS2000aDeviceObj.channelCount == PicoConstants.QUAD_SCOPE)
    
    % Channel C
    channelSettings(3).enabled = PicoConstants.FALSE;
    channelSettings(3).coupling = ps2000aEnuminfo.enPS2000ACoupling.PS2000A_DC;
    channelSettings(3).range = ps2000aEnuminfo.enPS2000ARange.PS2000A_2V;
    channelSettings(3).analogueOffset = 0.0;
    
    % Channel D
    channelSettings(4).enabled = PicoConstants.FALSE;
    channelSettings(4).coupling = ps2000aEnuminfo.enPS2000ACoupling.PS2000A_DC;
    channelSettings(4).range = ps2000aEnuminfo.enPS2000ARange.PS2000A_2V;
    channelSettings(4).analogueOffset = 0.0;
    
end

% Keep the status values returned from the driver.
numChannels = get(PS2000aDeviceObj, 'channelCount');

for ch = 1:numChannels
    
    status.setChannelStatus(ch) = invoke(PS2000aDeviceObj, 'ps2000aSetChannel', ...
        (ch - 1), channelSettings(ch).enabled, ...
        channelSettings(ch).coupling, channelSettings(ch).range, ...
        channelSettings(ch).analogueOffset);
    
end

digitalObj = get(PS2000aDeviceObj,'Digital');
invoke(digitalObj, 'ps2000aSetDigitalPort', evalin('base', 'ps2000aEnuminfo.enPS2000DigitalPort.PS2000A_DIGITAL_PORT0'), 1, 2.5);
invoke(digitalObj, 'ps2000aSetDigitalPort', evalin('base', 'ps2000aEnuminfo.enPS2000DigitalPort.PS2000A_DIGITAL_PORT1'), 0, 0);

% Obtain the maximum ADC Count from the driver
maxADCCount = double(get(PS2000aDeviceObj, 'maxADCValue'));

%% Trigger Setup
% Turn off trigger
% If a trigger is set and the autoStop property in the driver is set to
% '1', the device will stop collecting data once the number of post trigger
% samples have been collected.

% Trigger properties and functions are located in the Instrument
% Driver's Trigger group.

triggerGroupObj = get(PS2000aDeviceObj, 'Trigger');
triggerGroupObj = triggerGroupObj(1);

[status.setTriggerOff] = invoke(triggerGroupObj, 'setTriggerOff');

%% Set Data Buffers
% Data buffers for Channel A and B - buffers should be set with the driver,
% and these MUST be passed with application buffers to the wrapper driver
% in order to ensure data is correctly copied.

overviewBufferSize  = 250000; % Size of the buffer to collect data from buffer.
segmentIndex        = 0;

ratioMode = ps2000aEnuminfo.enPS2000ARatioMode.PS2000A_RATIO_MODE_NONE;

% Buffers to be passed to the driver
pDriverBufferChA = libpointer('int16Ptr', zeros(overviewBufferSize, 1));
pDriverBufferChB = libpointer('int16Ptr', zeros(overviewBufferSize, 1));
pDriverBufferPortD0 = libpointer('int16Ptr', zeros(overviewBufferSize, 1));

status.setDataBufferChA = invoke(PS2000aDeviceObj, 'ps2000aSetDataBuffer', ...
    channelA, pDriverBufferChA, overviewBufferSize, segmentIndex, ratioMode);

status.setDataBufferChB = invoke(PS2000aDeviceObj, 'ps2000aSetDataBuffer', ...
    channelB, pDriverBufferChB, overviewBufferSize, segmentIndex, ratioMode);

status.setDataBufferPortD0 = invoke(PS2000aDeviceObj, 'ps2000aSetDataBuffer', ...
    portD0, pDriverBufferPortD0, overviewBufferSize, segmentIndex, ratioMode);

% Application Buffers - these are for copying from the driver into temporarily.
pAppBufferChA = libpointer('int16Ptr', zeros(overviewBufferSize, 1));
pAppBufferChB = libpointer('int16Ptr', zeros(overviewBufferSize, 1));
pAppBufferPortD0 = libpointer('int16Ptr', zeros(overviewBufferSize, 1));

% Streaming properties and functions are located in the Instrument
% Driver's Streaming group.

streamingGroupObj = get(PS2000aDeviceObj, 'Streaming');
streamingGroupObj = streamingGroupObj(1);

% Register application buffer and driver buffers (with the wrapper).

status.setAppAndDriverBuffersA = invoke(streamingGroupObj, 'setAppAndDriverBuffers', channelA, ...
    pAppBufferChA, pDriverBufferChA, overviewBufferSize);

status.setAppAndDriverBuffersB = invoke(streamingGroupObj, 'setAppAndDriverBuffers', channelB, ...
    pAppBufferChB, pDriverBufferChB, overviewBufferSize);

status.setAppAndDriverPortD0 = invoke(streamingGroupObj, 'setAppAndDriverDigiBuffers', 0, ...
    pAppBufferPortD0, pDriverBufferPortD0, overviewBufferSize);

%% Run Streaming And Get Values
% Use default value for streaming interval which is 1e-6 for 1MS/s
% Collect data for 1 second with auto stop - maximum array size will depend
% on PC's resources - type 'memory' at MATLAB command prompt for further
% information.

% To change the sample interval e.g 5 us for 200KS/s. The call to
% PS2000aRunStreaming will output the actual sampling interval used by the
% driver.
%set(streamingGroupObj, 'streamingInterval', 5e-6);

% For 10MS/s, specify 100ns
%set(streamingGroupObj, 'streamingInterval', 100e-9);

% Set the number of pre- and post-trigger samples
% If no trigger is set 'numPreTriggerSamples' is ignored
set(PS2000aDeviceObj, 'numPreTriggerSamples', 0);
set(PS2000aDeviceObj, 'numPostTriggerSamples', 2000000);

% autoStop parameter can be set to false (0)
%set(streamingGroupObj, 'autoStop', PicoConstants.FALSE);

% Set other streaming parameters
downSampleRatio     = 1;
downSampleRatioMode = ps2000aEnuminfo.enPS2000ARatioMode.PS2000A_RATIO_MODE_NONE;

% Defined buffers to store data collected from channels.
% If capturing data without using the autoStop flag, or if using a trigger
% with the autoStop flag, allocate sufficient space (1.5 times the size is
% shown below) to allow for pre-trigger data. Pre-allocating the array is
% more efficient than using vertcat to combine data.

maxSamples = get(PS2000aDeviceObj, 'numPreTriggerSamples') + ...
    get(PS2000aDeviceObj, 'numPostTriggerSamples');

% Take into account the downSamplesRatioMode - required if collecting data
% without a trigger and using the autoStop flag.
% finalBufferLength = round(1.5 * maxSamples / downSampleRatio);

pBufferChAFinal = libpointer('int16Ptr', zeros(maxSamples, 1, 'int16'));
pBufferChBFinal = libpointer('int16Ptr', zeros(maxSamples, 1, 'int16'));
pBufferPortD0Final = libpointer('int16Ptr', zeros(maxSamples, 1, 'int16'));

% Prompt to press enter to begin capture
input('Press ENTER to begin data collection.', 's');

[status.runStreaming, actualSampleInterval, sampleIntervalTimeUnitsStr] = ...
    invoke(streamingGroupObj, 'ps2000aRunStreaming', downSampleRatio, ...
    downSampleRatioMode, overviewBufferSize);

disp('Streaming data...');
fprintf('Click the STOP button to stop capture or wait for auto stop if enabled.\n\n')

% Variables to be used when collecting the data
hasAutoStopped      = PicoConstants.FALSE;
newSamples          = 0; % Number of new samples returned from the driver.
previousTotal       = 0; % The previous total number of samples.
totalSamples        = 0; % Total samples captured by the device.
startIndex          = 0; % Start index of data in the buffer returned.
hasTriggered        = 0; % To indicate if trigger has occurred.
triggeredAtIndex    = 0; % The index in the overall buffer where the trigger occurred.

time = zeros(overviewBufferSize / downSampleRatio, 1);	% Array to hold time values

status.getStreamingLatestValues = PicoStatus.PICO_OK; % OK

[stopFig.h, stopFig.h] = stopButton();

flag = 1; % Use flag variable to indicate if stop button has been clicked (0)
setappdata(gcf, 'run', flag);

% Plot Properties

% Plot on a single figure
figure1 = figure('Name', 'PicoScope 2000 Series - Streaming Data Capture', 'NumberTitle', 'off');

analogue = subplot(2,1,1);

digital = subplot(2,1,2);

% Calculate limit - use max of multiple channels if plotting on same graph
% Estimate x limit to try and avoid using too much CPU when drawing
xlim(analogue, [0 (actualSampleInterval * maxSamples)]);
xlim(digital, [0 (actualSampleInterval * maxSamples)]);

yRange = channelARangeMV + 0.5;
ylim(analogue,[(-1 * yRange) yRange]);


hold(analogue,'on');
hold (digital, 'on');

grid(analogue, 'on');
grid(digital, 'on');

title(analogue,'Analogue');
title(digital, 'Digital');
xLabelStr = strcat('Time (', sampleIntervalTimeUnitsStr, ')');
xlabel(xLabelStr);
ylabel('Voltage (mV)');

% Get data values as long as power status has not changed (check for STOP button push inside loop)
while(hasAutoStopped == PicoConstants.FALSE && status.getStreamingLatestValues == PicoStatus.PICO_OK)
    
    ready = PicoConstants.FALSE;
    
    while(ready == PicoConstants.FALSE)
        
        status.getStreamingLatestValues = invoke(streamingGroupObj, 'getStreamingLatestValues');
        
        ready = invoke(streamingGroupObj, 'isReady');
        
        % Give option to abort from here
        flag = getappdata(gcf, 'run');
        drawnow;
        
        if(flag == 0)
            
            disp('STOP button clicked - aborting data collection.')
            break;
            
        end
        
        drawnow;
        
    end
    
    % Check for data
    [newSamples, startIndex] = invoke(streamingGroupObj, 'availableData');
    
    if (newSamples > 0)
        
        % Check if the scope has triggered
        [triggered, triggeredAt] = invoke(streamingGroupObj, 'isTriggerReady');
        
        if (triggered == PicoConstants.TRUE)
            
            % Adjust trigger position as MATLAB does not use zero-based
            % indexing
            
            bufferTriggerPosition = triggeredAt + 1;
            
            fprintf('Triggered - index in buffer: %d\n', bufferTriggerPosition);
            
            hasTriggered = triggered;
            
            % Adjust by 1 due to driver using zero indexing
            triggeredAtIndex = totalSamples + bufferTriggerPosition;
            
        end
        
        previousTotal = totalSamples;
        totalSamples = totalSamples + newSamples;
        
        % Printing to console can slow down acquisition - use for debug
        fprintf('Collected %d samples, startIndex: %d total: %d.\n', newSamples, startIndex, totalSamples);
        
        % Position indices of data in buffer
        firstValuePosn = startIndex + 1;
        lastValuePosn = startIndex + newSamples;
        
        % Convert data values to milliVolts from the application buffers
        bufferChAmV = adc2mv(pAppBufferChA.Value(firstValuePosn:lastValuePosn), channelARangeMV, maxADCCount);
        bufferChBmV = adc2mv(pAppBufferChB.Value(firstValuePosn:lastValuePosn), channelBRangeMV, maxADCCount);
        bufferPortD0 = pAppBufferPortD0.Value(firstValuePosn:lastValuePosn);
        
        %         Process collected data further if required - this example plots
        %         the data.
        
        % Copy data into final buffers
        pBufferChAFinal.Value(previousTotal + 1:totalSamples) = bufferChAmV;
        pBufferChBFinal.Value(previousTotal + 1:totalSamples) = bufferChBmV;
        pBufferPortD0Final.Value(previousTotal + 1:totalSamples) = bufferPortD0;
        
        % Time axis
        
        % Multiply by ratio mode as samples get reduced
        time = (double(actualSampleInterval) * double(downSampleRatio)) * [previousTotal:(totalSamples - 1)];
        
        % Plot channel A only
        plot(analogue, time, bufferChAmV);
        plot(digital, time, bufferPortD0);
        
        % Clear variables for use again
        clear bufferChAMV;
        clear bufferChBMV;
        clear bufferPortD0;
        clear firstValuePosn;
        clear lastValuePosn;
        clear startIndex;
        clear triggered;
        clear triggerAt;
        
    end
    
    % Check if auto stop has occurred
    hasAutoStopped = invoke(streamingGroupObj, 'autoStopped');
    
    if(hasAutoStopped == PicoConstants.TRUE)
        
        disp('AutoStop: TRUE - exiting loop.');
        break;
        
    end
    
    % Check if 'STOP' button pressed
    
    flag = getappdata(gcf, 'run');
    drawnow;
    
    if(flag == 0)
        
        disp('STOP button clicked - aborting data collection.')
        break;
        
    end
    
end

% Close the STOP button window
if(exist('stopFig', 'var'))
    
    close('Stop Button');
    clear stopFig;
    
end

drawnow;

if(hasTriggered == PicoConstants.TRUE)
    
    fprintf('Triggered at overall index: %d\n', triggeredAtIndex);
    
end

% Take hold off the current figure
hold off;

fprintf('\n');

%% STOP THE DEVICE
% This function should be called regardless of whether auto stop is enabled
% or not.

status.stop = invoke(PS2000aDeviceObj, 'ps2000aStop');

%% FIND THE NUMBER OF SAMPLES
% This is the number of samples held in the driver itself.
[status.noOfStreamingValues, numStreamingValues] = invoke(streamingGroupObj, 'ps2000aNoOfStreamingValues');

fprintf('Number of samples available from the driver: %u.\n\n', numStreamingValues);

%% PROCESS DATA
% Process all data if required

% Reduce size of arrays if required
if(totalSamples < maxSamples)
    
    pBufferChAFinal.Value(totalSamples + 1:end) = [];
    pBufferChBFinal.Value(totalSamples + 1:end) = [];
    pBufferPortD0Final.Value(totalSamples + 1:end) = [];
    
end

channelAFinal = pBufferChAFinal.Value();
channelBFinal = pBufferChBFinal.Value();
PortD0Final = pBufferPortD0Final.Value();

% Plot total data on another figure
anagloueFinalFigure = figure('Name', 'Streaming Analogue Data Capture', 'NumberTitle', 'off');
axes2 = axes('Parent', anagloueFinalFigure);
hold on;

xLabelStr = strcat('Time (', sampleIntervalTimeUnitsStr, ')');
xlabel(xLabelStr);
ylabel('Voltage (mV)');

% Find the maximum voltage range
maxYRange = max(channelARangeMV, channelBRangeMV);
ylim(axes2,[(-1 * maxYRange) maxYRange]);

time = (double(actualSampleInterval) * double(downSampleRatio)) * [0:length(channelAFinal) - 1];
plot(time, channelAFinal, time, channelBFinal);

grid on;
legend('Channel A', 'Channel B');

hold off;

movegui(anagloueFinalFigure, 'northwest');
drawnow;

disp('Converting decimal data into binary array')

D0 = zeros(length(PortD0Final), 8);

for sample = 1:length(PortD0Final)
    
    D0(sample,:)= bitget(PortD0Final(sample), 8:-1:1, 'int16');
    
end

digitalFinalFigure  = figure('Name','Streaming Digita; Data Capture ', ...
    'NumberTitle','off');



for i = 0:7
subplot(4,2,i+1); 
plot(time, D0(:,(8 - i)));
title(strcat('Digital Channel D', num2str(i)), 'FontWeight', 'bold');
xlabel(strcat('Time (', sampleIntervalTimeUnitsStr, ')'));
ylabel('Logic Level');
axis([-inf, inf, -0.5,1.5])
end

movegui(digitalFinalFigure, 'northeast');


%% DISCONNECT DEVICE

disconnect(PS2000aDeviceObj);