function [methodinfo,structs,enuminfo,ThunkLibName]=ps4000MFile
%PS4000MFILE Create structures to define interfaces found in 'ps4000Api'.

%This function was generated by loadlibrary.m parser version  on Wed Nov 12 15:27:17 2014
%perl options:'ps4000Api.i -outfile=ps4000MFile.m'
ival={cell(1,0)}; % change 0 to the actual number of functions to preallocate the data.
structs=[];enuminfo=[];fcnNum=1;
fcns=struct('name',ival,'calltype',ival,'LHS',ival,'RHS',ival,'alias',ival);
ThunkLibName=[];
% PICO_STATUS _stdcall ps4000OpenUnit ( int16_t * handle ); 
fcns.name{fcnNum}='ps4000OpenUnit'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000OpenUnitAsync ( int16_t * status ); 
fcns.name{fcnNum}='ps4000OpenUnitAsync'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000OpenUnitEx ( int16_t * handle , char * serial ); 
fcns.name{fcnNum}='ps4000OpenUnitEx'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'cstring'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000OpenUnitAsyncEx ( int16_t * status , char * serial ); 
fcns.name{fcnNum}='ps4000OpenUnitAsyncEx'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'cstring'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000OpenUnitProgress ( int16_t * handle , int16_t * progressPercent , int16_t * complete ); 
fcns.name{fcnNum}='ps4000OpenUnitProgress'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'int16Ptr', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetUnitInfo ( int16_t handle , char * string , int16_t stringLength , int16_t * requiredSize , PICO_INFO info ); 
fcns.name{fcnNum}='ps4000GetUnitInfo'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'cstring', 'int16', 'int16Ptr', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000FlashLed ( int16_t handle , int16_t start ); 
fcns.name{fcnNum}='ps4000FlashLed'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000IsLedFlashing ( int16_t handle , int16_t * status ); 
fcns.name{fcnNum}='ps4000IsLedFlashing'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000CloseUnit ( int16_t handle ); 
fcns.name{fcnNum}='ps4000CloseUnit'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000MemorySegments ( int16_t handle , uint16_t nSegments , int32_t * nMaxSamples ); 
fcns.name{fcnNum}='ps4000MemorySegments'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint16', 'int32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetChannel ( int16_t handle , PS4000_CHANNEL channel , int16_t enabled , int16_t dc , PS4000_RANGE range ); 
fcns.name{fcnNum}='ps4000SetChannel'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Channel', 'int16', 'int16', 'enPS4000Range'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetNoOfCaptures ( int16_t handle , uint16_t nCaptures ); 
fcns.name{fcnNum}='ps4000SetNoOfCaptures'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetTimebase ( int16_t handle , uint32_t timebase , int32_t noSamples , int32_t * timeIntervalNanoseconds , int16_t oversample , int32_t * maxSamples , uint16_t segmentIndex ); 
fcns.name{fcnNum}='ps4000GetTimebase'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'int32', 'int32Ptr', 'int16', 'int32Ptr', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetTimebase2 ( int16_t handle , uint32_t timebase , int32_t noSamples , float * timeIntervalNanoseconds , int16_t oversample , int32_t * maxSamples , uint16_t segmentIndex ); 
fcns.name{fcnNum}='ps4000GetTimebase2'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'int32', 'singlePtr', 'int16', 'int32Ptr', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetSigGenArbitrary ( int16_t handle , int32_t offsetVoltage , uint32_t pkToPk , uint32_t startDeltaPhase , uint32_t stopDeltaPhase , uint32_t deltaPhaseIncrement , uint32_t dwellCount , int16_t * arbitraryWaveform , int32_t arbitraryWaveformSize , SWEEP_TYPE sweepType , int16_t operationType , INDEX_MODE indexMode , uint32_t shots , uint32_t sweeps , SIGGEN_TRIG_TYPE triggerType , SIGGEN_TRIG_SOURCE triggerSource , int16_t extInThreshold ); 
fcns.name{fcnNum}='ps4000SetSigGenArbitrary'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int32', 'uint32', 'uint32', 'uint32', 'uint32', 'uint32', 'int16Ptr', 'int32', 'enSweepType', 'int16', 'enIndexMode', 'uint32', 'uint32', 'enSigGenTrigType', 'enSigGenTrigSource', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetSigGenBuiltIn ( int16_t handle , int32_t offsetVoltage , uint32_t pkToPk , int16_t waveType , float startFrequency , float stopFrequency , float increment , float dwellTime , SWEEP_TYPE sweepType , int16_t operationType , uint32_t shots , uint32_t sweeps , SIGGEN_TRIG_TYPE triggerType , SIGGEN_TRIG_SOURCE triggerSource , int16_t extInThreshold ); 
fcns.name{fcnNum}='ps4000SetSigGenBuiltIn'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int32', 'uint32', 'int16', 'single', 'single', 'single', 'single', 'enSweepType', 'int16', 'uint32', 'uint32', 'enSigGenTrigType', 'enSigGenTrigSource', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SigGenFrequencyToPhase ( int16_t handle , double frequency , INDEX_MODE indexMode , uint32_t bufferLength , uint32_t * phase ); 
fcns.name{fcnNum}='ps4000SigGenFrequencyToPhase'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'double', 'enIndexMode', 'uint32', 'uint32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SigGenArbitraryMinMaxValues ( int16_t handle , int16_t * minArbitraryWaveformValue , int16_t * maxArbitraryWaveformValue , uint32_t * minArbitraryWaveformSize , uint32_t * maxArbitraryWaveformSize ); 
fcns.name{fcnNum}='ps4000SigGenArbitraryMinMaxValues'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr', 'int16Ptr', 'uint32Ptr', 'uint32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SigGenSoftwareControl ( int16_t handle , int16_t state ); 
fcns.name{fcnNum}='ps4000SigGenSoftwareControl'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetEts ( int16_t handle , PS4000_ETS_MODE mode , int16_t etsCycles , int16_t etsInterleave , int32_t * sampleTimePicoseconds ); 
fcns.name{fcnNum}='ps4000SetEts'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000EtsMode', 'int16', 'int16', 'int32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetSimpleTrigger ( int16_t handle , int16_t enable , PS4000_CHANNEL source , int16_t threshold , THRESHOLD_DIRECTION direction , uint32_t delay , int16_t autoTrigger_ms ); 
fcns.name{fcnNum}='ps4000SetSimpleTrigger'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16', 'enPS4000Channel', 'int16', 'enThresholdDirection', 'uint32', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetTriggerChannelProperties ( int16_t handle , TRIGGER_CHANNEL_PROPERTIES * channelProperties , int16_t nChannelProperties , int16_t auxOutputEnable , int32_t autoTriggerMilliseconds ); 
fcns.name{fcnNum}='ps4000SetTriggerChannelProperties'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'tTriggerChannelPropertiesPtr', 'int16', 'int16', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetExtTriggerRange ( int16_t handle , PS4000_RANGE extRange ); 
fcns.name{fcnNum}='ps4000SetExtTriggerRange'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Range'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetTriggerChannelConditions ( int16_t handle , TRIGGER_CONDITIONS * conditions , int16_t nConditions ); 
fcns.name{fcnNum}='ps4000SetTriggerChannelConditions'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'tTriggerConditionsPtr', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetTriggerChannelDirections ( int16_t handle , THRESHOLD_DIRECTION channelA , THRESHOLD_DIRECTION channelB , THRESHOLD_DIRECTION channelC , THRESHOLD_DIRECTION channelD , THRESHOLD_DIRECTION ext , THRESHOLD_DIRECTION aux ); 
fcns.name{fcnNum}='ps4000SetTriggerChannelDirections'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enThresholdDirection', 'enThresholdDirection', 'enThresholdDirection', 'enThresholdDirection', 'enThresholdDirection', 'enThresholdDirection'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetTriggerDelay ( int16_t handle , uint32_t delay ); 
fcns.name{fcnNum}='ps4000SetTriggerDelay'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetPulseWidthQualifier ( int16_t handle , PWQ_CONDITIONS * conditions , int16_t nConditions , THRESHOLD_DIRECTION direction , uint32_t lower , uint32_t upper , PULSE_WIDTH_TYPE type ); 
fcns.name{fcnNum}='ps4000SetPulseWidthQualifier'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'tPwqConditionsPtr', 'int16', 'enThresholdDirection', 'uint32', 'uint32', 'enPulseWidthType'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000IsTriggerOrPulseWidthQualifierEnabled ( int16_t handle , int16_t * triggerEnabled , int16_t * pulseWidthQualifierEnabled ); 
fcns.name{fcnNum}='ps4000IsTriggerOrPulseWidthQualifierEnabled'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetTriggerTimeOffset ( int16_t handle , uint32_t * timeUpper , uint32_t * timeLower , PS4000_TIME_UNITS * timeUnits , uint16_t segmentIndex ); 
fcns.name{fcnNum}='ps4000GetTriggerTimeOffset'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint32Ptr', 'enPS4000TimeUnitsPtr', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetTriggerChannelTimeOffset ( int16_t handle , uint32_t * timeUpper , uint32_t * timeLower , PS4000_TIME_UNITS * timeUnits , uint16_t segmentIndex , PS4000_CHANNEL channel ); 
fcns.name{fcnNum}='ps4000GetTriggerChannelTimeOffset'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint32Ptr', 'enPS4000TimeUnitsPtr', 'uint16', 'enPS4000Channel'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetTriggerTimeOffset64 ( int16_t handle , int64_t * time , PS4000_TIME_UNITS * timeUnits , uint16_t segmentIndex ); 
fcns.name{fcnNum}='ps4000GetTriggerTimeOffset64'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int64Ptr', 'enPS4000TimeUnitsPtr', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetTriggerChannelTimeOffset64 ( int16_t handle , int64_t * time , PS4000_TIME_UNITS * timeUnits , uint16_t segmentIndex , PS4000_CHANNEL channel ); 
fcns.name{fcnNum}='ps4000GetTriggerChannelTimeOffset64'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int64Ptr', 'enPS4000TimeUnitsPtr', 'uint16', 'enPS4000Channel'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetValuesTriggerTimeOffsetBulk ( int16_t handle , uint32_t * timesUpper , uint32_t * timesLower , PS4000_TIME_UNITS * timeUnits , uint16_t fromSegmentIndex , uint16_t toSegmentIndex ); 
fcns.name{fcnNum}='ps4000GetValuesTriggerTimeOffsetBulk'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint32Ptr', 'enPS4000TimeUnitsPtr', 'uint16', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetValuesTriggerChannelTimeOffsetBulk ( int16_t handle , uint32_t * timesUpper , uint32_t * timesLower , PS4000_TIME_UNITS * timeUnits , uint16_t fromSegmentIndex , uint16_t toSegmentIndex , PS4000_CHANNEL channel ); 
fcns.name{fcnNum}='ps4000GetValuesTriggerChannelTimeOffsetBulk'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint32Ptr', 'enPS4000TimeUnitsPtr', 'uint16', 'uint16', 'enPS4000Channel'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetValuesTriggerTimeOffsetBulk64 ( int16_t handle , int64_t * times , PS4000_TIME_UNITS * timeUnits , uint16_t fromSegmentIndex , uint16_t toSegmentIndex ); 
fcns.name{fcnNum}='ps4000GetValuesTriggerTimeOffsetBulk64'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int64Ptr', 'enPS4000TimeUnitsPtr', 'uint16', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetValuesTriggerChannelTimeOffsetBulk64 ( int16_t handle , int64_t * times , PS4000_TIME_UNITS * timeUnits , uint16_t fromSegmentIndex , uint16_t toSegmentIndex , PS4000_CHANNEL channel ); 
fcns.name{fcnNum}='ps4000GetValuesTriggerChannelTimeOffsetBulk64'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int64Ptr', 'enPS4000TimeUnitsPtr', 'uint16', 'uint16', 'enPS4000Channel'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetDataBufferBulk ( int16_t handle , PS4000_CHANNEL channel , int16_t * buffer , int32_t bufferLth , uint16_t waveform ); 
fcns.name{fcnNum}='ps4000SetDataBufferBulk'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Channel', 'int16Ptr', 'int32', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetDataBuffers ( int16_t handle , PS4000_CHANNEL channel , int16_t * bufferMax , int16_t * bufferMin , int32_t bufferLth ); 
fcns.name{fcnNum}='ps4000SetDataBuffers'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Channel', 'int16Ptr', 'int16Ptr', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetDataBufferWithMode ( int16_t handle , PS4000_CHANNEL channel , int16_t * buffer , int32_t bufferLth , RATIO_MODE mode ); 
fcns.name{fcnNum}='ps4000SetDataBufferWithMode'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Channel', 'int16Ptr', 'int32', 'enRatioMode'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetDataBuffersWithMode ( int16_t handle , PS4000_CHANNEL channel , int16_t * bufferMax , int16_t * bufferMin , int32_t bufferLth , RATIO_MODE mode ); 
fcns.name{fcnNum}='ps4000SetDataBuffersWithMode'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Channel', 'int16Ptr', 'int16Ptr', 'int32', 'enRatioMode'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetDataBuffer ( int16_t handle , PS4000_CHANNEL channel , int16_t * buffer , int32_t bufferLth ); 
fcns.name{fcnNum}='ps4000SetDataBuffer'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Channel', 'int16Ptr', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetEtsTimeBuffer ( int16_t handle , int64_t * buffer , int32_t bufferLth ); 
fcns.name{fcnNum}='ps4000SetEtsTimeBuffer'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int64Ptr', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetEtsTimeBuffers ( int16_t handle , uint32_t * timeUpper , uint32_t * timeLower , int32_t bufferLth ); 
fcns.name{fcnNum}='ps4000SetEtsTimeBuffers'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint32Ptr', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000RunBlock ( int16_t handle , int32_t noOfPreTriggerSamples , int32_t noOfPostTriggerSamples , uint32_t timebase , int16_t oversample , int32_t * timeIndisposedMs , uint16_t segmentIndex , ps4000BlockReady lpReady , void * pParameter ); 
fcns.name{fcnNum}='ps4000RunBlock'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int32', 'int32', 'uint32', 'int16', 'int32Ptr', 'uint16', 'voidPtr', 'voidPtr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000RunStreaming ( int16_t handle , uint32_t * sampleInterval , PS4000_TIME_UNITS sampleIntervalTimeUnits , uint32_t maxPreTriggerSamples , uint32_t maxPostPreTriggerSamples , int16_t autoStop , uint32_t downSampleRatio , uint32_t overviewBufferSize ); 
fcns.name{fcnNum}='ps4000RunStreaming'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'enPS4000TimeUnits', 'uint32', 'uint32', 'int16', 'uint32', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000RunStreamingEx ( int16_t handle , uint32_t * sampleInterval , PS4000_TIME_UNITS sampleIntervalTimeUnits , uint32_t maxPreTriggerSamples , uint32_t maxPostPreTriggerSamples , int16_t autoStop , uint32_t downSampleRatio , int16_t downSampleRatioMode , uint32_t overviewBufferSize ); 
fcns.name{fcnNum}='ps4000RunStreamingEx'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'enPS4000TimeUnits', 'uint32', 'uint32', 'int16', 'uint32', 'int16', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000IsReady ( int16_t handle , int16_t * ready ); 
fcns.name{fcnNum}='ps4000IsReady'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetStreamingLatestValues ( int16_t handle , ps4000StreamingReady lpPs4000Ready , void * pParameter ); 
fcns.name{fcnNum}='ps4000GetStreamingLatestValues'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'voidPtr', 'voidPtr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000NoOfStreamingValues ( int16_t handle , uint32_t * noOfValues ); 
fcns.name{fcnNum}='ps4000NoOfStreamingValues'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetMaxDownSampleRatio ( int16_t handle , uint32_t noOfUnaggreatedSamples , uint32_t * maxDownSampleRatio , int16_t downSampleRatioMode , uint16_t segmentIndex ); 
fcns.name{fcnNum}='ps4000GetMaxDownSampleRatio'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'uint32Ptr', 'int16', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetValues ( int16_t handle , uint32_t startIndex , uint32_t * noOfSamples , uint32_t downSampleRatio , int16_t downSampleRatioMode , uint16_t segmentIndex , int16_t * overflow ); 
fcns.name{fcnNum}='ps4000GetValues'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'uint32Ptr', 'uint32', 'int16', 'uint16', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetValuesBulk ( int16_t handle , uint32_t * noOfSamples , uint16_t fromSegmentIndex , uint16_t toSegmentIndex , int16_t * overflow ); 
fcns.name{fcnNum}='ps4000GetValuesBulk'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32Ptr', 'uint16', 'uint16', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetValuesAsync ( int16_t handle , uint32_t startIndex , uint32_t noOfSamples , uint32_t downSampleRatio , int16_t downSampleRatioMode , uint16_t segmentIndex , void * lpDataReady , void * pParameter ); 
fcns.name{fcnNum}='ps4000GetValuesAsync'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint32', 'uint32', 'uint32', 'int16', 'uint16', 'voidPtr', 'voidPtr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000Stop ( int16_t handle ); 
fcns.name{fcnNum}='ps4000Stop'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetProbe ( int16_t handle , PS4000_PROBE probe , PS4000_RANGE range ); 
fcns.name{fcnNum}='ps4000SetProbe'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Probe', 'enPS4000Range'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000HoldOff ( int16_t handle , uint64_t holdoff , PS4000_HOLDOFF_TYPE type ); 
fcns.name{fcnNum}='ps4000HoldOff'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint64', 'enPs4000HoldOffType'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetProbe ( int16_t handle , PS4000_PROBE * probe ); 
fcns.name{fcnNum}='ps4000GetProbe'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000ProbePtr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetChannelInformation ( int16_t handle , PS4000_CHANNEL_INFO info , int32_t probe , int32_t * ranges , int32_t * length , int32_t channels ); 
fcns.name{fcnNum}='ps4000GetChannelInformation'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000ChannelInfo', 'int32', 'int32Ptr', 'int32Ptr', 'int32'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetFrequencyCounter ( int16_t handle , PS4000_CHANNEL channel , int16_t enabled , PS4000_FREQUENCY_COUNTER_RANGE range , int16_t thresholdMajor , int16_t thresholdMinor ); 
fcns.name{fcnNum}='ps4000SetFrequencyCounter'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Channel', 'int16', 'enPS4000FrequencyCounterRange', 'int16', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000EnumerateUnits ( int16_t * count , char * serials , int16_t * serialLth ); 
fcns.name{fcnNum}='ps4000EnumerateUnits'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'cstring', 'int16Ptr'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000PingUnit ( int16_t handle ); 
fcns.name{fcnNum}='ps4000PingUnit'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000SetBwFilter ( int16_t handle , PS4000_CHANNEL channel , int16_t enable ); 
fcns.name{fcnNum}='ps4000SetBwFilter'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPS4000Channel', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000TriggerWithinPreTriggerSamples ( int16_t handle , int16_t state ); 
fcns.name{fcnNum}='ps4000TriggerWithinPreTriggerSamples'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16'};fcnNum=fcnNum+1;
% PICO_STATUS _stdcall ps4000GetNoOfCaptures ( int16_t handle , uint16_t * nCaptures ); 
fcns.name{fcnNum}='ps4000GetNoOfCaptures'; fcns.calltype{fcnNum}='stdcall'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint16Ptr'};fcnNum=fcnNum+1;
structs.tTriggerConditions.packing=1;
structs.tTriggerConditions.members=struct('channelA', 'enTriggerState', 'channelB', 'enTriggerState', 'channelC', 'enTriggerState', 'channelD', 'enTriggerState', 'external', 'enTriggerState', 'aux', 'enTriggerState', 'pulseWidthQualifier', 'enTriggerState');
structs.tPwqConditions.packing=1;
structs.tPwqConditions.members=struct('channelA', 'enTriggerState', 'channelB', 'enTriggerState', 'channelC', 'enTriggerState', 'channelD', 'enTriggerState', 'external', 'enTriggerState', 'aux', 'enTriggerState');
structs.tTriggerChannelProperties.packing=1;
structs.tTriggerChannelProperties.members=struct('thresholdUpper', 'int16', 'thresholdUpperHysteresis', 'uint16', 'thresholdLower', 'int16', 'thresholdLowerHysteresis', 'uint16', 'channel', 'enPS4000Channel', 'thresholdMode', 'enThresholdMode');
enuminfo.enThresholdDirection=struct('ABOVE',0,'BELOW',1,'RISING',2,'FALLING',3,'RISING_OR_FALLING',4,'ABOVE_LOWER',5,'BELOW_LOWER',6,'RISING_LOWER',7,'FALLING_LOWER',8,'INSIDE',0,'OUTSIDE',1,'ENTER',2,'EXIT',3,'ENTER_OR_EXIT',4,'POSITIVE_RUNT',9,'NEGATIVE_RUNT',10,'NONE',2);
enuminfo.enPs4000HoldOffType=struct('PS4000_TIME',0,'PS4000_MAX_HOLDOFF_TYPE',1);
enuminfo.enPS4000Range=struct('PS4000_10MV',0,'PS4000_20MV',1,'PS4000_50MV',2,'PS4000_100MV',3,'PS4000_200MV',4,'PS4000_500MV',5,'PS4000_1V',6,'PS4000_2V',7,'PS4000_5V',8,'PS4000_10V',9,'PS4000_20V',10,'PS4000_50V',11,'PS4000_100V',12,'PS4000_MAX_RANGES',13,'PS4000_RESISTANCE_100R',13,'PS4000_RESISTANCE_1K',14,'PS4000_RESISTANCE_10K',15,'PS4000_RESISTANCE_100K',16,'PS4000_RESISTANCE_1M',17,'PS4000_MAX_RESISTANCES',18,'PS4000_ACCELEROMETER_10MV',18,'PS4000_ACCELEROMETER_20MV',19,'PS4000_ACCELEROMETER_50MV',20,'PS4000_ACCELEROMETER_100MV',21,'PS4000_ACCELEROMETER_200MV',22,'PS4000_ACCELEROMETER_500MV',23,'PS4000_ACCELEROMETER_1V',24,'PS4000_ACCELEROMETER_2V',25,'PS4000_ACCELEROMETER_5V',26,'PS4000_ACCELEROMETER_10V',27,'PS4000_ACCELEROMETER_20V',28,'PS4000_ACCELEROMETER_50V',29,'PS4000_ACCELEROMETER_100V',30,'PS4000_MAX_ACCELEROMETER',31,'PS4000_TEMPERATURE_UPTO_40',31,'PS4000_TEMPERATURE_UPTO_70',32,'PS4000_TEMPERATURE_UPTO_100',33,'PS4000_TEMPERATURE_UPTO_130',34,'PS4000_MAX_TEMPERATURES',35,'PS4000_RESISTANCE_5K',35,'PS4000_RESISTANCE_25K',36,'PS4000_RESISTANCE_50K',37,'PS4000_MAX_EXTRA_RESISTANCES',38);
enuminfo.enPS4000ChannelInfo=struct('CI_RANGES',0,'CI_RESISTANCES',1,'CI_ACCELEROMETER',2,'CI_PROBES',3,'CI_TEMPERATURES',4);
enuminfo.enThresholdMode=struct('LEVEL',0,'WINDOW',1);
enuminfo.enRatioMode=struct('RATIO_MODE_NONE',0,'RATIO_MODE_AGGREGATE',1,'RATIO_MODE_AVERAGE',2);
enuminfo.enSweepType=struct('UP',0,'DOWN',1,'UPDOWN',2,'DOWNUP',3,'MAX_SWEEP_TYPES',4);
enuminfo.enPS4000EtsMode=struct('PS4000_ETS_OFF',0,'PS4000_ETS_FAST',1,'PS4000_ETS_SLOW',2,'PS4000_ETS_MODES_MAX',3);
enuminfo.enPS4000FrequencyCounterRange=struct('FC_2K',0,'FC_20K',1,'FC_20',2,'FC_200',3,'FC_MAX',4);
enuminfo.enSigGenTrigType=struct('SIGGEN_RISING',0,'SIGGEN_FALLING',1,'SIGGEN_GATE_HIGH',2,'SIGGEN_GATE_LOW',3);
enuminfo.enChannelBufferIndex=struct('PS4000_CHANNEL_A_MAX',0,'PS4000_CHANNEL_A_MIN',1,'PS4000_CHANNEL_B_MAX',2,'PS4000_CHANNEL_B_MIN',3,'PS4000_CHANNEL_C_MAX',4,'PS4000_CHANNEL_C_MIN',5,'PS4000_CHANNEL_D_MAX',6,'PS4000_CHANNEL_D_MIN',7,'PS4000_MAX_CHANNEL_BUFFERS',8);
enuminfo.enPS4000Probe=struct('P_NONE',0,'P_CURRENT_CLAMP_10A',1,'P_CURRENT_CLAMP_1000A',2,'P_TEMPERATURE_SENSOR',3,'P_CURRENT_MEASURING_DEVICE',4,'P_PRESSURE_SENSOR_50BAR',5,'P_PRESSURE_SENSOR_5BAR',6,'P_OPTICAL_SWITCH',7,'P_UNKNOWN',8,'P_MAX_PROBES',8);
enuminfo.enSigGenTrigSource=struct('SIGGEN_NONE',0,'SIGGEN_SCOPE_TRIG',1,'SIGGEN_AUX_IN',2,'SIGGEN_EXT_IN',3,'SIGGEN_SOFT_TRIG',4);
enuminfo.enIndexMode=struct('SINGLE',0,'DUAL',1,'QUAD',2,'MAX_INDEX_MODES',3);
enuminfo.enTriggerState=struct('CONDITION_DONT_CARE',0,'CONDITION_TRUE',1,'CONDITION_FALSE',2,'CONDITION_MAX',3);
enuminfo.enPS4000OperationTypes=struct('PS4000_OP_NONE',0,'PS4000_WHITENOISE',1,'PS4000_PRBS',2);
enuminfo.enPS4000Channel=struct('PS4000_CHANNEL_A',0,'PS4000_CHANNEL_B',1,'PS4000_CHANNEL_C',2,'PS4000_CHANNEL_D',3,'PS4000_EXTERNAL',4,'PS4000_MAX_CHANNELS',4,'PS4000_TRIGGER_AUX',5,'PS4000_MAX_TRIGGER_SOURCES',6);
enuminfo.enPulseWidthType=struct('PW_TYPE_NONE',0,'PW_TYPE_LESS_THAN',1,'PW_TYPE_GREATER_THAN',2,'PW_TYPE_IN_RANGE',3,'PW_TYPE_OUT_OF_RANGE',4);
enuminfo.enPS4000TimeUnits=struct('PS4000_FS',0,'PS4000_PS',1,'PS4000_NS',2,'PS4000_US',3,'PS4000_MS',4,'PS4000_S',5,'PS4000_MAX_TIME_UNITS',6);
enuminfo.enWaveType=struct('PS4000_SINE',0,'PS4000_SQUARE',1,'PS4000_TRIANGLE',2,'PS4000_RAMP_UP',3,'PS4000_RAMP_DOWN',4,'PS4000_SINC',5,'PS4000_GAUSSIAN',6,'PS4000_HALF_SINE',7,'PS4000_DC_VOLTAGE',8,'PS4000_WHITE_NOISE',9,'MAX_WAVE_TYPES',10);
methodinfo=fcns;