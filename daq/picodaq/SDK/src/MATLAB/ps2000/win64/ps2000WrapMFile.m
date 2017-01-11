function [methodinfo,structs,enuminfo,ThunkLibName]=ps2000WrapMFile
%PS2000WRAPMFILE Create structures to define interfaces found in 'ps2000Wrap'.

%This function was generated by loadlibrary.m parser version  on Wed Nov 19 15:16:17 2014
%perl options:'ps2000Wrap.i -outfile=ps2000WrapMFile.m -thunkfile=ps2000Wrap_thunk_pcwin64.c -header=ps2000Wrap.h'
ival={cell(1,0)}; % change 0 to the actual number of functions to preallocate the data.
structs=[];enuminfo=[];fcnNum=1;
fcns=struct('name',ival,'calltype',ival,'LHS',ival,'RHS',ival,'alias',ival,'thunkname', ival);
MfilePath=fileparts(mfilename('fullpath'));
ThunkLibName=fullfile(MfilePath,'ps2000Wrap_thunk_pcwin64');
% extern int16_t __stdcall PollFastStreaming ( int16_t handle ); 
fcns.thunkname{fcnNum}='int16int16Thunk';fcns.name{fcnNum}='PollFastStreaming'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='int16'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% extern void __stdcall SetBuffer ( int16_t handle , int16_t channel , int16_t * buffer , uint32_t bufferSize ); 
fcns.thunkname{fcnNum}='voidint16int16voidPtruint32Thunk';fcns.name{fcnNum}='SetBuffer'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}=[]; fcns.RHS{fcnNum}={'int16', 'int16', 'int16Ptr', 'uint32'};fcnNum=fcnNum+1;
% extern void __stdcall SetAggregateBuffer ( int16_t handle , int16_t channel , int16_t * bufferMax , int16_t * bufferMin , uint32_t bufferSize ); 
fcns.thunkname{fcnNum}='voidint16int16voidPtrvoidPtruint32Thunk';fcns.name{fcnNum}='SetAggregateBuffer'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}=[]; fcns.RHS{fcnNum}={'int16', 'int16', 'int16Ptr', 'int16Ptr', 'uint32'};fcnNum=fcnNum+1;
% extern int16_t __stdcall FastStreamingReady ( int16_t handle ); 
fcns.thunkname{fcnNum}='int16int16Thunk';fcns.name{fcnNum}='FastStreamingReady'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='int16'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% extern uint32_t __stdcall GetFastStreamingDetails ( int16_t handle , int16_t * overflow , uint32_t * triggeredAt , int16_t * triggered , int16_t * auto_stop , int16_t * appBufferFull , uint32_t * startIndex ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrvoidPtrvoidPtrvoidPtrvoidPtrvoidPtrThunk';fcns.name{fcnNum}='GetFastStreamingDetails'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr', 'uint32Ptr', 'int16Ptr', 'int16Ptr', 'int16Ptr', 'uint32Ptr'};fcnNum=fcnNum+1;
% extern void __stdcall setEnabledChannels ( int16_t handle , int16_t * enabledChannels ); 
fcns.thunkname{fcnNum}='voidint16voidPtrThunk';fcns.name{fcnNum}='setEnabledChannels'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}=[]; fcns.RHS{fcnNum}={'int16', 'int16Ptr'};fcnNum=fcnNum+1;
% extern void __stdcall clearFastStreamingParameters ( int16_t handle ); 
fcns.thunkname{fcnNum}='voidint16Thunk';fcns.name{fcnNum}='clearFastStreamingParameters'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}=[]; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
structs.WRAP_BUFFER_INFO.members=struct('appBuffers', 'int16PtrPtr', 'bufferSizes', 'uint32#4');
methodinfo=fcns;