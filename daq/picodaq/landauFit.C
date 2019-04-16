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

#include <stdint.h>

#include <sstream>

#include "TFile.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TCanvas.h"

#include "/home/lhcb/g-2/daq/picodaq/InfoAcq.cc"
#include "/home/lhcb/g-2/daq/picodaq/Event.cc"

string to_string(int i)
{
	stringstream s;
	s << i;
	return s.str();
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

double fitFunction(double *x, double *p)
{
	return p[2]*TMath::Landau(-x[0], p[0], p[1]);
}

long ReadTree(const char *fileName, bool negative)
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
	long time;


	// apro il file in sola lettura
	TFile *input_file = new TFile(fileName,"READ");

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
	treeRTI->GetBranch("ElapsedTime")->SetAddress(&time);

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
	float xmin= negative? adc_to_mv(sampSet.max_adc_value,chSet1.range,-1*sampSet.max_adc_value) : 0 ; 
	float xmax = negative? 0 : adc_to_mv(sampSet.max_adc_value,chSet1.range,sampSet.max_adc_value) ;
	TH1F* spectrumMaximum = new TH1F( "hMax", "Maximum Spectrum", 1024, xmin,xmax );
	
	
	static int failCount = 0;
	//gSystem->Exec("rm fail/*");
	for (Long64_t index=0; index<nEvt; index++) {
		treeEvt->GetEntry(index);
		for (int ii=0; ii<sampSet.samplesStoredPerEvent; ii++) {
			float value =  adc_to_mv(sample[ii],chSet1.range,sampSet.max_adc_value);
			if (value > maximum) maximum = value ;
			if (value < minimum) minimum = value ;
		}
		spectrumMaximum->Fill(negative?minimum:maximum);
		if ((negative?minimum:maximum) >= 0)
		{
			cerr << "Error: positive signal\n";
			TH1F signal( "Event Plot", "Event Plot",sampSet. samplesStoredPerEvent, -sampSet.preTrig*sampSet.timeIntervalNanoseconds, (sampSet.samplesStoredPerEvent-sampSet.preTrig)*sampSet.timeIntervalNanoseconds );
	
			for (int jj=0; jj<sampSet.samplesStoredPerEvent; jj++) signal.SetBinContent(jj,adc_to_mv(sample[jj],chSet1.range,sampSet.max_adc_value));
			/*TCanvas c;
			signal.Draw();
			string filename = "fail/failSignal" + to_string(failCount)+".png";
			c.SaveAs(filename.c_str());*/
			failCount ++;
	
		}
		/*else
		{	
			clog << (negative?minimum:maximum) << endl;
		}*/
		maximum = 0.0;
		minimum = 0.0;
	}
	treeRTI->GetEntry(29);
	return time;
}

void writeEff(TH1F* h, double min, double max, string name, ostream& out)
{
	int nEntries = h->GetEntries();
	int minBin = h->GetXaxis()->FindBin(min);
	int maxBin = h->GetXaxis()->FindBin(max);
	int integral = h->Integral(minBin, maxBin);
	double eff = integral*1.0/nEntries;
	out << "Diode " << name << ":" << endl;
	out << "\tN = " << nEntries << endl;
	out << "\tn = " << integral << endl;
	out << "\teff = " << eff*100 << "%" << endl;
	out << "\ten = " << sqrt(eff*(1-eff)*nEntries) << endl;
	out << "\ten/N = " << sqrt(eff*(1-eff)/nEntries)*100 << "%" << endl;
	out << "------------------------------------------------------------------------------" << endl;
}

void writeShortEff(TH1F* h, double min, double max, double t, string name, ostream& out)
{
	int nEntries = h->GetEntries();
	int minBin = h->GetXaxis()->FindBin(min);
	int maxBin = h->GetXaxis()->FindBin(max);
	int integral = h->Integral(minBin, maxBin);
	double N123 = 3000/t;
	out << name << " ";
	out << N123 << " " << endl;
	//out << sqrt(eff*(1-eff)/nEntries) << endl;
}

TF1 *ff = new TF1("InversedLandau", fitFunction,-1000,0, 3);
fstream* fout = new fstream("analysis.txt", ios::out|ios::app);
fstream* effOut = new fstream("eff.txt", ios::out|ios::app);

int landauFit(string filename)
{
	long time = ReadTree(filename.c_str(), true);
	TH1F* hMax = (TH1F*)gDirectory->FindObject("hMax");
	if (hMax == 0) return 1; 
	
	writeEff(hMax, -1000,-20,filename,*fout);
	writeShortEff(hMax, -1000,-20,time,filename,*effOut);
	
	gStyle->SetOptFit();
	//gStyle->SetOptStat(001001011);
	
	TF1* f = new TF1("fitFun", fitFunction, -500, -20, 3); 
	
	f->SetParNames("mpv","sigma", "A");
	f->SetParameters(100, 100, 100);
	f->SetParLimits(1, 0, 1000);
	f->SetParLimits(2, 0, 1000000);
	
	hMax->Fit(f, "EMQR");
	TCanvas c;
	hMax->Draw();
	string imgname = "graphs/" + filename + ".png";
	c.SaveAs(imgname.c_str());
	imgname = "graphs/" + filename + ".tex";
	c.SaveAs(imgname.c_str());
	imgname = "graphs/" + filename + ".C";
	c.SaveAs(imgname.c_str());
	return 0;
}
