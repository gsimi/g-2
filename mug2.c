#include "TF1.h"
#include "TH1F.h"
#include "TRandom.h"
#include "TVector3.h"
#include "TVector2.h"

/* TOY SIMULATION OF G-2 MEASUREMENT EXPERIMENT
setup:

----------------------- 1 coincidence scintillator planes 
+++++++++++++++++++++++ solenoid
----------------------- 2 coincidence scintillator plane
-----------------------  absorber
----------------------- 3 veto scintillator
+++++++++++++++++++++++ solenoid

*/

//Pdf of the time difference distribution between the 
//coincidence of the incoming mu and the produced electron
//normalized to nsig signal events and nbkg flat background events
double dtpdf(double *x, double *par){
  double t=*x;          //proper time in microseconds
  double nsig=par[0];   //signal events
  double tau=par[1];    //muon lifetime in microseconds
  double alpha=par[2];  //asymmetry factor
  double g=par[3];      //lande' factor
  double omega=par[4];  //predicted larmor frequency in microseconds^-1
  double delta=par[5];  //initial phase
  double nbkg=par[6];   //background events in the window
  double bw=par[7];     //bin width
  double window=par[8]; //fit window
  double sig_norm=1+alpha/(1+tau*tau*omega*omega*g*g/2/2);
  double fsig=exp(-t/tau)/tau*(1+alpha*cos(g/2*omega*t+delta))/sig_norm;
  double fbkg=1./window;
  return bw*(nsig*fsig+nbkg*fbkg);
}

double dtpdf2(double *x, double *par){
  /*
  double t=*x;          //proper time in microseconds
  double nsig=par[0];   //signal events
  */
  double tau=par[1];    //muon lifetime in microseconds
  /*  
  double alpha=par[2];  //asymmetry factor
  double g=par[3];      //lande' factor
  double omega=par[4];  //predicted larmor frequency in microseconds^-1
  double delta=par[5];  //initial phase
  double nbkg=par[6];   //background events in the window
  double bw=par[7];     //bin width
  double window=par[8]; //fit window 
  */
  double tauminus=par[9];    //negative muon lifetime in microseconds


  /*
  double sig_norm=1+alpha/(1+tau*tau*omega*omega*g*g/2/2);
  double fsig=exp(-t/tau)/tau*(1+alpha*cos(g/2*omega*t+delta))/sig_norm;
  double sig_norm_minus=1+alpha/(1+tauminus*tauminus*omega*omega*g*g/2/2);
  double fsig_minus=exp(-t/tauminus)/tauminus*(1+alpha*cos(g/2*omega*t+delta))/sig_norm_minus;
  
  double fbkg=1./window;
  return bw*(nsig*(fsig+fsig_minus)/2+nbkg*fbkg);*/

  double f1=dtpdf(x,par);
  par[1]=tauminus;
  double f2=dtpdf(x,par);
  par[1]=tau;
  return (f1+f2)/2;
}
//build a TF1 object for the dtpdf configured such that the global normalization 
//is 100, the larmor frequency is computed based on the B-field, tau is the lifetime
//Can be used for the positive and the negative muon
//Bfield measured in Gauss = 1e-4 T
//time measured in microseconds = 1e-6 s
TF1 * getpdf(float B, float nsig, float nbkg, float alpha,
	     float tau, float window){
  TF1 *f=new TF1("dtpdf",dtpdf,0,10,9);
  float q=1; //in units of e
  float mmuc2=105; //in MeV
  double c=3; // in units of 1e8 m/s
  float omega=q*B*c*c/mmuc2;// in micro s^-1
  f->SetParameters( nsig,  tau,  alpha,  2   ,omega,    0,       nbkg,      1., window);
  f->SetParNames(  "nsig","tau","alpha","g", "omega_l", "delta", "nbkg", "bw","window");
  return f;
}

TF1 * getpdf2(float B, float nsig, float nbkg, float alpha,
	      float tau, float window, float tauminus){
  TF1 *f=new TF1("dtpdf2",dtpdf2,0,10,10);
  float q=1; //in units of e
  float mmuc2=105; //in MeV
  double c=3; // in units of 1e8 m/s
  float omega=q*B*c*c/mmuc2;// in micro s^-1
  f->SetParameters(nsig,   tau,  alpha,  2   ,omega,    0,        nbkg,    1., window, tauminus);
  f->SetParNames(  "nsig","tau","alpha","g", "omega_l", "delta", "nbkg", "bw","window","tauminus");
  return f;
}

