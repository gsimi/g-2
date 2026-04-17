/* read the arietta data into a ttree */
void readfiles(TTree *t){
  t->ReadFile("mulife26-2022.12.27.dat","dt");
  t->ReadFile("mulife26-2022.12.28.dat","dt");
  t->ReadFile("mulife26-2022.12.29.dat","dt");
  t->ReadFile("mulife26-2022.12.30.dat","dt");
  t->ReadFile("mulife26-2022.12.31.dat","dt");
  t->ReadFile("mulife26-2023.01.01.dat","dt");
  t->ReadFile("mulife26-2023.01.02.dat","dt");
  t->ReadFile("mulife26-2023.01.03.dat","dt");
  t->ReadFile("mulife26-2023.01.04.dat","dt");
  t->ReadFile("mulife26-2023.01.05.dat","dt");
  t->ReadFile("mulife26-2023.01.06.dat","dt");
  t->ReadFile("mulife26-2023.01.07.dat","dt");
  t->ReadFile("mulife26-2023.01.08.dat","dt");
  t->ReadFile("mulife26-2023.01.09.dat","dt");
  t->ReadFile("mulife26-2023.01.10.dat","dt");
  t->ReadFile("mulife26-2023.01.11.dat","dt");
}

/* plot the decay time distribution and fit it with expo+ const bkg*/
double* fitdt(double fitmin=0.4){
  TTree t( "t","t");
  readfiles(&t);
  t.SetAlias("t","dt*15.5/1e3");
  float fitmax=15;

  TH1F *ht = new TH1F("ht","decay time distribution", 100, 0, 15);
  t.Draw("t>>ht","t>0.2");

  TF1 *fp=new TF1("myexp","[0]*exp(-x/[1])+[2]",0,100);

  fp->SetParNames("A","tau","B");
  fp->SetParameters(100,2,1);
  fp->SetParLimits(fp->GetParNumber("A"),1,1e4);
  fp->SetParLimits(fp->GetParNumber("tau"),0.05,10);
  fp->SetParLimits(fp->GetParNumber("B"),0,1e3);

  ht->Fit(fp,"q","",fitmin,fitmax);
  ht->Draw("e");
  
  double tau=fp->GetParameter(fp->GetParNumber("tau"));
  double etau=fp->GetParError(fp->GetParNumber("tau"));
  double *tauetau = new double[2];
  tauetau[0]=tau; tauetau[1]=etau;
  cout<<"tau = "<<tauetau[0]<<" +/- "<<tauetau[1]<<endl;
  return tauetau;
}
/* gra systematic variation of lifetime with respect to cut on minimum decay time*/
TGraphErrors* tauvsfitmin(){
  const int n=10;
  float dfitmin=0.1;
  double x[n],y[n],ex[n],ey[n];
  for (int i=0;i<n;i++){
    float fitmin=i*dfitmin;
    double* tauetau=fitdt(fitmin); 
    x[i]=fitmin;
    ex[i]=0;
    y[i]=tauetau[0];
    ey[i]=tauetau[1];
    delete tauetau;
  }
  TGraphErrors* gr=new TGraphErrors(n,x,y,ex,ey);
  gr->Draw("APL");
  return gr;
}
