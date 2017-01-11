%% P34000A_IC_BLOCK_TEST
% Example script showing how to capture block data with PicoScope 2000
% Series

PS2000aConfig

ps2000aDeviceObj = icdevice('picotech_ps2000a_generic.mdd', '');
connect(ps2000aDeviceObj)

for i = 0:1
   status = invoke(ps2000aDeviceObj,'ps2000aSetChannel', i, 1, 1, 8, 0);
end


timebase = 10;
set(ps2000aDeviceObj, 'timebase', timebase);
[status, timeIntervalNanoSeconds, maxSamples] = invoke(ps2000aDeviceObj, 'ps2000aGetTimebase2', timebase, 0);

digitalObj = get(ps2000aDeviceObj,'Digital')
status = invoke(digitalObj, 'ps2000aSetDigitalPort', evalin('base', 'ps2000aEnuminfo.enPS2000DigitalPort.PS2000A_DIGITAL_PORT0'), 1, 2.5);
status = invoke(digitalObj, 'ps2000aSetDigitalPort', evalin('base', 'ps2000aEnuminfo.enPS2000DigitalPort.PS2000A_DIGITAL_PORT1'), 0, 0);

blockObj = get(ps2000aDeviceObj, 'Block');
blockObj = blockObj(1);
[status] = invoke(blockObj, 'runBlock', 0);

[numSamples, overflow, chA, chB, chC, chD, portD0, portD1] = invoke(blockObj, 'getBlockData', 0, 0, 1, 0);

[status] = invoke(ps2000aDeviceObj, 'ps2000aStop');

D0 = zeros(numSamples, 8);

disp('Converting decimal data into binary array')

for sample = 1:numSamples

    D0(sample,:)= bitget(portD0(sample), 8:-1:1, 'int16');
end


%% Plot data

scrsz = get(0,'ScreenSize');
figure1 = figure('Name','PicoScope 2000 Demonstration - Digital Block Mode Capture', ...
    'NumberTitle','off');

movegui(figure1, 'northwest');

axes1 = axes('Parent', figure1);
title(axes1, 'PicoScope 2000 Digital Demonstration', 'FontWeight', 'Bold');

% Calculate time (nanoseconds) and convert to milliseconds
% Use timeIntervalNanoSeconds output from ps5000aGetTimebase or
% ps5000aGetTimebase2 or calculate from Programmer's Guide.

timeNs = double(timeIntervalNanoSeconds) * double([0:numSamples - 1]);
timeMs = timeNs / 1e6;


for i = 0:7
subplot(4,2,i+1); 
plot(timeMs, D0(:,(8 - i)));
title(strcat('Digital Channel D', num2str(i)), 'FontWeight', 'bold');
xlabel('Time (ms)');
ylabel('Logic Level');
axis([-inf, inf, -0.5,1.5])
end

%scrsz = get(0,'ScreenSize');
figure2 = figure('Name','PicoScope 2000 Demonstration - Block Mode Capture', ...
    'NumberTitle','off');

movegui(figure2, 'northeast');

axes1 = axes('Parent', figure2);
title(axes1, 'PicoScope 2000 Demonstration', 'FontWeight', 'Bold');

% Calculate time (nanoseconds) and convert to milliseconds
% Use timeIntervalNanoSeconds output from ps5000aGetTimebase or
% ps5000aGetTimebase2 or calculate from Programmer's Guide.

timeNs = double(timeIntervalNanoSeconds) * double([0:numSamples - 1]);
timeMs = timeNs / 1e6;

% Channel A
axisHandleChA = subplot(2,1,1); 
plot(axisHandleChA, timeMs, chA);
title(axisHandleChA, 'Channel A', 'FontWeight', 'bold');
xlabel(axisHandleChA, 'Time (ms)');
ylabel(axisHandleChA, 'Voltage (mV)');
grid(axisHandleChA);

% Channel B

axisHandleChB = subplot(2,1,2); 
plot(axisHandleChB, timeMs, chB);
title(axisHandleChB, 'Channel B', 'FontWeight', 'bold');
xlabel(axisHandleChB, 'Time (ms)');
ylabel(axisHandleChB, 'Voltage (mV)');
grid(axisHandleChB);


%% Disconnect
disconnect(ps2000aDeviceObj);