//
//Generate the time difference distribution based on the dtpdf
//for the given 
//  number of entries
//  B field
//  background fraction, 
//  asymmetry parameter alpha
//  negative muon lifetime
//
TH1F* generate(int nsig, float B, int nbkg,
	       float alpha, float tauminus, float tauplus=2.2,
	       float DeltaB=-1){
  int nentries=nsig+nbkg;
  //  float tmin=0,tmax=7;   
  float tmin=1,tmax=tmin+tauplus*5;   
  TF1 *fplus=getpdf(B,nsig,nbkg,alpha,tauplus,tmax-tmin);
  TF1 *fminus=getpdf(B,nsig,nbkg,alpha,tauminus,tmax-tmin);
  TRandom r;
  //  float bw=2*3.1416/fplus->GetParameter(4)/6.6/2;
  float bw=1./fplus->GetParameter(4)/4;
  int nbins=(tmax-tmin)/bw;
  TH1F* h=new TH1F("dt","dt",nbins,tmin,tmax);
  int n=0;
  float fmax=fplus->Eval(0.);
  float fminus_max=fminus->Eval(0.);
  while (n<nentries){
    if (DeltaB>0){
      //      float thisB=r.Gaus(B,DeltaB);
      float thisB=r.Uniform(B-DeltaB,B+DeltaB);
      float q=1,c=3,mmuc2=105;
      float omega=q*thisB*c*c/mmuc2;// in micro s^-1
      fplus->SetParameter(4,omega);
      fminus->SetParameter(4,omega);
      fmax=fplus->Eval(0.);
      fminus_max=fminus->Eval(0.);
    }
    float dt=r.Uniform(tmin,tmax);
    float y=r.Uniform(0,fmax); 
    if (y<fplus->Eval(dt)) {
      h->Fill(dt);
      n++;
      //      cout<<"filled dt "<<dt<<" ";
    }
    float dtminus=r.Uniform(tmin,tmax);
    float yminus=r.Uniform(0,fminus_max); 
    if (yminus<fminus->Eval(dtminus)) {
      //now account for nuclear capture for negative muons
      float BRenu=tauminus/tauplus;
      float p=r.Uniform(0,1);
      if (p<BRenu){
	h->Fill(dtminus);
	n++;
      }
      //      cout<<"filled dt "<<dt<<" ";
    }

  }
  h->GetXaxis()->SetTitle("#mu s");
  char ytit[128]; sprintf(ytit,"# evt/%2.2f s",bw);
  h->GetYaxis()->SetTitle(ytit);
  char tit[128]; sprintf(tit,"time difference distribution, Nele=%d, B=%2.1f G, Nbkg=%d, alpha=%2.1f%% tau_-=%2.2f, ",nentries,B,nbkg,alpha*100,tauminus);
  h->SetTitle(tit);
  h->Draw("e");
  return h;
}

enum fittype{single_exp=1,double_exp=2};
TF1*
fit(TH1F *h, float B,float tauminus=0, fittype ifit=single_exp){
  TF1* f;
  switch (ifit){
  case double_exp:
    f=getpdf2(B,100,10,0.05,2.2,7,1);
    f->FixParameter(9,tauminus);  
  ////  f->SetParameter(9,tauminus);  
  ////    f->SetParLimits(9,0,3);  
    cout<<"fitting with double exponential"<<endl;
    break;
  case single_exp:
  default:
    f=getpdf(B,100,10,0.05,2.2,7);
    cout<<"fitting with single exponential"<<endl;
    break;
  };

  
  //nsig
  float nevents=h->GetEntries();
  f->SetParameter(0,nevents*0.9);
  f->SetParLimits(0,0.,50.*nevents);
  cout<<" fit(): h entries "<<nevents<<endl;

  //tau
  //f->FixParameter(1,2.2);
   f->SetParameter(1,2.2);
   f->SetParLimits(1,1,3);

  //alpha
  f->SetParameter(2,0.05);
  f->SetParLimits(2,-0.1,0.5);
  //f->FixParameter(2,0.05);

  //g
  f->SetParameter(3,2);
  f->SetParLimits(3,1,3);  

  //omega_l
  float q=1; float mmuc2=105; double c=3;
  f->FixParameter(4,q*B*c*c/mmuc2);  

  //delta
  f->SetParameter(5,0);
  f->SetParLimits(5,-3.1416/2,3.1416/2);
  f->FixParameter(5,0);

  //nbkg
  f->SetParameter(6,nevents*0.1);
  f->SetParLimits(6,0,2*nevents);

  //bw
  f->FixParameter(7,h->GetBinWidth(1));

  //window: 
  float tmin=h->GetXaxis()->GetXmin(), tmax=h->GetXaxis()->GetXmax();
   f->FixParameter(8,tmax-tmin);
  f->Print("V");
  //h->Fit(f,"lme","",tmin,tmax);
   h->Fit(f,"","",tmin,tmax);
  return f;
}

