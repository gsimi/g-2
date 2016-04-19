/*
  Gabriele Simi, University of Padova, 2016

  A macro to fit the pulseheigth spectrum of a SiPM
  
  Depends on spectrfitf.cc 
  
  Usage:

  .L fitscan.cc
  fitscan("ph-sipm-xxx-32.3.csv")

*/

#include "TH1F.h"
#include "TF1.h"
#include "TSpectrum.h"

#include <algorithm>
using namespace std;

TF1* fitscan(const char * csvname){
  gROOT->ProcessLine(".L readcsv.C");
  csvreader *csv = new csvreader(csvname);
  return fitscan(csv->getHistogram());
}

TF1*
fitscan(TH1F* h, 
	double fmin=0, double fmax=1,double width=40e-9 ){

  gROOT->ProcessLine(".L spectrfitf.cc+");

  /*

    Extract parametrs from histogram

  */
  double bw=h->GetBinWidth(1);
  int dim=h->GetNbinsX();
  double norm=h->Integral(); 
  double xmin=h->GetXaxis()->GetXmin();
  double xmax=h->GetXaxis()->GetXmax();
  //compute fit limits
  int jxmin=int(fmin*dim); int jxmax=int(fmax*dim)-1;
  if (jxmax>dim-1) {cout<<"max out of range"<<endl; return 0;}

  /*

    Search for peaks to compute initial values  

  */
  
  TSpectrum pf(8,1e-3);
  pf.Search(h,2,"nobackground");
  pf.Print("V");

  /*

    compute initial values of noise, gain, mean, offset

  */
  
  const int npeaks=pf.GetNPeaks();
  double gain=40,offset=7,noise=2.4,mu=0.28;
  if (npeaks>1){
    double x[npeaks];
    int ix[npeaks];
    for (int i=0;i<npeaks;i++) x[i]=pf.GetPositionX()[i];
    sort(x,x+npeaks-1);
    double pxmin=x[0],pxmax=x[npeaks-1];
    gain=(pxmax-pxmin)/(npeaks-1);
    offset=pxmin;
    mu=pf.GetPositionY()[1]/pf.GetPositionY()[0];//incorrect...
    if (npeaks>2) {
      pxmin=x[1]; pxmax=x[npeaks-1];
      gain=(pxmax-pxmin)/(npeaks-2);
      offset=pxmin-gain;
      mu=pf.GetPositionY()[2]/pf.GetPositionY()[0];//incorrect...      
    }
    noise=gain/10.;
  }


  /*

    initialize fit function

  */
  
  int npar=12;
  TF1 *f=new TF1("spectrfit",fitf_g2,xmin,xmax,npar);
  f->SetParNames(  "mu","gain","gNoise","offset","iNoise","ln_norm","ct","g2p","g2off","g2sigma");//,"eff","bw");
  f->SetParameters(mu,gain     ,noise  ,offset       ,noise     ,log(norm)  ,0.03, 0.1, gain/5,2*noise);

  f->SetParLimits(0,0,20*mu); //mu
  f->SetParLimits(1,bw,10*gain); //gain
  f->SetParLimits(2,bw/5,50*bw); //gnoise
  f->SetParLimits(3,-100*bw,100*bw);//offset
  //  f->SetParLimits(4,bw/50,50*bw);//inoise
  //  f->FixParameter(4,0);//inoise
  f->SetParLimits(5,0.5*log(norm),1.5*log(norm));//norm
  f->SetParLimits(6,0,0.9);//ct
  //  f->FixParameter(6,0);//ct fixed
  f->SetParLimits(7,0,0.9);//g2p
  //  f->FixParameter(7,0);
  f->SetParLimits(8,0,gain);//g2off
  //  f->FixParameter(8,0);
  f->SetParLimits(9,bw/5,50*bw);//g2sigma
  //  f->FixParameter(9,bw);
  f->FixParameter(10,1);//eff 
  f->FixParameter(11,bw);//bw

  cout<<"initial fit parameters:"<<endl;
  double pmin,pmax;
  for (int i=0;i<npar;i++){
    f->GetParLimits(i,pmin,pmax);
    printf("%s\t = %.3f \t [%.3f;%.3f]\n",f->GetParName(i),f->GetParameter(i),pmin,pmax);
  }

  /*

    perform fit

  */
  cout<<"fitting spectrum in ["<<xmin<<";"<<xmax<<"] ..."<<endl;
  h->Fit(f,"l","",xmin,xmax);
  cout<<"done"<<endl;
  //Draw
  //  h->SetMinimum(0.1);
  //  h->Draw("");

  return f;
}



