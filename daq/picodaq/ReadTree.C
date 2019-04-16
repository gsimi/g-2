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

#include "InfoAcq.cc"
#include "Event.cc"

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

void ReadTree(const char *fileName, int i, bool negative=false, bool draw = true) {

	// dichiaro le struct
	InfoAcq::chSettings chSet1;
	InfoAcq::chSettings chSet2;
	InfoAcq::samplingSettings sampSet;
	
	// dichiaro le variabili dell'evento
/*	uint64_t ID;
	uint32_t samplesStored;
	int64_t triggerInstant;
	int16_t timeUnit;
	int16_t* sample;

	uint64_t waveformInBlock;
	uint64_t elapsedTime;
	uint64_t waveformStored;
*/
	unsigned long long ID;
	int samplesStored;
	long long triggerInstant;
	short timeUnit;
	short* sample;

	unsigned long long waveformInBlock;
	unsigned long long elapsedTime;
	unsigned long long waveformStored;


	// apro il file in sola lettura
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

	int daVedere = i;

	// grafico dell'andamento temporale
	double* stored = new double[nRTI];
	double* time = new double[nRTI];

	for (int jj=0; jj<nRTI; jj++) {

		treeRTI->GetEntry(jj);
		stored[jj] = (double)waveformStored;
		time[jj] = (double)elapsedTime;

	}

	TGraph* timeDistr = new TGraph( nRTI, time, stored );
	timeDistr->SetNameTitle("Time Distribution","Time Distribution");
	
	// spettro in energia
	TH1F* spectrumIntegral = new TH1F( "hInt", "Integral Spectrum", 200, negative? -5e4:-1e3, negative? 1e3:5e4 );
	TH1F* spectrumIntegralMissed = new TH1F( "hIntMiss", "Integral Spectrum of missed events", 200, -5e4, 5e4 );
	float xmin = negative? adc_to_mv(sampSet.max_adc_value,chSet1.range,-1*sampSet.max_adc_value) : 0 ; 
	float xmax = negative? 0 : adc_to_mv(sampSet.max_adc_value,chSet1.range,sampSet.max_adc_value) ;
	TH1F* spectrumMaximum = new TH1F( "hMax", "Maximum Spectrum", 1024, xmin,xmax );
	TH1F* spectrumMaximumMissed = new TH1F( "hMaxMiss", "Maximum Spectrum of missed events", 180,
						-adc_to_mv(sampSet.max_adc_value,chSet1.range,sampSet.max_adc_value),
						adc_to_mv(sampSet.max_adc_value,chSet1.range,sampSet.max_adc_value) );
	TH1F* spectrumMaximumSig = new TH1F( "hMaxSig", "Maximum Spectrum of signal events", 180,
						-adc_to_mv(sampSet.max_adc_value,chSet1.range,sampSet.max_adc_value),
						adc_to_mv(sampSet.max_adc_value,chSet1.range,sampSet.max_adc_value) );
	for (Long64_t index=0; index<nEvt; index++) {
	  //	  if(index%2 == 0 ) continue;
		treeEvt->GetEntry(index);
		for (int ii=0; ii<sampSet.samplesStoredPerEvent; ii++) {
		  float value =  adc_to_mv(sample[ii],chSet1.range,sampSet.max_adc_value);
		  //printf ("valure :%f\n",value);
		  integral += value;
			if (value > maximum) maximum = value ;
			if (value < minimum) minimum = value ;
		}
		daVedere = index;
		spectrumIntegral->Fill(integral);
		spectrumMaximum->Fill(negative?minimum:maximum);
		if(negative? minimum > -50 : maximum < 50){
		  spectrumIntegralMissed->Fill(integral);
		  spectrumMaximumMissed->Fill(negative?minimum:maximum);
		} else {
		  spectrumMaximumSig->Fill(negative?minimum:maximum);
		}
		integral = 0.0;
		maximum = 0.0;
		minimum = 0.0;
	}

	// leggo e disegno un evento
	// dichiaro l'oggetto Event e lo riempio
	Event* evt = new Event(sampSet.samplesStoredPerEvent);
	cout << "Riempio l'oggetto EVENT\n";
	cerr << "DEBUG!!!!!: " << daVedere << " E QUESTO: " << waveformInBlock << endl;

	treeRTI->GetEntry(0);
	treeRTI->GetEntry(daVedere/waveformInBlock);
	evt->FillRTI(waveformInBlock,elapsedTime,waveformStored);
	evt->PrintRTI();

	treeEvt->GetEntry(i);
	evt->FillEvent(ID,triggerInstant,timeUnit,sample);

	TH1F* signal = new TH1F( "Event Plot", "Event Plot",sampSet. samplesStoredPerEvent, -sampSet.preTrig*sampSet.timeIntervalNanoseconds, (sampSet.samplesStoredPerEvent-sampSet.preTrig)*sampSet.timeIntervalNanoseconds );
	
	for (int jj=0; jj<sampSet.samplesStoredPerEvent; jj++) signal->SetBinContent(jj,adc_to_mv(sample[jj],chSet1.range,sampSet.max_adc_value));

	// Grafici
	if(draw)
	{
		TCanvas * c0 = new TCanvas("c0");
		c0->Divide(2,2);
	
		c0->cd(1);
		timeDistr->SetLineColor(2);
	/*	timeDistr->SetLineWidth(1);
		timeDistr->SetMarkerColor(4);
		timeDistr->SetMarkerStyle(2);
	*/	timeDistr->GetXaxis()->SetTitle("Instant (ms)");
		timeDistr->GetYaxis()->SetTitle("Waveforms stored");
		timeDistr->Draw("apL");

		c0->cd(2);
		spectrumIntegral->SetXTitle("Integral");
		spectrumIntegral->SetYTitle("Frequency");
		spectrumIntegral->Draw();

		c0->cd(3);
		signal->SetXTitle("Instant (ns)");
		signal->SetYTitle("Amplitude (mV)");
		signal->Draw();

		c0->cd(4);
		spectrumMaximum->SetXTitle("Maximum (mV)");
		spectrumMaximum->SetYTitle("Frequency");
		spectrumMaximum->Draw();
	}
	printf("# signals =%f, efficiency %f \%\n",
	       spectrumMaximumSig->GetEntries(),
	       spectrumMaximumSig->GetEntries()/waveformStored*100);

/**/
}





void viewEvent(TFile* input_file, int i){

	// dichiaro le struct
	InfoAcq::chSettings chSet1;
	InfoAcq::chSettings chSet2;
	InfoAcq::samplingSettings sampSet;
	
	// dichiaro le variabili dell'evento
/*	uint64_t ID;
	uint32_t samplesStored;
	int64_t triggerInstant;
	int16_t timeUnit;
	int16_t* sample;

	uint64_t waveformInBlock;
	uint64_t elapsedTime;
	uint64_t waveformStored;
*/
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
