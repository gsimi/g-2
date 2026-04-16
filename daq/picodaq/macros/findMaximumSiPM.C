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

enum {chA,chB};
void ReadTree(const char *fileName, bool negative, int channel = chB, Long64_t nEvtMax=-1)
{

	// dichiaro le struct
	InfoAcq::chSettings chSet1;
	InfoAcq::chSettings chSet2;
	InfoAcq::samplingSettings sampSet;
	
	// dichiaro le variabili dell'evento
	unsigned long long ID;
	int samplesStored;
	long long triggerInstant;
	short timeUnit;
	short* sample0;
	short* sample1;


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

	InfoAcq::chSettings chSet;
	chSet = channel == chA ? chSet1 : chSet2;

	if (chSet.enabled == false) {
	  cout<<" channel "<<channel<<" not enabled, stopping"<<endl;
	  break;
	}
	// imposto i branches per gli eventi
	sample0 = new short[sampSet.samplesStoredPerEvent];
	sample1 = new short[sampSet.samplesStoredPerEvent];
	treeEvt->SetBranchAddress("ID",&ID);
	treeEvt->SetBranchAddress("nSamp",&samplesStored);
	treeEvt->SetBranchAddress("Instant",&triggerInstant);
	treeEvt->SetBranchAddress("TimeUnit",&timeUnit);
	treeEvt->SetBranchAddress("WaveformsA",&sample0[0]);
	treeEvt->SetBranchAddress("WaveformsB",&sample1[0]);

	Long64_t nEvt = treeEvt->GetEntries();
	if (nEvtMax>=0 && nEvtMax<nEvt){nEvt=nEvtMax;}
	float maximum = 0.0;
	float minimum = 0.0;

	// spettro in energia
	
	float xmin= negative? adc_to_mv(sampSet.max_adc_value,chSet.range,-1*sampSet.max_adc_value) : 0 ; 
	float xmax = negative? 0 : adc_to_mv(sampSet.max_adc_value,chSet.range,sampSet.max_adc_value) ;
	//float xmin=-200;
	//float xmax=0;
	TH1F* spectrumMaximum = new TH1F( "hMax", "Maxima Distribution Spectrum", 1000, xmin,xmax );
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	TH1F* spectrumMaximum = new TH1F( "hMax", "Maximum Spectrum", 256, xmin,xmax );
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
		  short sample = channel == chA ? sample0[ii] : sample1[ii];
			float value =  adc_to_mv(sample,chSet.range,sampSet.max_adc_value);
			if (value > maximum) maximum = value ;
			if (value < minimum) minimum = value ;
		}
		//spectrumMaximum->Fill(negative?minimum:maximum);
		spectrumMaximum->Fill(negative?minimum-maximum:maximum-minimum);
		maximum = 0.0;
		minimum = 0.0;
	}
}

class FitFunction
{
	private:
		int n;
  //  float _bw;
	public:
  //  set_bw(float bw){_bw=bw;}
	FitFunction(int n): n(n){};
	double operator()(double* x, double *p)
	{
		//const int n = 5; // n gaussiane
		// 2 + 2n parametri

		double offset = p[0];
		double gain = p[1];
		double mu = p[2];
		double norm = p[3];
		//double *area = &p[2]; // n ampiezze
		double *sigma = &p[4]; // n sigma
		double X = x[0];
		double result = 0;
		cout<<"called"<<endl;
		for (int i =0; i < n; i++)
		{
		  result += norm*TMath::Poisson(i, mu)*TMath::Gaus(X, offset+i*gain, sigma[i]*fabs(gain), kTRUE);
		}
		return result;
	}
};


double fitfun(double* x, double *p)
	{
		//const int n = 5; // n gaussiane
		// 2 + 2n parametri

		double offset = p[0];
		double gain = p[1];
		double mu = p[2];
		double norm = p[3];
		//double *area = &p[2]; // n ampiezze
		double *sigma = &p[4]; // n sigma
		double X = x[0];
		double result = 0;
		int n=4;
		for (int i =0; i < n; i++)
		{
		  result += norm*TMath::Poisson(i, mu)*TMath::Gaus(X, offset+i*gain, sigma[i]*fabs(gain), kTRUE);
		}
		return result;
	}


