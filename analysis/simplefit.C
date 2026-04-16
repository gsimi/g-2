void readfiles(TTree &t){
  char fname[128]="g-2023.";
  int month=2;
  char basedir[128]="/home/lhcb/g-2/data/2022";
  for (int day=23;day<=28;day++){
    sprintf(fname,"%s/g-2023.%02d.%02d.dat",basedir,month,day);
    t.ReadFile(fname,"dt");
    cout<<"reading "<<fname<<endl;
  }
  int month=3;
  for (int day=1;day<=30;day++){
    sprintf(fname,"%s/g-2023.%02d.%02d.dat",basedir,month,day);
    t.ReadFile(fname,"dt");
    cout<<"reading "<<fname<<endl;
  }

}

void readfiles23(TTree &t, int istart=0, int nfiles=-1){
  char *farray[365];
  int ifile=0;
  int month=6;
  for (int day=27;day<=30;day++){
    char *fname=new char[128];
    sprintf(fname,"test-2023.%02d.%02d.dat",month,day);
    farray[ifile++]=fname;
  }
  int month=7;
  for (int day=1;day<=31;day++){
    char *fname=new char[128];
    sprintf(fname,"test-2023.%02d.%02d.dat",month,day);
    farray[ifile++]=fname;
  }
  int month=8;
  for (int day=1;day<=28;day++){
    char *fname=new char[128];
    sprintf(fname,"test-2023.%02d.%02d.dat",month,day);
    farray[ifile++]=fname;
  }
  int istop=istart+nfiles;
  if (nfiles<0){
    istop=ifile;
  }
  for (int i=istart;i<istop;i++){
    t.ReadFile(farray[i],"dt");
    cout<<"reading "<<farray[i]<<endl;
  }
}

void readfiles_natale23(TTree &t, int istart=0, int nfiles=-1){
  int month=12;
  char *farray[365];
  int ifile=0;
  for (int day=21;day<=31;day++){
    char *fname=new char[128];
    sprintf(fname,"natale/dati-2023.%02d.%02d.dat",month,day);
    farray[ifile++]=fname;
  }
  int month=01;
  for (int day=01;day<=31;day++){
    char *fname=new char[128];
    sprintf(fname,"natale/dati-2024.%02d.%02d.dat",month,day);
    farray[ifile++]=fname;
  }
  
  int month=02;
  for (int day=01;day<=29;day++){
    char *fname=new char[128];
    sprintf(fname,"natale/dati-2024.%02d.%02d.dat",month,day);
    farray[ifile++]=fname;
  }
  int month=03;
  for (int day=01;day<=05;day++){
    char *fname=new char[128];
    sprintf(fname,"natale/dati-2024.%02d.%02d.dat",month,day);
    farray[ifile++]=fname;
  }
  int istop=istart+nfiles;
  if (nfiles<0) {
    istop=ifile;
  }
  int ntot=0;
  for (int i=istart;i<istop;i++){
    ntot+=t.ReadFile(farray[i],"dt");
    cout<<"reading "<<farray[i]<<endl;
  }
  cout<<"Read "<<ntot<<" events"<<endl;
}

/* used for unbinned fit */
double pi=3.141592;
double ffit(double* x, double *p){
  double t=*x;
  double NS=p[0];
  double tau=p[1];
  double alpha=p[2];
  double omega=p[3];
  double phioverpi=p[4];
  double NB=p[5];
  double bw=p[6];
  double t1=p[7];
  double t2=p[8];
  double phi=pi*phioverpi;
  double valsig= exp(-t/tau)*(1+alpha*cos(omega*t+phi));
  double normsig=tau*(
		      exp(-t1/tau)-
		      exp(-t2/tau)+
		      1./(1+pow(omega*tau,2))*alpha*(
						     exp(-t1/tau)*cos(phi+omega*t1)-
						     omega*tau*sin(phi+omega*t1)+
						     exp(-t2/tau)*(
								   -1*cos(phi+omega*t2)+
								   omega*tau*sin(phi+omega*t2)
								   )
						     )
		       );
  
  //  double normsig=1;
  double valbkg=1;
  double normbkg=1/(t2-t1);
  //  double normbkg=1;
  return NS*bw*valsig/normsig+NB*bw*valbkg/normbkg;
}

