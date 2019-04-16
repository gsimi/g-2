function [methodinfo,structs,enuminfo,ThunkLibName]=ps5000aMFile
%PS5000AMFILE Create structures to define interfaces found in 'ps5000aApi'.

%This function was generated by loadlibrary.m parser version 1.1.6.37 on Fri Feb 14 09:48:19 2014
%perl options:'ps5000aApi.i -outfile=ps5000aMFile.m -thunkfile=ps5000a_thunk_pcwin64.c -header=ps5000aApi.h'
ival={cell(1,0)}; % change 0 to the actual number of functions to preallocate the data.
structs=[];enuminfo=[];fcnNum=1;
fcns=struct('name',ival,'calltype',ival,'LHS',ival,'RHS',ival,'alias',ival,'thunkname', ival);
MfilePath=fileparts(mfilename('fullpath'));
ThunkLibName=fullfile(MfilePath,'ps5000a_thunk_pcwin64');
% PICO_STATUS ps5000aOpenUnit ( int16_t * handle , char * serial , PS5000A_DEVICE_RESOLUTION resolution ); 
fcns.thunkname{fcnNum}='uint32voidPtrcstringPS5000A_DEVICE_RESOLUTIONThunk';fcns.name{fcnNum}='ps5000aOpenUnit'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'cstring', 'enPS5000ADeviceResolution'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aOpenUnitAsync ( int16_t * status , char * serial , PS5000A_DEVICE_RESOLUTION resolution ); 
fcns.thunkname{fcnNum}='uint32voidPtrcstringPS5000A_DEVICE_RESOLUTIONThunk';fcns.name{fcnNum}='ps5000aOpenUnitAsync'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'cstring', 'enPS5000ADeviceResolution'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aOpenUnitProgress ( int16_t * handle , int16_t * progressPercent , int16_t * complete ); 
fcns.thunkname{fcnNum}='uint32voidPtrvoidPtrvoidPtrThunk';fcns.name{fcnNum}='ps5000aOpenUnitProgress'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'int16Ptr', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetUnitInfo ( int16_t handle , char * string , int16_t stringLength , int16_t * requiredSize , PICO_INFO info ); 
fcns.thunkname{fcnNum}='uint32int16cstringint16voidPtruint32Thunk';fcns.name{fcnNum}='ps5000aGetUnitInfo'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'cstring', 'int16', 'int16Ptr', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aFlashLed ( int16_t handle , int16_t start ); 
fcns.thunkname{fcnNum}='uint32int16int16Thunk';fcns.name{fcnNum}='ps5000aFlashLed'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aIsLedFlashing ( int16_t handle , int16_t * status ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrThunk';fcns.name{fcnNum}='ps5000aIsLedFlashing'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aCloseUnit ( int16_t handle ); 
fcns.thunkname{fcnNum}='uint32int16Thunk';fcns.name{fcnNum}='ps5000aCloseUnit'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aMemorySegments ( int16_t handle , uint32_t nSegments , int32_t * nMaxSamples ); 
fcns.thunkname{fcnNum}='uint32int16uint32voidPtrThunk';fcns.name{fcnNum}='ps5000aMemorySegments'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'int32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetChannel ( int16_t handle , PS5000A_CHANNEL channel , int16_t enabled , PS5000A_COUPLING type , PS5000A_RANGE range , float analogOffset ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_CHANNELint16PS5000A_COUPLINGPS5000A_RANGEfloatThunk';fcns.name{fcnNum}='ps5000aSetChannel'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000AChannel', 'int16', 'enPS5000ACoupling', 'enPS5000ARange', 'single'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetBandwidthFilter ( int16_t handle , PS5000A_CHANNEL channel , PS5000A_BANDWIDTH_LIMITER bandwidth ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_CHANNELPS5000A_BANDWIDTH_LIMITERThunk';fcns.name{fcnNum}='ps5000aSetBandwidthFilter'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000AChannel', 'enPS5000ABandwidthLimiter'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetTimebase ( int16_t handle , uint32_t timebase , int32_t noSamples , int32_t * timeIntervalNanoseconds , int32_t * maxSamples , uint32_t segmentIndex ); 
fcns.thunkname{fcnNum}='uint32int16uint32int32voidPtrvoidPtruint32Thunk';fcns.name{fcnNum}='ps5000aGetTimebase'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'int32', 'int32Ptr', 'int32Ptr', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetTimebase2 ( int16_t handle , uint32_t timebase , int32_t noSamples , float * timeIntervalNanoseconds , int32_t * maxSamples , uint32_t segmentIndex ); 
fcns.thunkname{fcnNum}='uint32int16uint32int32voidPtrvoidPtruint32Thunk';fcns.name{fcnNum}='ps5000aGetTimebase2'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'int32', 'singlePtr', 'int32Ptr', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetSigGenArbitrary ( int16_t handle , int32_t offsetVoltage , uint32_t pkToPk , uint32_t startDeltaPhase , uint32_t stopDeltaPhase , uint32_t deltaPhaseIncrement , uint32_t dwellCount , int16_t * arbitraryWaveform , int32_t arbitraryWaveformSize , PS5000A_SWEEP_TYPE sweepType , PS5000A_EXTRA_OPERATIONS operation , PS5000A_INDEX_MODE indexMode , uint32_t shots , uint32_t sweeps , PS5000A_SIGGEN_TRIG_TYPE triggerType , PS5000A_SIGGEN_TRIG_SOURCE triggerSource , int16_t extInThreshold ); 
fcns.thunkname{fcnNum}='uint32int16int32uint32uint32uint32uint32uint32voidPtrint32PS5000A_SWEEP_TYPEPS5000A_EXTRA_OPERATIONSPS5000A_INDEX_MODEuint32uint32PS5000A_SIGGEN_TRIG_TYPEPS5000A_SIGGEN_TRIG_SOURCEint16Thunk';fcns.name{fcnNum}='ps5000aSetSigGenArbitrary'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int32', 'uint32', 'uint32', 'uint32', 'uint32', 'uint32', 'int16Ptr', 'int32', 'enPS5000ASweepType', 'enPS5000AExtraOperations', 'enPS5000AIndexMode', 'uint32', 'uint32', 'enPS5000ASigGenTrigType', 'enPS5000ASigGenTrigSource', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetSigGenBuiltIn ( int16_t handle , int32_t offsetVoltage , uint32_t pkToPk , PS5000A_WAVE_TYPE waveType , float startFrequency , float stopFrequency , float increment , float dwellTime , PS5000A_SWEEP_TYPE sweepType , PS5000A_EXTRA_OPERATIONS operation , uint32_t shots , uint32_t sweeps , PS5000A_SIGGEN_TRIG_TYPE triggerType , PS5000A_SIGGEN_TRIG_SOURCE triggerSource , int16_t extInThreshold ); 
fcns.thunkname{fcnNum}='uint32int16int32uint32PS5000A_WAVE_TYPEfloatfloatfloatfloatPS5000A_SWEEP_TYPEPS5000A_EXTRA_OPERATIONSuint32uint32PS5000A_SIGGEN_TRIG_TYPEPS5000A_SIGGEN_TRIG_SOURCEint16Thunk';fcns.name{fcnNum}='ps5000aSetSigGenBuiltIn'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int32', 'uint32', 'enPS5000AWaveType', 'single', 'single', 'single', 'single', 'enPS5000ASweepType', 'enPS5000AExtraOperations', 'uint32', 'uint32', 'enPS5000ASigGenTrigType', 'enPS5000ASigGenTrigSource', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetSigGenBuiltInV2 ( int16_t handle , int32_t offsetVoltage , uint32_t pkToPk , PS5000A_WAVE_TYPE waveType , double startFrequency , double stopFrequency , double increment , double dwellTime , PS5000A_SWEEP_TYPE sweepType , PS5000A_EXTRA_OPERATIONS operation , uint32_t shots , uint32_t sweeps , PS5000A_SIGGEN_TRIG_TYPE triggerType , PS5000A_SIGGEN_TRIG_SOURCE triggerSource , int16_t extInThreshold ); 
fcns.thunkname{fcnNum}='uint32int16int32uint32PS5000A_WAVE_TYPEdoubledoubledoubledoublePS5000A_SWEEP_TYPEPS5000A_EXTRA_OPERATIONSuint32uint32PS5000A_SIGGEN_TRIG_TYPEPS5000A_SIGGEN_TRIG_SOURCEint16Thunk';fcns.name{fcnNum}='ps5000aSetSigGenBuiltInV2'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int32', 'uint32', 'enPS5000AWaveType', 'double', 'double', 'double', 'double', 'enPS5000ASweepType', 'enPS5000AExtraOperations', 'uint32', 'uint32', 'enPS5000ASigGenTrigType', 'enPS5000ASigGenTrigSource', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetSigGenPropertiesArbitrary ( int16_t handle , uint32_t startDeltaPhase , uint32_t stopDeltaPhase , uint32_t deltaPhaseIncrement , uint32_t dwellCount , PS5000A_SWEEP_TYPE sweepType , uint32_t shots , uint32_t sweeps , PS5000A_SIGGEN_TRIG_TYPE triggerType , PS5000A_SIGGEN_TRIG_SOURCE triggerSource , int16_t extInThreshold ); 
fcns.thunkname{fcnNum}='uint32int16uint32uint32uint32uint32PS5000A_SWEEP_TYPEuint32uint32PS5000A_SIGGEN_TRIG_TYPEPS5000A_SIGGEN_TRIG_SOURCEint16Thunk';fcns.name{fcnNum}='ps5000aSetSigGenPropertiesArbitrary'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'uint32', 'uint32', 'uint32', 'enPS5000ASweepType', 'uint32', 'uint32', 'enPS5000ASigGenTrigType', 'enPS5000ASigGenTrigSource', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetSigGenPropertiesBuiltIn ( int16_t handle , double startFrequency , double stopFrequency , double increment , double dwellTime , PS5000A_SWEEP_TYPE sweepType , uint32_t shots , uint32_t sweeps , PS5000A_SIGGEN_TRIG_TYPE triggerType , PS5000A_SIGGEN_TRIG_SOURCE triggerSource , int16_t extInThreshold ); 
fcns.thunkname{fcnNum}='uint32int16doubledoubledoubledoublePS5000A_SWEEP_TYPEuint32uint32PS5000A_SIGGEN_TRIG_TYPEPS5000A_SIGGEN_TRIG_SOURCEint16Thunk';fcns.name{fcnNum}='ps5000aSetSigGenPropertiesBuiltIn'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'double', 'double', 'double', 'double', 'enPS5000ASweepType', 'uint32', 'uint32', 'enPS5000ASigGenTrigType', 'enPS5000ASigGenTrigSource', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSigGenSoftwareControl ( int16_t handle , int16_t state ); 
fcns.thunkname{fcnNum}='uint32int16int16Thunk';fcns.name{fcnNum}='ps5000aSigGenSoftwareControl'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetEts ( int16_t handle , PS5000A_ETS_MODE mode , int16_t etsCycles , int16_t etsInterleave , int32_t * sampleTimePicoseconds ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_ETS_MODEint16int16voidPtrThunk';fcns.name{fcnNum}='ps5000aSetEts'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000AEtsMode', 'int16', 'int16', 'int32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetTriggerChannelProperties ( int16_t handle , PS5000A_TRIGGER_CHANNEL_PROPERTIES * channelProperties , int16_t nChannelProperties , int16_t auxOutputEnable , int32_t autoTriggerMilliseconds ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrint16int16int32Thunk';fcns.name{fcnNum}='ps5000aSetTriggerChannelProperties'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'tPS5000ATriggerChannelPropertiesPtr', 'int16', 'int16', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetTriggerChannelConditions ( int16_t handle , PS5000A_TRIGGER_CONDITIONS * conditions , int16_t nConditions ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrint16Thunk';fcns.name{fcnNum}='ps5000aSetTriggerChannelConditions'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'tPS5000ATriggerConditionsPtr', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetTriggerChannelDirections ( int16_t handle , PS5000A_THRESHOLD_DIRECTION channelA , PS5000A_THRESHOLD_DIRECTION channelB , PS5000A_THRESHOLD_DIRECTION channelC , PS5000A_THRESHOLD_DIRECTION channelD , PS5000A_THRESHOLD_DIRECTION ext , PS5000A_THRESHOLD_DIRECTION aux ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_THRESHOLD_DIRECTIONPS5000A_THRESHOLD_DIRECTIONPS5000A_THRESHOLD_DIRECTIONPS5000A_THRESHOLD_DIRECTIONPS5000A_THRESHOLD_DIRECTIONPS5000A_THRESHOLD_DIRECTIONThunk';fcns.name{fcnNum}='ps5000aSetTriggerChannelDirections'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000AThresholdDirection', 'enPS5000AThresholdDirection', 'enPS5000AThresholdDirection', 'enPS5000AThresholdDirection', 'enPS5000AThresholdDirection', 'enPS5000AThresholdDirection'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetSimpleTrigger ( int16_t handle , int16_t enable , PS5000A_CHANNEL source , int16_t threshold , PS5000A_THRESHOLD_DIRECTION direction , uint32_t delay , int16_t autoTrigger_ms ); 
fcns.thunkname{fcnNum}='uint32int16int16PS5000A_CHANNELint16PS5000A_THRESHOLD_DIRECTIONuint32int16Thunk';fcns.name{fcnNum}='ps5000aSetSimpleTrigger'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16', 'enPS5000AChannel', 'int16', 'enPS5000AThresholdDirection', 'uint32', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetTriggerDelay ( int16_t handle , uint32_t delay ); 
fcns.thunkname{fcnNum}='uint32int16uint32Thunk';fcns.name{fcnNum}='ps5000aSetTriggerDelay'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetPulseWidthQualifier ( int16_t handle , PS5000A_PWQ_CONDITIONS * conditions , int16_t nConditions , PS5000A_THRESHOLD_DIRECTION direction , uint32_t lower , uint32_t upper , PS5000A_PULSE_WIDTH_TYPE type ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrint16PS5000A_THRESHOLD_DIRECTIONuint32uint32PS5000A_PULSE_WIDTH_TYPEThunk';fcns.name{fcnNum}='ps5000aSetPulseWidthQualifier'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'tPS5000APwqConditionsPtr', 'int16', 'enPS5000AThresholdDirection', 'uint32', 'uint32', 'enPS5000APulseWidthType'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aIsTriggerOrPulseWidthQualifierEnabled ( int16_t handle , int16_t * triggerEnabled , int16_t * pulseWidthQualifierEnabled ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrvoidPtrThunk';fcns.name{fcnNum}='ps5000aIsTriggerOrPulseWidthQualifierEnabled'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetTriggerTimeOffset ( int16_t handle , uint32_t * timeUpper , uint32_t * timeLower , PS5000A_TIME_UNITS * timeUnits , uint32_t segmentIndex ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrvoidPtrvoidPtruint32Thunk';fcns.name{fcnNum}='ps5000aGetTriggerTimeOffset'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint32Ptr', 'enPS5000ATimeUnitsPtr', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetTriggerTimeOffset64 ( int16_t handle , int64_t * time , PS5000A_TIME_UNITS * timeUnits , uint32_t segmentIndex ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrvoidPtruint32Thunk';fcns.name{fcnNum}='ps5000aGetTriggerTimeOffset64'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int64Ptr', 'enPS5000ATimeUnitsPtr', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetValuesTriggerTimeOffsetBulk ( int16_t handle , uint32_t * timesUpper , uint32_t * timesLower , uint32_t * timeUnits , uint32_t fromSegmentIndex , uint32_t toSegmentIndex ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrvoidPtrvoidPtruint32uint32Thunk';fcns.name{fcnNum}='ps5000aGetValuesTriggerTimeOffsetBulk'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint32Ptr', 'uint32Ptr', 'uint32', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetValuesTriggerTimeOffsetBulk64 ( int16_t handle , int64_t * times , uint32_t * timeUnits , uint32_t fromSegmentIndex , uint32_t toSegmentIndex ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrvoidPtruint32uint32Thunk';fcns.name{fcnNum}='ps5000aGetValuesTriggerTimeOffsetBulk64'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int64Ptr', 'uint32Ptr', 'uint32', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetDataBuffers ( int16_t handle , PS5000A_CHANNEL channel , int16_t * bufferMax , int16_t * bufferMin , int32_t bufferLth , uint32_t segmentIndex , PS5000A_RATIO_MODE mode ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_CHANNELvoidPtrvoidPtrint32uint32PS5000A_RATIO_MODEThunk';fcns.name{fcnNum}='ps5000aSetDataBuffers'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000AChannel', 'int16Ptr', 'int16Ptr', 'int32', 'uint32', 'enPS5000ARatioMode'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetDataBuffer ( int16_t handle , PS5000A_CHANNEL channel , int16_t * buffer , int32_t bufferLth , uint32_t segmentIndex , PS5000A_RATIO_MODE mode ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_CHANNELvoidPtrint32uint32PS5000A_RATIO_MODEThunk';fcns.name{fcnNum}='ps5000aSetDataBuffer'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000AChannel', 'int16Ptr', 'int32', 'uint32', 'enPS5000ARatioMode'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetEtsTimeBuffer ( int16_t handle , int64_t * buffer , int32_t bufferLth ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrint32Thunk';fcns.name{fcnNum}='ps5000aSetEtsTimeBuffer'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int64Ptr', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetEtsTimeBuffers ( int16_t handle , uint32_t * timeUpper , uint32_t * timeLower , int32_t bufferLth ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrvoidPtrint32Thunk';fcns.name{fcnNum}='ps5000aSetEtsTimeBuffers'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint32Ptr', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aIsReady ( int16_t handle , int16_t * ready ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrThunk';fcns.name{fcnNum}='ps5000aIsReady'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aRunBlock ( int16_t handle , int32_t noOfPreTriggerSamples , int32_t noOfPostTriggerSamples , uint32_t timebase , int32_t * timeIndisposedMs , uint32_t segmentIndex , ps5000aBlockReady lpReady , void * pParameter ); 
fcns.thunkname{fcnNum}='uint32int16int32int32uint32voidPtruint32voidPtrvoidPtrThunk';fcns.name{fcnNum}='ps5000aRunBlock'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int32', 'int32', 'uint32', 'int32Ptr', 'uint32', 'FcnPtr', 'voidPtr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aRunStreaming ( int16_t handle , uint32_t * sampleInterval , PS5000A_TIME_UNITS sampleIntervalTimeUnits , uint32_t maxPreTriggerSamples , uint32_t maxPostTriggerSamples , int16_t autoStop , uint32_t downSampleRatio , PS5000A_RATIO_MODE downSampleRatioMode , uint32_t overviewBufferSize ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrPS5000A_TIME_UNITSuint32uint32int16uint32PS5000A_RATIO_MODEuint32Thunk';fcns.name{fcnNum}='ps5000aRunStreaming'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'enPS5000ATimeUnits', 'uint32', 'uint32', 'int16', 'uint32', 'enPS5000ARatioMode', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetStreamingLatestValues ( int16_t handle , ps5000aStreamingReady lpPs5000aReady , void * pParameter ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrvoidPtrThunk';fcns.name{fcnNum}='ps5000aGetStreamingLatestValues'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'FcnPtr', 'voidPtr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aNoOfStreamingValues ( int16_t handle , uint32_t * noOfValues ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrThunk';fcns.name{fcnNum}='ps5000aNoOfStreamingValues'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetMaxDownSampleRatio ( int16_t handle , uint32_t noOfUnaggreatedSamples , uint32_t * maxDownSampleRatio , PS5000A_RATIO_MODE downSampleRatioMode , uint32_t segmentIndex ); 
fcns.thunkname{fcnNum}='uint32int16uint32voidPtrPS5000A_RATIO_MODEuint32Thunk';fcns.name{fcnNum}='ps5000aGetMaxDownSampleRatio'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'uint32Ptr', 'enPS5000ARatioMode', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetValues ( int16_t handle , uint32_t startIndex , uint32_t * noOfSamples , uint32_t downSampleRatio , PS5000A_RATIO_MODE downSampleRatioMode , uint32_t segmentIndex , int16_t * overflow ); 
fcns.thunkname{fcnNum}='uint32int16uint32voidPtruint32PS5000A_RATIO_MODEuint32voidPtrThunk';fcns.name{fcnNum}='ps5000aGetValues'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'uint32Ptr', 'uint32', 'enPS5000ARatioMode', 'uint32', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetValuesAsync ( int16_t handle , uint32_t startIndex , uint32_t noOfSamples , uint32_t downSampleRatio , PS5000A_RATIO_MODE downSampleRatioMode , uint32_t segmentIndex , void * lpDataReady , void * pParameter ); 
fcns.thunkname{fcnNum}='uint32int16uint32uint32uint32PS5000A_RATIO_MODEuint32voidPtrvoidPtrThunk';fcns.name{fcnNum}='ps5000aGetValuesAsync'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'uint32', 'uint32', 'enPS5000ARatioMode', 'uint32', 'voidPtr', 'voidPtr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetValuesBulk ( int16_t handle , uint32_t * noOfSamples , uint32_t fromSegmentIndex , uint32_t toSegmentIndex , uint32_t downSampleRatio , PS5000A_RATIO_MODE downSampleRatioMode , int16_t * overflow ); 
fcns.thunkname{fcnNum}='uint32int16voidPtruint32uint32uint32PS5000A_RATIO_MODEvoidPtrThunk';fcns.name{fcnNum}='ps5000aGetValuesBulk'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint32', 'uint32', 'uint32', 'enPS5000ARatioMode', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetValuesOverlapped ( int16_t handle , uint32_t startIndex , uint32_t * noOfSamples , uint32_t downSampleRatio , PS5000A_RATIO_MODE downSampleRatioMode , uint32_t segmentIndex , int16_t * overflow ); 
fcns.thunkname{fcnNum}='uint32int16uint32voidPtruint32PS5000A_RATIO_MODEuint32voidPtrThunk';fcns.name{fcnNum}='ps5000aGetValuesOverlapped'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'uint32Ptr', 'uint32', 'enPS5000ARatioMode', 'uint32', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetValuesOverlappedBulk ( int16_t handle , uint32_t startIndex , uint32_t * noOfSamples , uint32_t downSampleRatio , PS5000A_RATIO_MODE downSampleRatioMode , uint32_t fromSegmentIndex , uint32_t toSegmentIndex , int16_t * overflow ); 
fcns.thunkname{fcnNum}='uint32int16uint32voidPtruint32PS5000A_RATIO_MODEuint32uint32voidPtrThunk';fcns.name{fcnNum}='ps5000aGetValuesOverlappedBulk'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'uint32Ptr', 'uint32', 'enPS5000ARatioMode', 'uint32', 'uint32', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aTriggerWithinPreTriggerSamples ( int16_t handle , PS5000A_TRIGGER_WITHIN_PRE_TRIGGER state ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_TRIGGER_WITHIN_PRE_TRIGGERThunk';fcns.name{fcnNum}='ps5000aTriggerWithinPreTriggerSamples'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000ATriggerWithinPreTrigger'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetTriggerInfoBulk ( int16_t handle , PS5000A_TRIGGER_INFO * triggerInfo , uint32_t fromSegmentIndex , uint32_t toSegmentIndex ); 
fcns.thunkname{fcnNum}='uint32int16voidPtruint32uint32Thunk';fcns.name{fcnNum}='ps5000aGetTriggerInfoBulk'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'tPS5000ATriggerInfoPtr', 'uint32', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aEnumerateUnits ( int16_t * count , char * serials , int16_t * serialLth ); 
fcns.thunkname{fcnNum}='uint32voidPtrcstringvoidPtrThunk';fcns.name{fcnNum}='ps5000aEnumerateUnits'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'cstring', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetChannelInformation ( int16_t handle , PS5000A_CHANNEL_INFO info , int32_t probe , int32_t * ranges , int32_t * length , int32_t channels ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_CHANNEL_INFOint32voidPtrvoidPtrint32Thunk';fcns.name{fcnNum}='ps5000aGetChannelInformation'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000AChannelInfo', 'int32', 'int32Ptr', 'int32Ptr', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aMaximumValue ( int16_t handle , int16_t * value ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrThunk';fcns.name{fcnNum}='ps5000aMaximumValue'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aMinimumValue ( int16_t handle , int16_t * value ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrThunk';fcns.name{fcnNum}='ps5000aMinimumValue'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetAnalogueOffset ( int16_t handle , PS5000A_RANGE range , PS5000A_COUPLING coupling , float * maximumVoltage , float * minimumVoltage ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_RANGEPS5000A_COUPLINGvoidPtrvoidPtrThunk';fcns.name{fcnNum}='ps5000aGetAnalogueOffset'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000ARange', 'enPS5000ACoupling', 'singlePtr', 'singlePtr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetMaxSegments ( int16_t handle , uint32_t * maxSegments ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrThunk';fcns.name{fcnNum}='ps5000aGetMaxSegments'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aChangePowerSource ( int16_t handle , PICO_STATUS powerState ); 
fcns.thunkname{fcnNum}='uint32int16uint32Thunk';fcns.name{fcnNum}='ps5000aChangePowerSource'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aCurrentPowerSource ( int16_t handle ); 
fcns.thunkname{fcnNum}='uint32int16Thunk';fcns.name{fcnNum}='ps5000aCurrentPowerSource'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aStop ( int16_t handle ); 
fcns.thunkname{fcnNum}='uint32int16Thunk';fcns.name{fcnNum}='ps5000aStop'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aPingUnit ( int16_t handle ); 
fcns.thunkname{fcnNum}='uint32int16Thunk';fcns.name{fcnNum}='ps5000aPingUnit'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetNoOfCaptures ( int16_t handle , uint32_t nCaptures ); 
fcns.thunkname{fcnNum}='uint32int16uint32Thunk';fcns.name{fcnNum}='ps5000aSetNoOfCaptures'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetNoOfCaptures ( int16_t handle , uint32_t * nCaptures ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrThunk';fcns.name{fcnNum}='ps5000aGetNoOfCaptures'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetNoOfProcessedCaptures ( int16_t handle , uint32_t * nProcessedCaptures ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrThunk';fcns.name{fcnNum}='ps5000aGetNoOfProcessedCaptures'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aSetDeviceResolution ( int16_t handle , PS5000A_DEVICE_RESOLUTION resolution ); 
fcns.thunkname{fcnNum}='uint32int16PS5000A_DEVICE_RESOLUTIONThunk';fcns.name{fcnNum}='ps5000aSetDeviceResolution'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000ADeviceResolution'};fcnNum=fcnNum+1;
% PICO_STATUS ps5000aGetDeviceResolution ( int16_t handle , PS5000A_DEVICE_RESOLUTION * resolution ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrThunk';fcns.name{fcnNum}='ps5000aGetDeviceResolution'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS5000ADeviceResolutionPtr'};fcnNum=fcnNum+1;
structs.tPS5000ATriggerInfo.packing=1;
structs.tPS5000ATriggerInfo.members=struct('status', 'uint32', 'segmentIndex', 'uint32', 'triggerIndex', 'uint32', 'triggerTime', 'int64', 'timeUnits', 'int16', 'reserved0', 'int16', 'reserved1', 'uint64');
structs.tPS5000ATriggerConditions.packing=1;
structs.tPS5000ATriggerConditions.members=struct('channelA', 'enPS5000ATriggerState', 'channelB', 'enPS5000ATriggerState', 'channelC', 'enPS5000ATriggerState', 'channelD', 'enPS5000ATriggerState', 'external', 'enPS5000ATriggerState', 'aux', 'enPS5000ATriggerState', 'pulseWidthQualifier', 'enPS5000ATriggerState');
structs.tPS5000APwqConditions.packing=1;
structs.tPS5000APwqConditions.members=struct('channelA', 'enPS5000ATriggerState', 'channelB', 'enPS5000ATriggerState', 'channelC', 'enPS5000ATriggerState', 'channelD', 'enPS5000ATriggerState', 'external', 'enPS5000ATriggerState', 'aux', 'enPS5000ATriggerState');
structs.tPS5000ATriggerChannelProperties.packing=1;
structs.tPS5000ATriggerChannelProperties.members=struct('thresholdUpper', 'int16', 'thresholdUpperHysteresis', 'uint16', 'thresholdLower', 'int16', 'thresholdLowerHysteresis', 'uint16', 'channel', 'enPS5000AChannel', 'thresholdMode', 'enPS5000AThresholdMode');
enuminfo.enPS5000ARatioMode=struct('PS5000A_RATIO_MODE_NONE',0,'PS5000A_RATIO_MODE_AGGREGATE',1,'PS5000A_RATIO_MODE_DECIMATE',2,'PS5000A_RATIO_MODE_AVERAGE',4,'PS5000A_RATIO_MODE_DISTRIBUTION',8);
enuminfo.enPS5000AThresholdDirection=struct('PS5000A_ABOVE',0,'PS5000A_BELOW',1,'PS5000A_RISING',2,'PS5000A_FALLING',3,'PS5000A_RISING_OR_FALLING',4,'PS5000A_ABOVE_LOWER',5,'PS5000A_BELOW_LOWER',6,'PS5000A_RISING_LOWER',7,'PS5000A_FALLING_LOWER',8,'PS5000A_INSIDE',0,'PS5000A_OUTSIDE',1,'PS5000A_ENTER',2,'PS5000A_EXIT',3,'PS5000A_ENTER_OR_EXIT',4,'PS5000A_POSITIVE_RUNT',9,'PS5000A_NEGATIVE_RUNT',10,'PS5000A_NONE',2);
enuminfo.enPS5000AChannelInfo=struct('PS5000A_CI_RANGES',0);
enuminfo.enPS5000AThresholdMode=struct('PS5000A_LEVEL',0,'PS5000A_WINDOW',1);
enuminfo.enPS5000APulseWidthType=struct('PS5000A_PW_TYPE_NONE',0,'PS5000A_PW_TYPE_LESS_THAN',1,'PS5000A_PW_TYPE_GREATER_THAN',2,'PS5000A_PW_TYPE_IN_RANGE',3,'PS5000A_PW_TYPE_OUT_OF_RANGE',4);
enuminfo.enPS5000AChannelBufferIndex=struct('PS5000A_CHANNEL_A_MAX',0,'PS5000A_CHANNEL_A_MIN',1,'PS5000A_CHANNEL_B_MAX',2,'PS5000A_CHANNEL_B_MIN',3,'PS5000A_CHANNEL_C_MAX',4,'PS5000A_CHANNEL_C_MIN',5,'PS5000A_CHANNEL_D_MAX',6,'PS5000A_CHANNEL_D_MIN',7,'PS5000A_MAX_CHANNEL_BUFFERS',8);
enuminfo.enPS5000ATriggerWithinPreTrigger=struct('PS5000A_DISABLE',0,'PS5000A_ARM',1);
enuminfo.enPS5000ACoupling=struct('PS5000A_AC',0,'PS5000A_DC',1);
enuminfo.enPS5000AEtsMode=struct('PS5000A_ETS_OFF',0,'PS5000A_ETS_FAST',1,'PS5000A_ETS_SLOW',2,'PS5000A_ETS_MODES_MAX',3);
enuminfo.enPS5000ASweepType=struct('PS5000A_UP',0,'PS5000A_DOWN',1,'PS5000A_UPDOWN',2,'PS5000A_DOWNUP',3,'PS5000A_MAX_SWEEP_TYPES',4);
enuminfo.enPS5000ASigGenTrigType=struct('PS5000A_SIGGEN_RISING',0,'PS5000A_SIGGEN_FALLING',1,'PS5000A_SIGGEN_GATE_HIGH',2,'PS5000A_SIGGEN_GATE_LOW',3);
enuminfo.enPS5000AIndexMode=struct('PS5000A_SINGLE',0,'PS5000A_DUAL',1,'PS5000A_QUAD',2,'PS5000A_MAX_INDEX_MODES',3);
enuminfo.enPS5000ADeviceResolution=struct('PS5000A_DR_8BIT',0,'PS5000A_DR_12BIT',1,'PS5000A_DR_14BIT',2,'PS5000A_DR_15BIT',3,'PS5000A_DR_16BIT',4);
enuminfo.enPS5000AWaveType=struct('PS5000A_SINE',0,'PS5000A_SQUARE',1,'PS5000A_TRIANGLE',2,'PS5000A_RAMP_UP',3,'PS5000A_RAMP_DOWN',4,'PS5000A_SINC',5,'PS5000A_GAUSSIAN',6,'PS5000A_HALF_SINE',7,'PS5000A_DC_VOLTAGE',8,'PS5000A_WHITE_NOISE',9,'PS5000A_MAX_WAVE_TYPES',10);
enuminfo.enPS5000ABandwidthLimiter=struct('PS5000A_BW_FULL',0,'PS5000A_BW_20MHZ',1);
enuminfo.enPS5000ASigGenTrigSource=struct('PS5000A_SIGGEN_NONE',0,'PS5000A_SIGGEN_SCOPE_TRIG',1,'PS5000A_SIGGEN_AUX_IN',2,'PS5000A_SIGGEN_EXT_IN',3,'PS5000A_SIGGEN_SOFT_TRIG',4);
enuminfo.enPS5000AChannel=struct('PS5000A_CHANNEL_A',0,'PS5000A_CHANNEL_B',1,'PS5000A_CHANNEL_C',2,'PS5000A_CHANNEL_D',3,'PS5000A_EXTERNAL',4,'PS5000A_MAX_CHANNELS',4,'PS5000A_TRIGGER_AUX',5,'PS5000A_MAX_TRIGGER_SOURCES',6);
enuminfo.enPS5000AExtraOperations=struct('PS5000A_ES_OFF',0,'PS5000A_WHITENOISE',1,'PS5000A_PRBS',2);
enuminfo.enPS5000ATriggerState=struct('PS5000A_CONDITION_DONT_CARE',0,'PS5000A_CONDITION_TRUE',1,'PS5000A_CONDITION_FALSE',2,'PS5000A_CONDITION_MAX',3);
enuminfo.enPS5000ARange=struct('PS5000A_10MV',0,'PS5000A_20MV',1,'PS5000A_50MV',2,'PS5000A_100MV',3,'PS5000A_200MV',4,'PS5000A_500MV',5,'PS5000A_1V',6,'PS5000A_2V',7,'PS5000A_5V',8,'PS5000A_10V',9,'PS5000A_20V',10,'PS5000A_50V',11,'PS5000A_MAX_RANGES',12);
enuminfo.enPS5000ATimeUnits=struct('PS5000A_FS',0,'PS5000A_PS',1,'PS5000A_NS',2,'PS5000A_US',3,'PS5000A_MS',4,'PS5000A_S',5,'PS5000A_MAX_TIME_UNITS',6);
methodinfo=fcns;