double toyfit(int nsig, float B, float nbkg, float alpha, float tauminus, fittype ifit,float DeltaB=-1){
  //  double nevents=nsig/(1-fb);
  cout<<"toyfit(): nsig = "<<nsig<<", nbkg = "<<nbkg<<endl;
  TH1F* h = generate(nsig, B, nbkg, alpha, tauminus,2.2,DeltaB);
  TF1* f = fit(h,B,tauminus,ifit);
  return f->GetParError(3);
}

//distribution of the muon angle of incidence
double generate_mu_costheta(TRandom *r){
  bool missed=true;
  double costheta;
  while (missed){
    costheta=r->Uniform(0,1);
    //this is the costheta distribution
    double y=costheta*costheta, ymax=1;
    double p=r->Uniform(0,1);
    if (p<y/ymax) missed=false;
  }
  return costheta;  
}

//distribution of the muon position
double generate_mu_position(TRandom *r, double W1){
  double mu_pos = r->Uniform(0,W1);
  //cout<<"muon position : "<<mu_pos<<" ";
  return mu_pos;
}

//distribution of electron angle with respecto to the
//muon spin direction
double generate_ele_costheta(TRandom *r){
  bool missed=true;
  double costheta;
  while (missed){
    costheta=r->Uniform(0,1);
    //this is the costheta distribution
    double y=(1+1./3*costheta), ymax=1+1./3;
    double p=r->Uniform(0,ymax);
    if (p<y) missed=false;
  }
  return costheta;  
}

//electron energy spectrum
double ele_energy(TRandom *r){
  bool missed=true;
  double energy=-1;
  while (missed){
    energy=r->Uniform(0.,52.8);
    //this is the energy distribution
    double y=energy, ymax=52.8;
    double p=r->Uniform(0,ymax);
    if (p<y) missed=false;
  }
  return energy;
}

TH1F* h_ele_e(int nentries){
  TH1F* h = new TH1F("e_ele","generated electron energy spectrum",100,0,52.8);
  TRandom *r=new TRandom;
  for (int i=0;i<nentries;i++)
    h->Fill(ele_energy(r));
  return h;
}

/* class Absorber{ */
/*  public: */
/*   Absorber(const char* absorber, double &thickness, double& dedx, double &rho, double &tauminus) */
/* }; */

class Config{
 public:
  Config(char *absorber,int geometry);
  ~Config(){};
  double dedx;
  double rho;
  double tauminus;//mus
  double thickness;//cm
  fittype ifit;
  double W13;
  double W2;
  double d1;
  double d2;
  double d1_2;
  double d2_3;
  double d2_a;
 private:
};
Config::Config(char *absorber, int geometry){

  /* thickness is the default thickness of the absorber. It is
     defined such that the energy loss in different materials is the same.
     The absolute normalization is given by the thickness of the copper absorber
     set so that the energy loss is 31MeV */

  if (strcmp(absorber, "Cu")==0){
    // copper
    dedx=1.4;
    rho=8.9;
    tauminus=0.16;//mus
    thickness=2.5;//cm
    ifit=single_exp;
  }
  if (strcmp(absorber,"Al")==0){
    // aluminium
    dedx=1.6;
    rho=2.7;
    tauminus=0.88;//mus
    thickness=2.5*8.9*1.4/rho/dedx;
    ifit=double_exp;
  }
  if (strcmp(absorber,"Sci")==0){
    dedx=1.03;
    rho=2.0;
    tauminus=2.03;//mus
    thickness=2.5*8.9*1.4/rho/dedx;
    ifit=double_exp;
  }
  if (strcmp(absorber,"Pb")==0){
    dedx=1.12;
    rho=11.35;
    tauminus=75./1000.;//mus
    thickness=2.5*8.9*1.4/rho/dedx;
    ifit=single_exp;
  }
  if (geometry == 1){
    W13 = 50;       //width of scintillators 1 and 3[cm]
    W2 = 50;       //width of scintillator 2[cm]
    d1 = 1;       //thickness of scintillator 1 [cm]
    d2 = 1;       //thickness of scintillator 2 [cm]
    d1_2 = 8;     //distance between scintillators 1 and 2
    d2_3 = 3;    //distance between scintillators 2 and 3
    d2_a = 0.25;    // distance between scintillator 2 and the absorber
  }
}

