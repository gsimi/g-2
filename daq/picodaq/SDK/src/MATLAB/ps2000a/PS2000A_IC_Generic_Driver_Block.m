%% P2000A_IC_BLOCK_TEST
% Example script showing how to capture block data with PicoScope 2000
% Series

PS2000aConfig

ps2000aDeviceObj = icdevice('picotech_ps2000a_generic.mdd', '');
connect(ps2000aDeviceObj)

triggerObj = get(ps2000aDeviceObj, 'Trigger');
triggerObj = triggerObj(1);

[status] = invoke(triggerObj, 'setSimpleTrigger', 0, 1000, 2);

timebase = 64;
[status, timeIntervalNanoSeconds, maxSamples] = invoke(ps2000aDeviceObj, 'ps2000aGetTimebase2', timebase, 0);

blockObj = get(ps2000aDeviceObj, 'Block');
blockObj = blockObj(1);
[status] = invoke(blockObj, 'runBlock', 0);

[numSamples, overflow, chA, chB, chC, chD] = invoke(blockObj, 'getBlockData', 0, 0, 1, 0);

[status] = invoke(ps2000aDeviceObj, 'ps2000aStop');

%% Plot data

figure1 = figure('Name','PicoScope 2000 Example - Block Mode Capture', ...
    'NumberTitle','off');

axes1 = axes('Parent', figure1);

% Calculate time (nanoseconds) and convert to milliseconds
% Use timeIntervalNanoSeconds output from ps2000aGetTimebase2 or 
% calculate from Programmer's Guide.

timeNs = double(timeIntervalNanoSeconds) * double([0:numSamples - 1]);
timeMs = timeNs / 1e6;

% Channel A
axisHandleChA = subplot(2,1,1); 
plot(axisHandleChA, timeMs, chA, 'b');
title(axisHandleChA, 'Channel A', 'FontWeight', 'bold');
xlabel(axisHandleChA, 'Time (ms)');
ylabel(axisHandleChA, 'Voltage (mV)');
grid(axisHandleChA);

% Channel B

axisHandleChB = subplot(2,1,2); 
plot(axisHandleChB, timeMs, chB, 'r');
title(axisHandleChB, 'Channel B', 'FontWeight', 'bold');
xlabel(axisHandleChB, 'Time (ms)');
ylabel(axisHandleChB, 'Voltage (mV)');
grid(axisHandleChB);


%% Disconnect
disconnect(ps2000aDeviceObj);
