/*
 * InfoAcq.h
 *
 *  Created on: Dec 20, 2016
 *      Author: carlo
 */

#ifndef INFOACQ_H_
#define INFOACQ_H_

#include "TTree.h"

//#include "Structs.h"

class InfoAcq {

public:

	struct chSettings {
		bool enabled;
		int8_t DCcoupled; // DC = D, AC = A, None = N
		int16_t range; // None = -1
		float analogueOffset;
		bool triggerSet;
		int8_t triggerDirection; // Rising = R, Falling = F, None = N
		float triggerThreshold;
	};

	struct samplingSettings {
		int32_t max_adc_value;
		uint32_t no_segments;
		uint32_t no_waveforms;
		uint32_t timebase;
		int32_t nSamplesDIV;
		int32_t samplesStoredPerEvent;
		int32_t preTrig;
		float timeIntervalNanoseconds;
	};

public:

	InfoAcq();
	virtual ~InfoAcq();
	void FillSettings(chSettings *ch1, chSettings *ch2, samplingSettings *samp);
	void PrintInfo();
	void CreateChannelBranches(TTree* treeCh);
	void CreateSamplingBranches(TTree* treeSamp);

	chSettings* Get_ptrToChSet() {return &chSet[0];};
	samplingSettings Get_sampSet() {return sampSet;};

private:

	chSettings chSet[2];
	samplingSettings sampSet;

};

#endif /* INFOACQ_H_ */