/* void config(const char* absorber, double &thickness, double& dedx, double &rho, double &tauminus, */
/* 	    fittype& ifit){ */

/*   /\* thickness is the default thickness of the absorber. It is */
/*      defined such that the energy loss in different materials is the same. */
/*      The absolute normalization is given by the thickness of the copper absorber */
/*      set so that the energy loss is 31MeV *\/ */

/*   if (strcmp(absorber, "Cu")==0){ */
/*     // copper */
/*     dedx=1.4; */
/*     rho=8.9; */
/*     tauminus=0.16;//mus */
/*     thickness=2.5;//cm */
/*     ifit=single_exp; */
/*   } */
/*   if (strcmp(absorber,"Al")==0){ */
/*     // aluminium */
/*     dedx=1.6; */
/*     rho=2.7; */
/*     tauminus=0.88;//mus */
/*     thickness=2.5*8.9*1.4/rho/dedx; */
/*     ifit=double_exp; */
/*   } */
/*   if (strcmp(absorber,"Sci")==0){ */
/*     dedx=1.03; */
/*     rho=2.0; */
/*     tauminus=2.03;//mus */
/*     thickness=2.5*8.9*1.4/rho/dedx; */
/*     ifit=double_exp; */
/*   } */
/*   if (strcmp(absorber,"Pb")==0){ */
/*     dedx=1.12; */
/*     rho=11.35; */
/*     tauminus=75./1000.;//mus */
/*     thickness=2.5*8.9*1.4/rho/dedx; */
/*     ifit=single_exp; */
/*   } */
/* } */


/* Function checking if the muon, given its position, angle, the absorber and the geometry of the experiment, is likely to hit : 
- scintillator 2 (level 1)
- the absorber (level 2)
- scintillator 3 (level 3)
 */
enum targettype{scint1,scint2,absor,scint3};

bool check_muon(double mu_pos, double theta, Config cfg, targettype target){
  double theta_min = 0; 
  double theta_max = 0;
  bool check = false;


  switch (target){
  case scint2: {
    //case 1 : muon at the left of scintillator 2
    if (mu_pos < ((cfg.W13-cfg.W2)/2)){
      theta_min = atan(((cfg.W13-cfg.W2)/2 - mu_pos) / (cfg.d1+cfg.d2+cfg.d1_2));
      theta_max = atan(((cfg.W13+cfg.W2)/2 - mu_pos) / (cfg.d1+cfg.d1_2));
    }
    //case 2 : muon at the right of scintillator 2
    if (mu_pos > ((cfg.W13+cfg.W2)/2)){
      theta_min = atan(((cfg.W13-cfg.W2)/2 - mu_pos) / (cfg.d1+cfg.d1_2));
      theta_max = atan(((cfg.W13+cfg.W2)/2 - mu_pos) / (cfg.d1+cfg.d2+cfg.d1_2));
    }
    //case 3 : muon above scintillator 2
    if ((mu_pos >= (cfg.W13-cfg.W2)/2) && (mu_pos <= (cfg.W13+cfg.W2)/2)){
      theta_min = atan(((cfg.W13-cfg.W2)/2 - mu_pos) / (cfg.d1+cfg.d1_2));
      theta_max = atan(((cfg.W13+cfg.W2)/2 - mu_pos) / (cfg.d1+cfg.d1_2));  
    }
    break;
  }
  case absor: {
  double d = cfg.d1+cfg.d2+cfg.d1_2+cfg.d2_a;
  theta_min = atan(-mu_pos/d);
  theta_max = atan((cfg.W13-mu_pos)/d);
    break;
  }
  case scint3: {
  double D = cfg.d1+cfg.d1_2+cfg.d2+cfg.d2_3;
  theta_min = atan(-mu_pos/D);
  theta_max = atan((cfg.W13-mu_pos)/D);
    break;
  }
  default:
  cout<<"target must be scint2, scint3 or absorber"<<endl;
  }

  if ((theta>theta_min) && (theta<theta_max)){
    check = true;
  }
  //cout<<"the result of the test is : "<<check<<" ";
  return check;
}