TF1* configfit(TH1F *ht, double fitmin, double fitmax){
  TF1 *fpg2;
  fpg2 = ((TF1*)gROOT->FindObject("myexpcos")); if (fpg2!=0) delete fpg2;  
  /*  
   fpg2=new TF1("myexpcos","[0]/[6]*exp(-x/[1])/[1]*(1+[2]*cos([3]*x+[4]*3.141592))+[5]/[6]", 0,100);
   fpg2->SetParNames("NS","tau","alpha","omega","phioverpi","NB","bw");
   */
  fpg2=new TF1("myexpcos",ffit,fitmin,fitmax,9);
  fpg2->SetParNames("NS","tau","alpha","omega","phioverpi","NB","bw","fitmin","fitmax");
   
  fpg2->SetTitle(" Modulated Exponential Binned Fit ");

  double NS=ht->Integral(ht->GetBin(fitmin),ht->GetBin(fitmax));
  fpg2->SetParameter("NS",NS);
  fpg2->SetParLimits(fpg2->GetParNumber("NS"),0,NS*2);
  //
  fpg2->SetParameter("tau",2.2);
  fpg2->SetParLimits(fpg2->GetParNumber("tau"),1,3.5);  
  //  fpg2->FixParameter(fpg2->GetParNumber("tau"),2.2);
  //
  fpg2->SetParameter("alpha",0.05);
  fpg2->SetParLimits(fpg2->GetParNumber("alpha"),0,1.);
  //  
  fpg2->SetParameter("omega",4.7);
  fpg2->SetParLimits(fpg2->GetParNumber("omega"),4.,5.2);
  //
  fpg2->SetParameter("phioverpi",0.);
  fpg2->SetParLimits(fpg2->GetParNumber("phioverpi"),-1,1.);
  //FIXED
  //fpg2->FixParameter(fpg2->GetParNumber("phioverpi"),0);
  //
  fpg2->SetParameter("NB",0.);
  fpg2->SetParLimits(fpg2->GetParNumber("NB"),-1*ht->GetEntries()/5,ht->GetEntries()/5);
  //FIXED
  double bw=ht->GetBinWidth(0);
  fpg2->FixParameter(fpg2->GetParNumber("bw"),bw);
  //
  fpg2->FixParameter(fpg2->GetParNumber("fitmin"),fitmin);
  fpg2->FixParameter(fpg2->GetParNumber("fitmax"),fitmax);
  return fpg2;
}

// cannot properly normlize the function in the fitting interval
TF1* configunbinnedfit(TH1F *ht, double fitmin, double fitmax){
  cout<<" WARNING: CANNOT PROPERLY NORMAILZE THE FIT FUNCTION"<<endl;
  TF1* fpg2=new TF1("myexpcos", ffit, fitmin, fitmax, 9);
  fpg2->SetTitle(" Modulated Exponential Unbinned Fit ");
  fpg2->SetParNames("NS","tau","alpha","omega","phioverpi","NB","bw","fitmin","fitmax");
  fpg2->FixParameter(fpg2->GetParNumber("NS"),ht->GetEntries());
  fpg2->SetParLimits(fpg2->GetParNumber("NS"),0,ht->GetEntries()*2);
  //
  fpg2->SetParameter("tau",2.2);
  fpg2->SetParLimits(fpg2->GetParNumber("tau"),1,3.5);  
  //
  fpg2->SetParameter("alpha",0.0);
  fpg2->SetParLimits(fpg2->GetParNumber("alpha"),0,1.);
  //  
  fpg2->SetParameter("omega",4.7);
  fpg2->SetParLimits(fpg2->GetParNumber("omega"),4.,5.2);
  //
  fpg2->SetParameter("phioverpi",0.);
  fpg2->SetParLimits(fpg2->GetParNumber("phioverpi"),-1,1.);
  //FIXED
  //fpg2->FixParameter(fpg2->GetParNumber("phioverpi"),0);
  //
  fpg2->SetParameter("NB",0.);
  fpg2->SetParLimits(fpg2->GetParNumber("NB"),-1*ht->GetEntries()/5,ht->GetEntries()/5);
  //FIXED
  double bw=ht->GetBinWidth(0);
  fpg2->SetParameter("bw",bw);
  fpg2->FixParameter(fpg2->GetParNumber("bw"),bw);
  //
  fpg2->FixParameter(fpg2->GetParNumber("fitmin"),fitmin);
  fpg2->FixParameter(fpg2->GetParNumber("fitmax"),fitmax);
  return fpg2;
}


