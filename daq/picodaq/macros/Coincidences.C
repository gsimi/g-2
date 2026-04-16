#include <stdint.h>
//typedef  char int8_t;
// typedef  short int16_t;
// typedef  unsigned short uint16_t;
// typedef  int int32_t;
// typedef  unsigned int uint32_t;
// typedef  unsigned long long uint64_t;
// typedef  long long int64_t;

#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TCanvas.h"

#include "../src/InfoAcq.cc"
#include "../src/Event.cc"

void SetCanvas(TCanvas *cCurrent, bool kGrid=false)
{
	cCurrent->SetSelected(cCurrent);
   	cCurrent->ToggleToolBar();
   	cCurrent->ToggleEditor();
	if(kGrid) cCurrent->SetGrid();
}

float adc_to_mv(int16_t raw, int16_t rangeIndex, int16_t maxADCValue)
{
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

	return (raw * inputRanges[rangeIndex])*1. / maxADCValue;
}

void ShowSpectrum(const char *fileName, bool negative=false, bool draw = true) {

	// dichiaro le struct
	InfoAcq::chSettings chSet1;
	InfoAcq::chSettings chSet2;
	InfoAcq::samplingSettings sampSet;

	unsigned long long ID;
	int	  samplesStored;
	long long triggerInstant;
	short 	  timeUnit;
	short	 *sample;

	unsigned long long waveformInBlock;
	unsigned long long elapsedTime;
	unsigned long long waveformStored;

	TFile *input_file = new TFile(fileName,"READ");

	// controllo a schermo la struttura del file
	input_file->Print();

	// leggo i trees
	TTree *treeCh = (TTree*)input_file->Get("Channels");
	TTree *treeSamp = (TTree*)input_file->Get("SampSets");
	TTree *treeEvt = (TTree*)input_file->Get("Event");
	TTree *treeRTI = (TTree*)input_file->Get("RTI");

	// TFile->Get() restituisce un oggetto generico che va
	// convertito esplicitamente anteponendo (TTree*)

	// prelevo i branch con le info e li associo alle struct
	treeCh->SetBranchAddress("Ch1",&chSet1.enabled);
	treeCh->SetBranchAddress("Ch2",&chSet2.enabled);
	treeSamp->SetBranchAddress("Settings",&sampSet.max_adc_value);

	// leggo le entries
	// dichiaro l'oggetto InfoAcq e lo riempio
	InfoAcq* info = new InfoAcq();
	cout << "Riempio l'oggetto INFO\n";
	treeCh->GetEntry(0);
	treeSamp->GetEntry(0);
	info->FillSettings(&chSet1,&chSet2,&sampSet);
	info->PrintInfo();

	// imposto i branches per gli eventi
	sample = new short[sampSet.samplesStoredPerEvent];
	treeEvt->SetBranchAddress("ID",&ID);
	treeEvt->SetBranchAddress("nSamp",&samplesStored);
	treeEvt->SetBranchAddress("Instant",&triggerInstant);
	treeEvt->SetBranchAddress("TimeUnit",&timeUnit);
	treeEvt->SetBranchAddress("Waveforms",&sample[0]);
	treeRTI->SetBranchAddress("WaveformInBlock",&waveformInBlock);
	treeRTI->SetBranchAddress("ElapsedTime",&elapsedTime);
	treeRTI->SetBranchAddress("WaveformStored",&waveformStored);


	Long64_t nRTI = treeRTI->GetEntries();
	Long64_t nEvt = treeEvt->GetEntries();
	float integral = 0.0;
	float maximum = 0.0;
	float minimum = 0.0;

	float xmin = negative? adc_to_mv(sampSet.max_adc_value,chSet1.range,-1*sampSet.max_adc_value) : 0 ; 
	float xmax = negative? 0 : adc_to_mv(sampSet.max_adc_value,chSet1.range,sampSet.max_adc_value) ;
	TH1F* spectrumMaximum = new TH1F( "hMax", "Maxima Distribution", 1024, xmin,xmax );


	for (Long64_t index=0; index<nEvt; index++) 
	{
		treeEvt->GetEntry(index);
		for (int ii=0; ii<sampSet.samplesStoredPerEvent; ii++)
		{
		  float value =  adc_to_mv(sample[ii],chSet1.range,sampSet.max_adc_value);
			if (value > maximum) maximum = value ;
			if (value < minimum) minimum = value ;
		}
		spectrumMaximum->Fill(negative?minimum:maximum);
		maximum = 0.0;
		minimum = 0.0;
	}

	if(draw)
	{
		gStyle->SetOptStat(1001111);
		TCanvas * c0 = new TCanvas("Maxima Distribution");
		SetCanvas(c0, false);

		spectrumMaximum->SetXTitle("Pulse Height (mV)");
		spectrumMaximum->SetYTitle("Frequency");
		spectrumMaximum->Draw();
	}
}