bool check_ele(double ele_x, double ele_y, double ele_theta, Config cfg, targettype target){
  double theta_min = 0; 
  double theta_max = 0;
  bool check = false;
 

  switch (target){
  case scint2: {
    doube d = cfg.d2+cfg.d2_a+ele_y;
    //case 1 : electron at the right of scintillator 2
    if (ele_x > ((cfg.W13+cfg.W2)/2)){
      theta_min = atan((ele_x-(cfg.W13+cfg.W2)/2)/d);
      theta_max = atan((ele_x-(cfg.W13-cfg.W2)/2)/(cfg.d2_a+ele_y));
    }
    //case 2 : electron at the left of scintillator 2
    if (ele_x < ((cfg.W13-cfg.W2)/2)){
      theta_min = atan((ele_x-(cfg.W13+cfg.W2)/2)/(cfg.d2_a+ele_y));
      theta_max = atan((ele_x-(cfg.W13-cfg.W2)/2)/d);
    }
    //case 3 : electron below scintillator 2
    if ((ele_x >= (cfg.W13-cfg.W2)/2) && (ele_x <= (cfg.W13+cfg.W2)/2)){
      theta_min = atan((ele_x-(cfg.W13+cfg.W2)/2)/(cfg.d2_a+ele_y));
      theta_max = atan((ele_x-(cfg.W13-cfg.W2)/2)/(cfg.d2_a+ele_y));  
    }
    break;
  }
  case scint1: {
  double D =cfg.d1_2+cfg.d2+cfg.d2_a+ele_y;
  theta_min = atan((ele_x-cfg.W13)/D);
  theta_max = atan(ele_x/D);
    break;
  }
  default:
  cout<<"target must be scint1 or scint2"<<endl;
  }

  if ((ele_theta>theta_min) && (ele_theta<theta_max)){
    check = true;
  }
  //cout<<"the result of the test is : "<<check<<" ";
  return check;
}

/* 
   -nmu is the number of muons incident on the absorber
   -muons are stopped in the absorber and electrons generated
   with simplfied energy distribution
   -stopping power proportional to thickness * dedx *rho
   -The number of signal events is determined requiring 
   that electrons exit the absorber
 */
double generate_nsig(int nmu, char* absorber, double thickness){
  Config c(absorber,1);
  //reference_pstop is that of 2.5 cm of copper (from Amsler)
  float reference_pstop=1e-2;
  float pstop=reference_pstop*thickness*c.dedx*c.rho/(2.5*1.4*8.9); 

  TRandom r;
  TH1F* h = new TH1F("e_ele","ele energy",100,0,52.8);  
  for (int i=0;i<nmu*pstop;i++){
    //generate the direction of the muon spin
    float mu_costheta=generate_mu_costheta(&r);
    TVector3 vmu(0,0,1); 
    vmu.RotateX(acos(mu_costheta));
    //generate direction of the elctron momentum
    float ele_costheta=generate_ele_costheta(&r);
    float ele_phi=r.Uniform(-3.1416,3.1416);
    TVector3 vele(vmu);
    vele.RotateX(acos(ele_costheta));
    vele.Rotate(ele_phi,vmu);

    /* discard downgoing electrons and electrons stopped in the absorber */
    if (vele.CosTheta()<0) continue; 
    //generate the position where the muon has stopped
    float y=r.Uniform(0,thickness);
    //length travelled by the electron to exit the slab
    double l=(thickness-y)/vele.CosTheta();
    //energy lost by electron
    double deltaE=l*c.rho*c.dedx;
    double eleE=ele_energy(&r);
    if (eleE<deltaE) continue;
    h->Fill(eleE);
  }
  h->Draw();
  cout<<h->GetEntries()<<endl;
  double nele=h->GetEntries();
  printf("fraction of good events %2.2f/%d = %2.2f%% \n",nele,nmu,nele/nmu*100);
  return nele;
}