double* computeg2(TF1* fp, TF1* fpg2){
  /* formula per la freq. ang. di precessione omega=g*e*B*c*c/(2*mmuc2); */
  //calcolo del campo magnetico
  double mu0=4*3.1415926535e-7;// T*m/A
  double n=560/0.9;// spire/m densita' di spire del solenoide
  double i=7;// A corrente nel solenoide
  double B=mu0*n*i;// T
  cout<<" computed magnetic field B = "<<B<<" T"<<endl;
  //calcolo di g
  double omega=fpg2->GetParameter("omega")*1e6;//Hz
  double eomega=fpg2->GetParError(fpg2->GetParNumber("omega"))*1e6;//Hz
  double mmuc2overe=105e6;// eV/e massa del muone
  double c=3e8;// m/s
  double g= omega*2*mmuc2overe/B/c/c;
  double eg= eomega*2*mmuc2overe/B/c/c;
  cout<<" g = "<<g<<" +/- "<<eg<<endl;
  double alpha=fpg2->GetParameter("alpha");
  double ealpha=fpg2->GetParError(fpg2->GetParNumber("alpha"));
  cout<<" alpha = "<<alpha*100<<" +/- "<<ealpha<<" %"<<endl;
  double *val=new double [6];
  val[0]=alpha;
  val[1]=ealpha;
  val[2]=g;
  val[3]=eg;
  val[4]=fpg2->GetParameter("phioverpi");;
  val[5]=fpg2->GetParError(fpg2->GetParNumber("phioverpi"));
  return val;
}


void scanlikelihood(TF1* fpg2){
  delete_canvas("canv");
  TCanvas *canv=new TCanvas("canv");
  canv->Divide(2,2);
  int i=0;
  string parlist[4];
  parlist[0]="alpha";parlist[1]="omega";parlist[2]="phioverpi";parlist[3]="NB";
  for (int i=0;i<4;i++){
    canv->cd(i+1);
    const char* parname=parlist[i].c_str();
    int parnum = fpg2->GetParNumber(parname)+1;
    double limit1, limit2;
    fpg2->GetParLimits(fpg2->GetParNumber(parname),limit1,limit2);
    if ((limit2-limit1)==0) continue;
    double parval = fpg2->GetParameter(parname);
    double parerr = fpg2->GetParError(fpg2->GetParNumber(parname));
    double plim_min, plim_max;
    fpg2->GetParLimits(fpg2->GetParNumber(parname),plim_min,plim_max);
    double pmin=TMath::Max(parval-2*parerr,plim_min);
    double pmax=TMath::Min(parval+2*parerr,plim_max);
    int npts=100;
    char scanstr[256];
    sprintf(scanstr," SCAn %i %i %f %f",parnum,npts,pmin,pmax);
    //    gMinuit->mnhelp("SCAn");
    gMinuit->Command(scanstr);
    TGraph *gr = (TGraph*)gMinuit->GetPlot();
    if (gr!=0){
      gr->SetMarkerStyle(21);      
      gr->SetTitle(TString("Likelihood scan of parameter ")+parname); 
      gr->Draw("alp");
    }
  }
}

void delete_canvas(const char* name){
  TCanvas *old = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(name);
   if (old && old->IsOnHeap()) {
    delete old;
  }
}


