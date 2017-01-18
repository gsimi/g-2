/*
 * DeviceManager.h
 *
 *  Created on: Dec 22, 2016
 *      Author: carlo
 */

#ifndef DEVICEMANAGER_H_
#define DEVICEMANAGER_H_

#include "utilities.h"
#include "InfoAcq.h"
#include "Event.h"
#include "TFile.h"
#include "TTree.h"

class DeviceManager {

public:
	typedef struct UNIT
	{
		int16_t handle;
		MODEL_TYPE					model;
		int8_t						modelString[8];
		int8_t						serial[10];
		int16_t						complete;
		int16_t						openStatus;
		int16_t						openProgress;
		PS5000A_RANGE				firstRange;
		PS5000A_RANGE				lastRange;
		int16_t						channelCount;
		int16_t						maxADCValue;
		SIGGEN_TYPE					sigGen;
		int16_t						hasHardwareETS;
		uint16_t					awgBufferSize;
		CHANNEL_SETTINGS			channelSettings [PS5000A_MAX_CHANNELS];
		PS5000A_DEVICE_RESOLUTION	resolution;
	} UNIT;

	struct SETTINGS {
		CHANNEL_SETTINGS channelSettings[2];
		TRIGGER_SETTINGS triggerSettings[2];
		int32_t no_segments;
		int32_t no_waveforms;
		uint32_t timebase;
		int32_t nSamplesDIV;
		int32_t samplesStored;
		int32_t preTrig;
		float timeIntervalNanoseconds;
		uint64_t durata_s;
		uint64_t nMaxSignals;
	};

public:
	DeviceManager(const char* config_file);
	virtual ~DeviceManager();

	/*FUNZIONI PER LA GESTIONE DEL DEVICE*/
	void openDevice();
	void closeDevice();
	void set_info();
	PICO_STATUS changePowerSource(PICO_STATUS status);
	PICO_STATUS setChannels();
	PICO_STATUS setChannelsFromFile(const char* config_file);
	PICO_STATUS setSimpleTrigger(int16_t * trigger,PS5000A_THRESHOLD_DIRECTION * triggerDirections,float * triggerThresholdMV,int16_t ntriggering);

	void Run();

	/*UTILITA'*/
	int8_t YN_check();

	/*FUNZIONI PER RIEMPIRE LE STRUCT*/
	void FillStructsInfoAcq();
	void PrintInfo() {info->PrintInfo();};

	/*FUNZIONI CHE RESTITUISCONO I MEMBRI PRIVATE*/
	UNIT* Get_ptrToUnit() {return unit;};
	SETTINGS* Get_ptrToSettings() {return s_info;};
	int32_t Get_samplesStored() {return s_info->samplesStored;};

private:

	UNIT* unit;
	SETTINGS* s_info;
	InfoAcq* info;
	Event* evt;

	TFile* file;
	TTree* treeCh;
	TTree* treeSamp;
	TTree* treeEvt;
	TTree* treeRTI;

};

#endif /* DEVICEMANAGER_H_ */