//3rd version of generate_nsig : return a vector (nsig, nbkg)
TVector2 generate_nsig_nbkg(int nmu, char* absorber, double thickness, double W2, double d2_3){
  TVector2 res(0,0);
  Config cfg(absorber,1);
  cfg.W2 = W2;
  cfg.d2_3 = d2_3;
  //reference_pstop is that of 2.5 cm of copper (from Amsler)
  /* float reference_pstop=1e-2; */
  /* float pstop=reference_pstop*thickness*dedx*rho/(2.5*1.4*8.9);  */
  float p_absorb_per_MeV=0.01/10; // 1%/10MeV 
  float stop=cfg.dedx*cfg.rho*p_absorb_per_MeV;
  targettype scint2 = scint2;
  targettype absor = absor;
  targettype scint3 = scint3;
  double d = cfg.d1+cfg.d2+cfg.d1_2+cfg.d2_a;
  TRandom r;
  TH1F* h = new TH1F("e_ele","ele energy",100,0,52.8);
  double n_bkg = 0;
  double n_mu_abs = 0;
  for (int i=0;i<nmu;i++){
    //muon position and angle at the top of scintillator 1, between 0 and W1
    double mu_pos = generate_mu_position(&r,cfg.W13);
    int b = floor(r.Uniform(0,10));
    double mu_theta = pow(-1,b)*acos(generate_mu_costheta(&r));  //theta can be either positive or negative
    //cout<<"theta ="<<mu_theta<<" ";
    if (check_muon(mu_pos,mu_theta,cfg,scint2)){
      if (!check_muon(mu_pos,mu_theta,cfg,absor)){
	n_bkg++;
      }
      else {
	if (!check_muon(mu_pos,mu_theta,cfg,scint3)){
	  n_bkg++;
	}
	else { //is a signal candidate
	  n_mu_abs++;
	  double a = r.Uniform(0,1);
	  if (a<=stop){
	    //generate the direction of the muon spin
	    //float mu_costheta=generate_mu_costheta(&r);
	    float mu_costheta = cos(mu_theta);
	    TVector3 vmu(0,0,1); 
	    vmu.RotateX(acos(mu_costheta));
	    //generate direction of the elctron momentum         
	    float ele_costheta=generate_ele_costheta(&r);
	    float ele_phi=r.Uniform(-3.1416,3.1416);
	    TVector3 vele(vmu);
	    vele.RotateX(acos(ele_costheta));
	    vele.Rotate(ele_phi,vmu);
	    /* discard downgoing electrons and electrons stopped in the absorber */
	    if (vele.CosTheta()<0) {
	      n_bkg++;
	      continue;
	    }
	    //generate the position where the muon has stopped
	    float ele_y=r.Uniform(0,thickness);
	    float ele_x = mu_pos+(d+ele_y)*tan(mu_theta);
	    //length travelled by the electron to exit the slab
	    double l=(thickness-ele_y)/vele.CosTheta();
	    //energy lost by electron
	    double deltaE=l*cfg.rho*cfg.dedx;
	    double eleE=ele_energy(&r);
	    if (eleE<deltaE) continue;
	    h->Fill(eleE);
	  }
	}
      }
    }
  }
  cout<<"muon hitting the absorber ="<<n_mu_abs<<" ";
  cout<<"muon bkg ="<<n_bkg<<" ";
  h->Draw();
  //cout<<h->GetEntries()<<endl;
  double nele=h->GetEntries();
  cout<<"true muons ="<<nele<<" ";
  //printf("fraction of good events %2.2f/%d = %2.2f%% \n",nele,nmu,nele/nmu*100);
  res.Set(nele,n_bkg);
  return res;
}



double toy(double nweeks, double B, double nbkg, double alpha, char *absorber, float thickness){
  // double dedx, rho, tauminus, thickness_nom;
  //fittype ifit;
  //config(absorber,thickness_nom, dedx,rho,tauminus,ifit);
  Config cfg(absorber,1);
  float ndays=nweeks*7;
  float time=ndays*24*3600;//in seconds
  float nmu=time*7;//muons in the acceptance. 
  /* 7 Hz rate is an estimate from Amsler assuming rescaling their rate 
     for muons to an area of 60cm x 60cm area */
  float nele= generate_nsig_nbkg(nmu,absorber,thickness,cfg.W2,cfg.d2_3).X() ;//nominal number of signal events;
  
  double gm2err=toyfit(nele,B,nbkg,alpha,cfg.tauminus,cfg.ifit);
  cout<<"nele ="<<nele<<endl;
  cout<<"gm2err = "<<gm2err<<endl;  
 return gm2err;  
}

