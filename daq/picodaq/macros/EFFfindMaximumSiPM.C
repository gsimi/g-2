#include <TF1.h>
#include <TSpectrum.h>
#include <fstream>
#include <TMath.h>
#include <TH1F.h>
#include <TDirectory.h>
#include <string>
#include <TROOT.h>
#include <TSystem.h>
#include <TGraphErrors.h>
#include <TStyle.h>

//#include "/home/lhcb/g-2/daq/picodaq/ReadTree.C"

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

void ReadTree(const char *fileName, bool negative)
{

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


	// apro il file in sola lettura
	TFile *input_file = new TFile(fileName,"READ");

	// leggo i trees
	TTree *treeCh = (TTree*)input_file->Get("Channels");
	TTree *treeSamp = (TTree*)input_file->Get("SampSets");
	TTree *treeEvt = (TTree*)input_file->Get("Event");
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

	// imposto i branches per gli eventi
	sample = new short[sampSet.samplesStoredPerEvent];
	treeEvt->SetBranchAddress("ID",&ID);
	treeEvt->SetBranchAddress("nSamp",&samplesStored);
	treeEvt->SetBranchAddress("Instant",&triggerInstant);
	treeEvt->SetBranchAddress("TimeUnit",&timeUnit);
	treeEvt->SetBranchAddress("Waveforms",&sample[0]);


	Long64_t nEvt = treeEvt->GetEntries();
	float maximum = 0.0;
	float minimum = 0.0;

	// spettro in energia
	bool anegative = ~negative;
	cout<<negative<<'\t'<<anegative<<endl;
	float xmin = negative? adc_to_mv(sampSet.max_adc_value,chSet2.range,-1*sampSet.max_adc_value) : 0 ; 
	float xmax = negative? 0 : adc_to_mv(sampSet.max_adc_value,chSet2.range,sampSet.max_adc_value) ;
	int nbinRange;
	if(chSet2.range==3) nbinRange=128;
//////////////////////////////////////////////////////////////////////////////////////////////////////////
	else if(chSet2.range==4) nbinRange=256;
	else if(chSet2.range==5) nbinRange=512;
	else if(chSet2.range==6) nbinRange=1024;
	else if(chSet2.range==7) nbinRange=2048;
	else if(chSet2.range==8) nbinRange=4096;
	else if(chSet2.range==9) nbinRange=8192;
	else  nbinRange=100000;



	TH1F* spectrumMaximum = new TH1F( "hMax", "Maxima Distribution Spectrum", nbinRange, xmin,xmax );
	cerr<<" Range   : "<<xmax-xmin<<" mV "<<endl;
	cerr<<" #Events : "<<nEvt<<endl;
	cerr<<" #Samples: "<<sampSet.samplesStoredPerEvent<<endl;
	cerr<<" Timestamp: "<<sampSet.timeIntervalNanoseconds<<" ns"<<endl;
//////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (Long64_t index=0; index<nEvt; index++) 
	{
		treeEvt->GetEntry(index);
		for (int ii=0; ii<sampSet.samplesStoredPerEvent; ii++)
		{
			float value =  adc_to_mv(sample[ii],chSet2.range,sampSet.max_adc_value);
			if (value > maximum) maximum = value ;
			if (value < minimum) minimum = value ;
		}
		spectrumMaximum->Fill(negative?minimum:maximum);
		maximum = 0.0;
		minimum = 0.0;
	}
}


int findMaximumSiPM(string filename1, string filename2, string filename3)
{

string filename = filename1 + filename2+ filename3; 
//	gSystem->Load("ReadTree_C.so");
//	gROOT->ProcessLineSync(".L ReadTree.C+");
	ReadTree(filename.c_str(), false);
	TH1F* hMax = (TH1F*)gDirectory->FindObject("hMax");
	hMax->GetXaxis()->SetTitle("Pulse Height[mV] ");
	if (hMax == 0) return 1; 

//	double xmax = hMax->GetXaxis()->GetXmax();
//	double xmin = hMax->GetXaxis()->GetXmin() + (xmax - hMax->GetXaxis()->GetXmin())/90.0;

	TCanvas c;
	hMax->Draw();
	string imgname = "graphs/"+filename1+filename2  + ".pdf";
	c.SaveAs(imgname.c_str());
	imgname = "graphs/"+filename1+filename2  + ".C";
	c.SaveAs(imgname.c_str());
	imgname = "graphs/"+filename1+filename2  + ".png";
	c.SaveAs(imgname.c_str());
	

	return 0;
}


void fitGain(string files)
{
	ifstream fin(files.c_str());
	ofstream debug("log.txt");
	string filename;
	vector<float> offset, eOffset, gain,eGain, V;
	float tmpOffset, tmpEOffset, tmpGain, tmpEGain, tmpV;
	gStyle->SetOptFit();
	while(fin>> filename>>tmpV)
	{
		//if (findMaximumSiPM(filename, tmpOffset, tmpEOffset, tmpGain, tmpEGain)) continue;
		debug << tmpOffset << ' ' << tmpGain << endl;
		offset.push_back(tmpOffset);
		eOffset.push_back(tmpEOffset);
		gain.push_back(tmpGain);
		eGain.push_back(tmpEGain);
		V.push_back(tmpV);
	}
	TGraphErrors* g = new TGraphErrors(offset.size(), &V[0], &gain[0], 0, &eGain[0]);
	g->Draw();
	TF1 *f = new TF1("linfit", "pol1");
	g->Fit(f, "EMQ");
	TCanvas c;
	g->Draw();
	string imgname = "graphs/gain.png";
	c.SaveAs(imgname.c_str());
	imgname = "graphs/gain.tex";
	c.SaveAs(imgname.c_str());
	imgname = "graphs/gain.C";
	c.SaveAs(imgname.c_str());
	
}

