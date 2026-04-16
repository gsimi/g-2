/*
 * Event.h
 *
 *  Created on: Dec 19, 2016
 *      Author: carlo
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "TTree.h"

enum {chA,chB};
class Event {

public:
	struct event {
		uint64_t ID;
		int32_t samplesStored;
		int64_t instant;
		int16_t timeUnit;
		int16_t* sample;
	};

	struct runningTimeInfo {
		uint64_t waveformInBlock;
		uint64_t elapsedTime;
		uint64_t waveformStored;
	};

public :
	Event(uint32_t Nsamples);
	virtual ~Event();
	void FillEvent(uint64_t evtNumber, int64_t time, int16_t measureUnit, int16_t* values, int ch);
	void FillRTI(uint64_t inBlock, uint64_t time, uint64_t howMany);
	void Free();
	void PrintEvent(const uint32_t min, const uint32_t max);
	void PrintRTI();

	void CreateEventBranches(TTree* treeEvt);
	void CreateRTIBranches(TTree* treeRTI);

	uint64_t Get_ID() {return ID;};
	uint32_t Get_samplesStored() {return samplesStored;};
	int64_t Get_instant() {return instant;};
	int16_t Get_timeUNit() {return timeUnit;};
	int16_t* Get_ptrToSamples(int ch) {return ch==chA ? sample0 : sample1;};
	runningTimeInfo Get_RTI() {return RTI;};

private :
	uint64_t ID;
	int32_t samplesStored;
	int64_t instant;
	int16_t timeUnit;
	int16_t* sample0;
	int16_t* sample1;

	runningTimeInfo RTI;

};

#endif /* EVENT_H_ */
