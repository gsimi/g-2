/*
 * Event.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: carlo
 */

#include "Event.h"
#include <iostream>

using namespace std;

Event::Event(uint32_t Nsamples) {
	// TODO Auto-generated constructor stub
	samplesStored = Nsamples;
	sample0 = new int16_t[samplesStored];
	sample1 = new int16_t[samplesStored];
	Free();
	cout << "Creato evento:\t" << samplesStored << " samples\n";
}

Event::~Event() {
	// TODO Auto-generated destructor stub
	Free();
}

void Event::FillEvent(uint64_t evtNumber, int64_t time, int16_t measureUnit, int16_t* values, int ch) {

	ID = evtNumber;
	instant = time;
	timeUnit = measureUnit;

	for (int32_t i = 0; i<samplesStored; i++) {
	  if (ch==0)
	    sample0[i] = values[i];
	  if (ch==1)
	    sample1[i] = values[i];
	}

}

void Event::FillRTI(uint64_t inBlock, uint64_t time, uint64_t howMany) {

	RTI.waveformInBlock = inBlock;
	RTI.elapsedTime = time;
	RTI.waveformStored = howMany;

}

void Event::Free() {
	ID = 0;
	instant = 0;
	timeUnit = 0;
	free(sample0);
	sample0 = (int16_t*) calloc(samplesStored,sizeof(int16_t));
	free(sample1);
	sample1 = (int16_t*) calloc(samplesStored,sizeof(int16_t));
	RTI.waveformInBlock = 0;
	RTI.elapsedTime = 0;
	RTI.waveformStored = 0;
}

void Event::PrintEvent(const uint32_t min, const uint32_t max) {

	cout << "******************************************************************************\n";
	cout << "EVENT\t" << ID << endl;
	cout << "Trigger instant\t" << instant << endl;
	cout << "Time unit\t" << timeUnit << endl << endl;

	if (max > (uint32_t)samplesStored) cout << "MAX PRINT RANGE MUST BE LESS THAN " << samplesStored << endl;
	else {
	  for (uint32_t i = min; i<max; i++) cout << "Sample0\t" << i << "\t" << sample0[i] << endl;
	  for (uint32_t i = min; i<max; i++) cout << "Sample1\t" << i << "\t" << sample1[i] << endl;
	}

}

void Event::PrintRTI() {

	cout << "******************************************************************************\n";
	cout << "RTI" << endl;
	cout << "Waveforms in block\t" << RTI.waveformInBlock << endl;
	cout << "Elapsed time\t" << RTI.elapsedTime << endl;
	cout << "Waveforms stored\t" << RTI.waveformStored << endl;

}

void Event::CreateEventBranches(TTree* treeEvt) {

	treeEvt->Branch("ID",&ID,"ID/l");
	treeEvt->Branch("nSamp",&samplesStored,"nSamp/I");
	treeEvt->Branch("Instant",&instant,"Instant/L");
	treeEvt->Branch("TimeUnit",&timeUnit,"TimeUnit/S");
	treeEvt->Branch("WaveformsA",&sample0[0],"Samples[nSamp]/S");
	treeEvt->Branch("WaveformsB",&sample1[0],"Samples[nSamp]/S");

}

void Event::CreateRTIBranches(TTree* treeRTI) {

//	treeRTI->Branch("RTI",&RTI.waveformInBlock,"WaveformInBlock/l:ElapsedTime:WaveformStored");
	treeRTI->Branch("WaveformInBlock",&RTI.waveformInBlock,"WaveformInBlock/l");
	treeRTI->Branch("ElapsedTime",&RTI.elapsedTime,"ElapsedTime/l");
	treeRTI->Branch("WaveformStored",&RTI.waveformStored,"WaveformStored/l");

}
