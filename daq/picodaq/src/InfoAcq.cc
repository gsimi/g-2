/*
 * InfoAcq.cpp
 *
 *  Created on: Dec 20, 2016
 *      Author: carlo
 */

#include "InfoAcq.h"
#include <cstdio>
#include <iostream>

using namespace std;

InfoAcq::InfoAcq() {
	// TODO Auto-generated constructor stub

	for (int32_t i=0; i<2; i++){
		chSet[i].enabled = false;
		chSet[i].DCcoupled = 'N'; // DC = D, AC = A, None = N
		chSet[i].range = -1;
		chSet[i].analogueOffset = 0.0;
		chSet[i].triggerSet = false;
		chSet[i].triggerDirection = 'N'; // Rising = R, Falling = F, None = N
		chSet[i].triggerThreshold = 0.0;
	}

	sampSet.no_segments = 0;
	sampSet.no_waveforms = 0;
	sampSet.max_adc_value = 0;
	sampSet.timebase = 0;
	sampSet.nSamplesDIV = 0;
	sampSet.samplesStoredPerEvent = 0;
	sampSet.preTrig = 0;
	sampSet.timeIntervalNanoseconds = 0;

}

InfoAcq::~InfoAcq() {
	// TODO Auto-generated destructor stub
}

void InfoAcq::FillSettings(chSettings *ch1, chSettings *ch2, samplingSettings *samp) {

	if (ch1->enabled) {
		chSet[0].enabled = ch1->enabled;
		chSet[0].DCcoupled = ch1->DCcoupled;
		chSet[0].analogueOffset = ch1->analogueOffset;
		chSet[0].range = ch1->range;
		chSet[0].triggerDirection = ch1->triggerDirection;
		chSet[0].triggerSet = ch1->triggerSet;
		chSet[0].triggerThreshold = ch1->triggerThreshold;
	}

	if (ch2->enabled) {
		chSet[1].enabled = ch2->enabled;
		chSet[1].DCcoupled = ch2->DCcoupled;
		chSet[1].analogueOffset = ch2->analogueOffset;
		chSet[1].range = ch2->range;
		chSet[1].triggerDirection = ch2->triggerDirection;
		chSet[1].triggerSet = ch2->triggerSet;
		chSet[1].triggerThreshold = ch2->triggerThreshold;
	}

	sampSet.max_adc_value = samp->max_adc_value;
	sampSet.nSamplesDIV = samp->nSamplesDIV;
	sampSet.no_segments = samp->no_segments;
	sampSet.no_waveforms = samp->no_waveforms;
	sampSet.preTrig = samp->preTrig;
	sampSet.samplesStoredPerEvent = samp->samplesStoredPerEvent;
	sampSet.timeIntervalNanoseconds = samp->timeIntervalNanoseconds;
	sampSet.timebase = samp->timebase;

}

void InfoAcq::PrintInfo() {

	uint16_t inputRanges [12] = {
			10,
			20,
			50,
			100,
			200,
			500,
			1000,
			2000,
			5000,
			10000,
			20000,
			50000};

	for (int32_t i=0; i<2; i++) {

		cout << "******************************************************************************\n";
		printf("CHANNEL %c ",'A'+i);

		if (!chSet[i].enabled) cout << "NOT ENABLED\n";
		else if (chSet[i].enabled) {
			cout << "ENABLED\n";
			cout << "Coupling\t\t";
			if (chSet[i].DCcoupled == 'D') cout << "DC\n";
			else if (chSet[i].DCcoupled == 'A') cout << "AC\n";
			cout << "Range (mV)\t\t" << inputRanges[chSet[i].range] << endl;
			cout << "Offset (mV)\t\t" << chSet[i].analogueOffset << endl;
			if (!chSet[i].triggerSet) cout << "TRIGGER NOT SET\n";
			else if (chSet[i].triggerSet) {
				cout << "TRIGGER SET\n";
				cout << "Trigger direction\t";
				if (chSet[i].triggerDirection == 'R') cout << "RISING\n";
				else if (chSet[i].triggerDirection == 'F') cout << "FALLING";
				cout << "Trigger threshold (mV)\t" << chSet[i].triggerThreshold << endl;
			}
		}
		cout << "\n";
	}

	cout << "******************************************************************************\n";
	cout << "SETTINGS\n";
	cout << "Max ADC value\t\t\t\t" << sampSet.max_adc_value << endl;
	cout << "Number of segments\t\t\t" << sampSet.no_segments << endl;
	cout << "Number of waveforms\t\t\t" << sampSet.no_waveforms << endl;
	cout << "Timebase\t\t\t\t" << sampSet.timebase << endl;
	cout << "Number of samples per DIV\t\t" << sampSet.nSamplesDIV << endl;
	cout << "Number of samples per waveform\t\t" << sampSet.samplesStoredPerEvent << endl;
	cout << "Number of pretrigger samples\t\t" << sampSet.preTrig << endl;
	cout << "Sampling interval (ns)\t\t" << sampSet.timeIntervalNanoseconds << endl;
	cout << "\n";

}

void InfoAcq::CreateChannelBranches(TTree* treeCh){

	treeCh->Branch("Ch1",&chSet[0].enabled,"Enabled/O:Coupling/C:Range/S:Offset/F:TriggerSet/O:Direction/C:Threshold/F");
	treeCh->Branch("Ch2",&chSet[1].enabled,"Enabled/O:Coupling/C:Range/S:Offset/F:TriggerSet/O:Direction/C:Threshold/F");

}

void InfoAcq::CreateSamplingBranches(TTree* treeSamp){

	cout << "Dimensione di sampSet\t" << sizeof(sampSet) << endl;
	treeSamp->Branch("Settings",&sampSet.max_adc_value,"MaxAdcValue/I:nSegments/i:nWaveforms/i:Timebase/i:nSamplesDIV/I:SamplesPerWaveform/I:PreTrig/I:DeltaT_ns/F");

}