void printformula(TF1* fpg2, double x, double y){
  TString tformula=fpg2->GetExpFormula();
  tformula.ReplaceAll("[0]","NS");
  tformula.ReplaceAll("[1]","\\tau");
  tformula.ReplaceAll("[2]","\\alpha");
  tformula.ReplaceAll("[3]","\\omega");
  tformula.ReplaceAll("[4]*3.141592","\\phi");
  tformula.ReplaceAll("[5]","NB");
  tformula.ReplaceAll("[6]","bw");

  TLatex *formula=new TLatex(x,y,tformula);
  formula->Draw();
}

TGraphErrors* graph_resuduals( TH1F* h, TF1* f, double fitmin, double fitmax){
  int const Nbins(h->GetNbinsX());
  double x[Nbins],y[Nbins];
  double ex[Nbins],ey[Nbins];
  int Npoints=0, jfirst=0;
  double chi2=0;
  for (int i =1;i<=Nbins;i++) { 
    double position = h->GetBinCenter(i);
    if (fitmin<position && position<fitmax){
	jfirst=i-1;
	double residual = h->GetBinContent(i) - f->Eval(position); 
	if (h->GetBinContent(i) !=0)
	  chi2+=residual*residual/h->GetBinContent(i);
	double rel_residual = residual/f->Eval(position);
	x[Npoints]= position;
	y[Npoints]= rel_residual;
	//y[Npoints]= residual;
	ex[Npoints]= = 0 ;
	ey[Npoints] = 1./sqrt(h->GetBinContent(i));
	//ey[Npoints] = sqrt(h->GetBinContent(i));
	Npoints++;
      }
  }
  TGraphErrors* gr=0;
  if (Npoints!=0){
    gr= new TGraphErrors(Npoints,x,y,ex,ey);
    gr->SetMarkerStyle(21);
    gr->SetMinimum(-0.25);
    gr->SetMaximum(0.25);
    int ndf=Npoints - f->GetNumberFreeParameters();
    cout<<f->GetTitle()<<" chi2/NDoF = "<<chi2/ndf<<endl;
  } 
  return gr;
}