void tables(char* absorber="Cu", double W2=50, double d2_3=10){
  //double dedx, rho, tauminus, thickness_nom; fittype ifit;
  //config(absorber,thickness_nom, dedx,rho,tauminus,ifit);
  Config cfg(absorber,1);
  cfg.W2 = W2;
  cfg.d2_3 = d2_3;
  float tauplus=2.2;
  /* 
     first: find the number of signal events expected with this configuration 
  */
  float nweeks=1;
  float ndays=nweeks*7;
  float time=ndays*24*3600;//in seconds
  float nmu=time*7;//muons in the acceptance. 
  /* 7 Hz rate is an estimate from Amsler assuming rescaling their rate 
     for muons to an area of 60cm x 60cm area */
  float ns_nom= generate_nsig_nbkg(nmu,absorber,cfg.thickness,cfg.W2,cfg.d2_3).X() ;//nominal number of signal events;
  cout<<" nominal number of signal events in "<<ndays<<" days = "<<ns_nom<<endl;
  //float nb_nom=1e3*nweeks; // nominal number of background events (guess)
  float nb_nom=generate_nsig_nbkg(nmu,absorber,cfg.thickness,cfg.W2,cfg.d2_3).Y();
  /*
    second: generate the distribution of the time differences
    for a given value of B field, background fractoin, measured asymmetry,
    thickness and fit to extract the error on the g-2
  */
  float B_nom=55; //nominal B field
  //  float fb_nom=0.1; //nominal background fraction
  float alpha_nom=0.05; //nominal asymmetry parameter
  double B[15]={20,25,30,35,40,45,50,B_nom,60,65,70,75,80,100,120};
  double gm2err_B[15];
  for (int i=0;i<15;i++)
    gm2err_B[i]=toyfit(ns_nom,B[i],nb_nom,alpha_nom,cfg.tauminus,cfg.ifit);

  double fb[8]={0.01,0.05,0.10,0.20,0.40,0.50,0.55,0.58};
  double gm2err_fb[8];
  for (int i=0;i<8;i++){
    float nbkg=fb[i]/(1-fb[i])*ns_nom;
    gm2err_fb[i]=toyfit(ns_nom,B_nom,nbkg,alpha_nom,cfg.tauminus,cfg.ifit);
  }

  int ns[6]={5000,20000,50000,200000,500000,1000000};
  double gm2err_ns[6];
  for (int i=0;i<6;i++)
    gm2err_ns[i]=toyfit(ns[i],B_nom,nb_nom,alpha_nom,cfg.tauminus,cfg.ifit);

  double alpha[8]={5e-3,1e-2,2e-2,3e-2,4e-2,5e-2,1e-1,2e-1};
  double gm2err_alpha[8];
  for (int i=0;i<8;i++)
    gm2err_alpha[i]=toyfit(ns_nom,B_nom,nb_nom,alpha[i],cfg.tauminus,cfg.ifit);

  double thickness[10]={5e-1,1,1.5,2,2.5,3,5,10,20,40};
  double gm2err_thickness[10];
  double eff_thickness[10];
  float tmin=1,tmax=tmin+tauplus*5;   
  float frac_fit=
    0.5*(exp(-tmin/cfg.tauminus)-exp(-tmax/cfg.tauminus))+
    0.5*(exp(-tmin/tauplus)-exp(-tmax/tauplus));
    
  for (int i=0;i<10;i++){
    float nsig= generate_nsig_nbkg(nmu,absorber,thickness[i],cfg.W2,cfg.d2_3).X() ;//nominal number of signal events;
    gm2err_thickness[i]=toyfit(nsig,B_nom,nb_nom,alpha_nom,cfg.tauminus,cfg.ifit);
    eff_thickness[i]=nsig/nmu*frac_fit;
  }
  /*
  //scintillator 2 width dependency
  double scint2_width1[10] = {10,15,20,25,30,35,40,45,50,55};
  double eff_width[10];
  for (int i=0;i<10;i++){
    float nsig= generate_nsig_nbkg(nmu,absorber,c.thickness,scint2_width1[i],d2_3).X() ;//nominal number of signal events; 
    eff_width[i]=nsig/nmu;
  }
  double frac_bkg[10];
  for(int i=0;i<10;i++){
float nbkg = generate_nsig_nbkg(nmu,absorber,c.thickness,scint2_width[i],d2_3).Y() ;
 frac_bkg[i] = nbkg/nmu;
 }
  //d2_3 depedency
  double scint_gap[10] = {5.5,6,6.5,7,7.5,8,8.5,9,9.5,10};
  double eff_gap[10];
  for(int i=0;i<10;i++){
   float nsig= generate_nsig_nbkg(nmu,absorber,c.thickness,W2,scint_gap[i]).X() ;
   eff_gap[i] = nsig/nmu;
   }
  double bkg_gap[10];
  for(int i=0;i<10;i++){
   float nbkg= generate_nsig_nbkg(nmu,absorber,c.thickness,W2,scint_gap[i]).Y() ;
   bkg_gap[i] = nbkg/nmu;
   }
  double scint2_width[10] = {2.5,7.5,12.5,17.5,22.5,27.5,32.5,37.5,42.5,47.5};
  double gm2err_W2[10];  
  for (int i=0;i<10;i++){
    float nsig = generate_nsig_nbkg(nmu,absorber,c.thickness,scint2_width[i],d2_3).X();
    float nbkg = generate_nsig_nbkg(nmu,absorber,c.thickness,scint2_width[i],d2_3).Y();
    gm2err_W2[i] = toyfit(nsig,B_nom,nbkg,alpha_nom,c.tauminus,c.ifit);
  }
  double scint_gap[10] = {0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5,9.5};
  double gm2err_d2_3[10];  
  for (int i=0;i<10;i++){
    float nsig = generate_nsig_nbkg(nmu,absorber,c.thickness,W2,scint_gap[i]).X();
    float nbkg = generate_nsig_nbkg(nmu,absorber,c.thickness,W2,scint_gap[i]).Y();
    gm2err_d2_3[i] = toyfit(nsig,B_nom,nbkg,alpha_nom,c.tauminus,c.ifit);
  }
  */
  double Bsigma[10]={0, 1./100, 2./100, 3./100, 3.5/100, 4./100, 4.5/100, 5./100, 6./100, 7./100};
  double gm2err_Bsigma[10];
  for (int i=0;i<10;i++){
    float DeltaB=B_nom*Bsigma[i]*sqrt(12);
    gm2err_Bsigma[i]=toyfit(ns_nom,B_nom,nb_nom,alpha_nom,cfg.tauminus,cfg.ifit,DeltaB);
  }

  cout<<"B g-2_error_vs_B_"<<absorber<<endl;
  for (int i=0;i<15;i++)
    cout<<B[i]<<"\t"<<gm2err_B[i]<<endl;

  cout<<endl;
  cout<<"fb g-2_error_vs_fb_"<<absorber<<endl;
  for (int i=0;i<8;i++)
    cout<<fb[i]<<"\t"<<gm2err_fb[i]<<endl;

  cout<<endl;
  cout<<"ns g-2_error_vs_ns_"<<absorber<<endl;
  for (int i=0;i<6;i++)
    cout<<ns[i]<<"\t"<<gm2err_ns[i]<<endl;

  cout<<endl;
  cout<<"alpha g-2_error_vs_alpha_"<<absorber<<endl;
  for (int i=0;i<8;i++)
    cout<<alpha[i]<<"\t"<<gm2err_alpha[i]<<endl;

  cout<<endl;
  cout<<"thickness err_vs_t_"<<absorber<<endl;
  for (int i=0;i<10;i++)
    cout<<thickness[i]<<"\t"<<gm2err_thickness[i]<<endl;

  cout<<endl;
  cout<<"thickness eff_vs_t_"<<absorber<<endl;
  for (int i=0;i<10;i++)
    cout<<thickness[i]<<"\t"<<eff_thickness[i]<<endl;


  cout<<endl;
  cout<<"Bsigma g-2_error_vs_Bsigma_"<<absorber<<endl;
  for (int i=0;i<10;i++)
    cout<<Bsigma[i]<<"\t"<<gm2err_Bsigma[i]<<endl;
  /*
  cout<<endl;
  cout<<"Detection_efficiency_vs_W2_"<<absorber<<endl;
  for (int i=0;i<10;i++)
    cout<<scint2_width1[i]<<"\t"<<eff_width[i]<<endl;

  cout<<endl;
  cout<<"Background_frac_vs_W2_"<<absorber<<endl;
  for (int i=0;i<10;i++)
    cout<<scint2_width[i]<<"\t"<<frac_bkg[i]<<endl;

  cout<<endl;
  cout<<"Detection_efficiency_vs_d2_3_"<<absorber<<endl;
  for (int i=0;i<10;i++)
    cout<<scint_gap[i]<<"\t"<<eff_gap[i]<<endl;

  cout<<endl;
  cout<<"Background_frac_vs_d2_3_"<<absorber<<endl;
  for (int i=0;i<10;i++)
    cout<<scint_gap[i]<<"\t"<<bkg_gap[i]<<endl;
  
  cout<<endl;
  cout<<"W2 g-2_error_vs_W2_"<<absorber<<endl;
  for (int i=0;i<10;i++)
    cout<<scint2_width[i]<<"\t"<<gm2err_W2[i]<<endl;

  cout<<endl;
  cout<<"d2_3 g-2_error_vs_d2_3_"<<absorber<<endl;
  for (int i=0;i<10;i++)
    cout<<scint_gap[i]<<"\t"<<gm2err_d2_3[i]<<endl;
  */
}