///////////////////////////////////////////////////////////////////////////////////////////////////
//				          ***New ***
/*
double weighted_mean_with_error(double x[], double sigma[],int n, double &sigma_mean){

double s1=.0,s2=.0;
for(int i=0;i<n;i++){
						s1+=x[i]/(sigma[i]*sigma[i]);
						s2+=1/(sigma[i]*sigma[i]);
					}
double mean= s1/s2;
sigma_mean = sqrt(1/s2);

cerr<<" \n \n --->Average : "<<mean<<" +- "<<sigma_mean<<'\t'<<(sigma_mean/mean *100.)<<" % "<<endl;

return mean;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////



//	int findMaximumSiPM(string filename, float& offset, float& eOffset, float& gain, float& eGain)

enum  {positive,negative};
int findMaximumSiPM(string filename1, string filename2, string filename3 /* ,int nsigma*/, int nPeaks=4, int ch=chB, int polarity=negative, Long64_t nEvt=-1)
{
  
  string filename = filename1+filename2+filename3;
  
  float offset,  eOffset, gain,  eGain;
  
  //	gSystem->Load("ReadTree_C.so");
  //	gROOT->ProcessLineSync(".L ReadTree.C+");
  ReadTree(filename.c_str(), polarity, ch, nEvt);
	TSpectrum t;
	TH1F* hMax = (TH1F*)gDirectory->FindObject("hMax");
	hMax->GetXaxis()->SetTitle("Pulse Height[mV] ");
	if (hMax == 0) return 1;
//
	t.Search(hMax,2,"",0.01);

	//	int nPeaks = t.GetNPeaks() + 1;

	if (nPeaks <= 1) return 1;
	FitFunction* ff = new FitFunction(nPeaks);
	double xmax = hMax->GetXaxis()->GetXmax();
	double xmin = hMax->GetXaxis()->GetXmin() + (xmax - hMax->GetXaxis()->GetXmin())/90.0;
	
	//TF1* f = new TF1("fitFun", ff, xmin, xmax, nPeaks + 4, "ff"); 
	TF1* f = new TF1("fitfun", fitfun, xmin, xmax, nPeaks + 4); 
	f->SetNpx(1000);

	f->SetParNames("offset","gain", "mu", "norm");
	float gain=t.GetPositionX()[0] - t.GetPositionX()[1];
	if (polarity==negative) gain=-gain;
	float offset=t.GetPositionX()[0];
	float mu=t.GetPositionY()[1]/t.GetPositionY()[0]; if (mu>1) mu=1;
	float norm=hMax->Integral("width");	
	f->SetParameters(offset,
			 gain,
			 mu, 
			 norm);
	cout<<" offset = "<<offset<<"; gain = "<<gain<<"; mu = "<<mu<<"; norm= "<<norm<<endl;
	f->SetParLimits(f->GetParNumber("mu"), xmin, xmax);
	f->SetParLimits(f->GetParNumber("gain"), gain/10, gain*10);
	f->SetParLimits(f->GetParNumber("mu"), 0, 1);
	f->SetParLimits(f->GetParNumber("norm"), norm/3.0, norm*3.0);
	for (int i = 0; i < nPeaks; i++)
	{
		f->SetParameter(4+i, 0.3+0.15*i);
		f->SetParLimits(4+i, 0.05,3);
	}


	hMax->Fit(f, "EMR+");
	TCanvas c;
	hMax->Draw();
	string imgname = "graphs/"+filename1+filename2  + ".pdf";
	c.SaveAs(imgname.c_str());
	imgname = "graphs/"+filename1+filename2  + ".C";
	c.SaveAs(imgname.c_str());
/*
	imgname = "graphs/" + filename + ".tex";
	c.SaveAs(imgname.c_str());
	imgname = "graphs/" + filename + ".C";
	c.SaveAs(imgname.c_str());
*/

	offset = f->GetParameter(0);
	eOffset = f->GetParError(0);
	gain = f->GetParameter(1);
	eGain = f->GetParError(1);
	//file << f->GetParameter(0) << " " << f->GetParError(0) << " " << f->GetParameter(1) << " " << f->GetParError(1) << endl;

	// *** New ***
	cerr<< "### From Fit : "<<endl;
	cerr<< "\n *** Gain   : "<<gain<<" +- "<<eGain<<endl;
	float mu_fit = f->GetParameter(2);
	float sigma_mu_fit = f->GetParError(2);
	cerr<<   " *** mu_fit : "<<mu_fit<<" +- "<<sigma_mu_fit<<"\n\n"<<endl;

	float *areas = new float[nPeaks-1];
	for(int i=1;	i<nPeaks;	i++)
	{
	cerr<< " *** Peak "<<i<<endl;
	float sigma = f->GetParameter(3+i);
	cerr<< "   # Sigma : "<< sigma <<endl;
	float centroid = -1.*i*gain;

///////////////////////////////
// Half gain for integral
//////////////////////////////
	float half_gain = gain / 2.0;
        int binMin = hMax->GetXaxis()->FindBin(centroid - half_gain );
        int binMax = hMax->GetXaxis()->FindBin(centroid + half_gain );
	areas[i-1] = hMax->Integral(binMin,binMax);

/*
//////////////////////////////
// Nsigmas for integral
/////////////////////////////
        int binMin = hMax->GetXaxis()->FindBin(centroid - nsigma*sigma);
        int binMax = hMax->GetXaxis()->FindBin(centroid + nsigma*sigma);
	areas[i-1] = hMax->Integral(binMin,binMax);
	cerr<< "   # Area (from  "<<binMin<< " to "<<binMax<<" bins) : "<<areas[i-1]<<"\n"<<endl;
*/
	}


	cerr<<"	*** Half gain *** "<<endl;

// Poissonian mean value
/*
	double *mu_s = new double[nPeaks-2];
	double *sigma_mu_s = new double[nPeaks-2];
	cerr<< "\n\n *** Poissonian expectation value *** "<<endl;
	for(int i=2;	i<nPeaks;	i++)
	{
	float tmp_mu = (areas[i-1]/areas[i-2])*(float)i;
	float tmp_sigma_mu = (1./areas[i-2])* TMath::Sqrt( areas[i-1]* ( 1.+ (areas[i-1]/areas[i-2])  )	);
		mu_s[i-2] = tmp_mu;
		sigma_mu_s[i-2] = tmp_sigma_mu;
		cerr<< " /mu_{"<<(i-1)<<"} : "<< tmp_mu<<" +- "<<tmp_sigma_mu<<endl;
	}
	double sigma_mu_mean ;
	double mu_mean = weighted_mean_with_error(mu_s,sigma_mu_s,nPeaks-2,sigma_mu_mean);
*/

cerr<< "\n\n *** Poissonian expectation value *** "<<endl;
        float   mu_s = (areas[1]/areas[0])* 2.;
        float   sigma_mu_s = (1./areas[0])* TMath::Sqrt( areas[1]* ( 1.+ (areas[1]/areas[0])  ) );
                cerr<< " /mu  : "<< mu_s<<" +- "<<sigma_mu_s<<endl;

//ofstream fout; fout.open("Gain_log.txt",ios::app);
ofstream fout; fout.open("Mu_log.txt",ios::app);
//fout<<filename2<<'\t'<<gain<<'\t'<<eGain<<'\t'<<mu_mean<<'\t'<<sigma_mu_mean<<endl;
fout<<filename2<<'\t'<<gain<<'\t'<<eGain<<'\t'<<mu_s<<'\t'<<sigma_mu_s<<endl;
fout.close();
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