double* simplefit(bool fitprecession=false, bool binned=true, int istart=0, int nfiles=-1){
  /* 
     Read the data into a TTree 
     and convert clock periods into microseconds
  */

  TTree t( "t","t");
  // readfiles(t);  // 2022/2023 with B-field 
  // float fitmin=1; //minium time used in the fit
  // float fitmax=7; //max time used in the fit
  //readfiles23(t); // 2023/2024 no B-field
  readfiles_natale23(t,istart,nfiles); // 2023/2024 with B-field 
  float fitmin=0.5; //minium time used in the fit
  float fitmax=10; //max time used in the fit
  t.SetAlias("t","dt*15.5/1e3");

  /* 
     Configure the fit parameters
  */

  /* B=55e-4; mmuc2overe=105e6; c=3e8; omega=B*c*c/mmuc2overe; pi=3.141592; tauosc=2*pi/omega */
  // float tauosc=1.33;
  TString fitopt="EMSl";//E=better error estimation, M=improve: after min, serch for new one, l=likelihood
  double bw=0.25; //bin width in microseconds
  double histmin=0,histmax=10; //histogram limits in microseconds
  int nbins=(histmax-histmin)/bw; //number of bins of the time histogram
  
  /* 
     Fill the histogram of the experimental time distribution
     excluding low dt background coincidences if needed
  */

  TH1F *ht;
  ht = ((TH1F*)gROOT->FindObject("ht")); if (ht!=0) delete ht;  
  ht = new TH1F("ht","decay time distribution", nbins, histmin, histmax );
  //t.Project("ht","t","t>0.25");
  t.Project("ht","t");


  /* 
     Fit the time distribution with a simple exponential
     used for comparisons
  */

  delete_canvas("c1");
  TCanvas *c1=new TCanvas("c1");
  TF1 *fp;
  fp = ((TF1*)gROOT->FindObject("myexp")); if (fp!=0) delete fp;  
  fp = new TF1("myexp","[0]*exp(-x/[1])+[2]",0,100);
  fp->SetTitle(" Simple Exponential ");
  fp->SetParameters(100,2,0);
  fp->SetParNames("A","tau","B");
  //fp->FixParameter(fp->GetParNumber("B"),0);
  TFitResultPtr fitresult;
  fitresult=ht->Fit(fp,fitopt,"",fitmin,fitmax);
  ht->Draw("e");
  gStyle->SetOptFit(111111);
  gPad->SetLogy();
  double *gval = new double [6];
  /*
    If requested fit the time distribution with exponential
    convoluted with a sinusoidal representing the efffect of the precession
   */
  if (fitprecession){
    // Fit the data with an exponential(x)precession
    TF1* fpg2;
    if (binned){
      fpg2 = configfit(ht,fitmin,fitmax);
      fpg2->SetLineColor(kBlue);
      fitresult=ht->Fit(fpg2,fitopt,"",fitmin,fitmax);
    } else {
      // Try an unbinned max likelihood fit.
      fpg2 = configunbinnedfit(ht,fitmin,fitmax);
      char sele[256]; sprintf(sele,"%f<t && t<%f",fitmin,fitmax);
      cout<<" sele = "<<sele<<endl;
      fitresult=t.UnbinnedFit("myexpcos","t",sele);
      int N=t.GetEntries(sele);
      double bw=ht->GetBinWidth(0);
      fpg2->SetParameter(fpg2->GetParNumber("Norm"),N*bw);
    }
    fpg2->SetLineColor(kBlue);
    ht->Draw("e");
    if (!binned) fpg2->Draw("same");
    if (binned) printformula(fpg2,0.1,5);
    
    /* 
       Display the results
    */
    
    // Compute lande factor g
    gval=computeg2(fp, fpg2);
    
    //Plot the residuals
    if (fp!=0){
      delete_canvas("residuals");
      TCanvas *rcanv = new TCanvas("residuals");
      TH1F* frame = rcanv->DrawFrame(fitmin,-0.25,fitmax,0.25);
      frame->SetTitle("relative residuals");
      TGraphErrors *g_res=graph_resuduals(ht,fp,fitmin,fitmax);
      g_res->SetTitle("relative residuals without precession");
      g_res->Draw("P same");
    
      TGraphErrors *g_res2=graph_resuduals(ht,fpg2,fitmin,fitmax);
      g_res2->SetTitle("relative residuals with precession");
      g_res2->SetMarkerColor(kBlue);
      g_res2->Draw("P same");
    
      TF1 *f0=new TF1("f0","0",0,100);
      f0->Draw("same");
    
      rcanv->BuildLegend();
      rcanv->SetTitle("Residuals");
      rcanv->Modified();
      rcanv->Update();
    }
    //Scan the likelihood for the relevant parameters
    if (binned) scanlikelihood(fpg2);
    //scanlikelihood(fpg2);
  }
  return gval;
}

void scanalpha(){
  const int nfiles=5;
  //  const int n=int(72./nfiles-1);
  const int ngroups=14;
  double x[ngroups],alpha[ngroups],ealpha[ngroups],g[ngroups],eg[ngroups],phi[ngroups],ephi[ngroups];
  for (int i=0;i<ngroups;i++){
    int start=i*nfiles;
    double *gval=simplefit(true,true,start,nfiles);
    alpha[i]=gval[0];
    ealpha[i]=gval[1];
    g[i]=gval[2];
    eg[i]=gval[3];
    phi[i]=gval[4];
    ephi[i]=gval[5];
    x[i]=i+1;
  }
  TGraphErrors *gr_alpha=new TGraphErrors(ngroups,x,alpha,0,ealpha);
  gr_alpha->SetName("alpha");
  gr_alpha->SetTitle("alpha");

  TGraphErrors *gr_g=new TGraphErrors(ngroups,x,g,0,eg);
  gr_g->SetName("g");
  gr_g->SetTitle("g");

  TGraphErrors *gr_phi=new TGraphErrors(ngroups,x,phi,0,ephi);
  gr_phi->SetName("phi");
  gr_phi->SetTitle("phi");

  TCanvas *canv=new TCanvas("canv_scan","canvas scan params",600,900);
  canv->Divide(1,3);
  canv->cd(1);
  gr_alpha->Draw("APL");
  canv->cd(2);
  gr_g->Draw("APL");
  canv->cd(3);
  gr_phi->Draw("APL");
}