void viewEvent(const char* fileName, int i){


	TFile *input_file = new TFile(fileName,"READ");

	// dichiaro le struct
	InfoAcq::chSettings chSet1;
	InfoAcq::chSettings chSet2;
	InfoAcq::samplingSettings sampSet;

	unsigned long long ID;
	int samplesStored;
	long long triggerInstant;
	short timeUnit;
	short* sample;

	unsigned long long waveformInBlock;
	unsigned long long elapsedTime;
	unsigned long long waveformStored;


	// controllo a schermo la struttura del file
	input_file->Print();

	// leggo i trees
	TTree *treeCh = (TTree*)input_file->Get("Channels");
	TTree *treeSamp = (TTree*)input_file->Get("SampSets");
	TTree *treeEvt = (TTree*)input_file->Get("Event");
	TTree *treeRTI = (TTree*)input_file->Get("RTI");
	// TFile->Get() restituisce un oggetto generico che va
	// convertito esplicitamente anteponendo (TTree*)

	// prelevo i branch con le info e li associo alle struct
	treeCh->SetBranchAddress("Ch1",&chSet1.enabled);
	treeCh->SetBranchAddress("Ch2",&chSet2.enabled);
	treeSamp->SetBranchAddress("Settings",&sampSet.max_adc_value);

	// leggo le entries
	// dichiaro l'oggetto InfoAcq e lo riempio
	InfoAcq* info = new InfoAcq();
	cout << "Riempio l'oggetto INFO\n";
	treeCh->GetEntry(0);
	treeSamp->GetEntry(0);
	info->FillSettings(&chSet1,&chSet2,&sampSet);
	info->PrintInfo();

	// imposto i branches per gli eventi
	sample = new short[sampSet.samplesStoredPerEvent];
	treeEvt->SetBranchAddress("ID",&ID);
	treeEvt->SetBranchAddress("nSamp",&samplesStored);
	treeEvt->SetBranchAddress("Instant",&triggerInstant);
	treeEvt->SetBranchAddress("TimeUnit",&timeUnit);
	treeEvt->SetBranchAddress("Waveforms",&sample[0]);
	treeRTI->SetBranchAddress("WaveformInBlock",&waveformInBlock);
	treeRTI->SetBranchAddress("ElapsedTime",&elapsedTime);
	treeRTI->SetBranchAddress("WaveformStored",&waveformStored);


	int daVedere = i;

	// leggo e disegno un evento
	// dichiaro l'oggetto Event e lo riempio
	Event* evt = new Event(sampSet.samplesStoredPerEvent);
	cout << "Riempio l'oggetto EVENT\n";

	treeRTI->GetEntry(0);
	treeRTI->GetEntry(daVedere/waveformInBlock);
	evt->FillRTI(waveformInBlock,elapsedTime,waveformStored);
	evt->PrintRTI();

	treeEvt->GetEntry(daVedere);
	evt->FillEvent(ID,triggerInstant,timeUnit,sample);

	TH1F* signal = new TH1F( "Event Plot", "Event Plot",sampSet. samplesStoredPerEvent, -sampSet.preTrig*sampSet.timeIntervalNanoseconds, (sampSet.samplesStoredPerEvent-sampSet.preTrig)*sampSet.timeIntervalNanoseconds );
	
	for (int jj=0; jj<sampSet.samplesStoredPerEvent; jj++) signal->SetBinContent(jj,adc_to_mv(sample[jj],chSet1.range,sampSet.max_adc_value));

	// Grafici
	TCanvas * c0 = new TCanvas("c0");

	c0->cd(3);
	signal->SetXTitle("Instant (ns)");
	signal->SetYTitle("Amplitude (mV)");
	signal->Draw();

/**/
}
