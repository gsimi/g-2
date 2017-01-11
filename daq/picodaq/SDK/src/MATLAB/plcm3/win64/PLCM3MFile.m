function [methodinfo,structs,enuminfo,ThunkLibName]=PLCM3MFile
%PLCM3MFILE Create structures to define interfaces found in 'PLCM3Api'.

%This function was generated by loadlibrary.m parser version 1.1.6.37 on Wed Jun 25 12:18:55 2014
%perl options:'PLCM3Api.i -outfile=PLCM3MFile.m -thunkfile=PLCM3_thunk_pcwin64.c -header=PLCM3Api.h'
ival={cell(1,0)}; % change 0 to the actual number of functions to preallocate the data.
structs=[];enuminfo=[];fcnNum=1;
fcns=struct('name',ival,'calltype',ival,'LHS',ival,'RHS',ival,'alias',ival,'thunkname', ival);
MfilePath=fileparts(mfilename('fullpath'));
ThunkLibName=fullfile(MfilePath,'PLCM3_thunk_pcwin64');
% PICO_STATUS PLCM3Enumerate ( char * details , unsigned long * length , PLCM3_COMMUNICATION_TYPE type ); 
fcns.thunkname{fcnNum}='uint32cstringvoidPtrPLCM3_COMMUNICATION_TYPEThunk';fcns.name{fcnNum}='PLCM3Enumerate'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'cstring', 'ulongPtr', 'enPLCM3CommunicationType'};fcnNum=fcnNum+1;
% PICO_STATUS PLCM3OpenUnit ( short * handle , char * serial ); 
fcns.thunkname{fcnNum}='uint32voidPtrcstringThunk';fcns.name{fcnNum}='PLCM3OpenUnit'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'cstring'};fcnNum=fcnNum+1;
% PICO_STATUS PLCM3OpenUnitViaIp ( short * handle , char * serial , char * ipAddress ); 
fcns.thunkname{fcnNum}='uint32voidPtrcstringcstringThunk';fcns.name{fcnNum}='PLCM3OpenUnitViaIp'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16Ptr', 'cstring', 'cstring'};fcnNum=fcnNum+1;
% PICO_STATUS PLCM3CloseUnit ( short handle ); 
fcns.thunkname{fcnNum}='uint32int16Thunk';fcns.name{fcnNum}='PLCM3CloseUnit'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16'};fcnNum=fcnNum+1;
% PICO_STATUS PLCM3IpDetails ( short handle , short * enabled , char * ipaddress , unsigned short * length , unsigned short * listeningPort , PLCM3_IP_DETAILS_TYPE type ); 
fcns.thunkname{fcnNum}='uint32int16voidPtrcstringvoidPtrvoidPtrPLCM3_IP_DETAILS_TYPEThunk';fcns.name{fcnNum}='PLCM3IpDetails'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'int16Ptr', 'cstring', 'uint16Ptr', 'uint16Ptr', 'enPLCM3IpDetailsType'};fcnNum=fcnNum+1;
% PICO_STATUS PLCM3GetUnitInfo ( short handle , char * string , short stringLength , short * requiredSize , PICO_INFO info ); 
fcns.thunkname{fcnNum}='uint32int16cstringint16voidPtruint32Thunk';fcns.name{fcnNum}='PLCM3GetUnitInfo'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'cstring', 'int16', 'int16Ptr', 'uint32'};fcnNum=fcnNum+1;
% PICO_STATUS PLCM3SetChannel ( short handle , PLCM3_CHANNELS channel , PLCM3_DATA_TYPES type ); 
fcns.thunkname{fcnNum}='uint32int16PLCM3_CHANNELSPLCM3_DATA_TYPESThunk';fcns.name{fcnNum}='PLCM3SetChannel'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPLCM3Channels', 'enPLCM3DataType'};fcnNum=fcnNum+1;
% PICO_STATUS PLCM3SetMains ( short handle , unsigned short sixtyHertz ); 
fcns.thunkname{fcnNum}='uint32int16uint16Thunk';fcns.name{fcnNum}='PLCM3SetMains'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'uint16'};fcnNum=fcnNum+1;
% PICO_STATUS PLCM3GetValue ( short handle , PLCM3_CHANNELS channel , long * value ); 
fcns.thunkname{fcnNum}='uint32int16PLCM3_CHANNELSvoidPtrThunk';fcns.name{fcnNum}='PLCM3GetValue'; fcns.calltype{fcnNum}='Thunk'; fcns.LHS{fcnNum}='uint32'; fcns.RHS{fcnNum}={'int16', 'enPLCM3Channels', 'longPtr'};fcnNum=fcnNum+1;
enuminfo.enPLCM3CommunicationType=struct('PLCM3_CT_USB',1,'PLCM3_CT_ETHERNET',2,'PLCM3_CT_ALL',4294967295);
enuminfo.enPLCM3Channels=struct('PLCM3_CHANNEL_1',1,'PLCM3_CHANNEL_2',2,'PLCM3_CHANNEL_3',3,'PLCM3_MAX_CHANNELS',3);
enuminfo.enPLCM3IpDetailsType=struct('PLCM3_IDT_GET',0,'PLCM3_IDT_SET',1);
enuminfo.enPLCM3DataType=struct('PLCM3_OFF',0,'PLCM3_1_MILLIVOLT',1,'PLCM3_10_MILLIVOLTS',2,'PLCM3_100_MILLIVOLTS',3,'PLCM3_VOLTAGE',4,'PLCM3_MAX_DATA_TYPES',5);
methodinfo=fcns;