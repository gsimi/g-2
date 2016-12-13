#include "TH1F.h"
#include "TCanvas.h"
#include "TF1.h"
#include <fstream>
#include <sstream>

using namespace std;
void plot ( string fileName, bool clock_counts=false ){
  /* calibration */
  float time_calib_m=14.988e-3, time_calib_q=9.6e-3;// [microsec/clock count]
  /* configuration */
  float tmin=1, tmax=1e3*time_calib_m+time_calib_q; //[microseconds]
  int nbins=25;

  TH1F *dtraw, *dtmuplus, *dtmu;
    
    // open file want to read
    ifstream file;
    file.open(fileName.c_str());
    
    string input;
    //TVectorD xV;
    vector <double> xV,rawV;
    double x;
    
    // READ ALL THE FILE
    while ( getline(file, input) ){
        // check commented line with "#" or blank line
        if ( input.find("#") == string::npos && !input.empty() ){
            // transform string to stream
            stringstream stream( input );
            stream >> x;
            // memorize x data
            //cout << "x = " << x << "\t" << "y = " << y << endl;
	    rawV.push_back(x);
	    if (clock_counts) x=x*time_calib_m + time_calib_q; //[micro seconds]
            xV.push_back(x);
            }
    }
    
    dtraw = new TH1F("dtraw","Segnali mu+",1000,0,1000);
    for (unsigned int i=0; i<rawV.size(); i++) {
      dtraw->Fill(rawV[i]);
    }
    
    // FAI L'DTMUPLUS
    //    dtmuplus = new TH1F("dt","Segnali",nbins,tmin,tmax);
    dtmuplus = new TH1F("dtmup","Segnali mu+",nbins,tmin,tmax);
    for (unsigned int i=0; i<xV.size(); i++) {
      dtmuplus->Fill(xV[i]);
    }
    


    /*dtmuplus->SetLineColor(2);
    dtmuplus->SetLineWidth(4);
    dtmuplus->SetMarkerColor(40);
    dtmuplus->SetMarkerSize(10);
    dtmuplus->SetMarkerStyle(7);
    //dtmuplus->SetTitle("Option ACP example");*/

    
    dtmuplus->GetXaxis()->SetTitle("t (microsecondi)");
    dtmuplus->GetYaxis()->SetTitle("conteggi");
    dtmuplus->Draw("e");

    float bw=(tmax-tmin)/nbins;
    char sfitmuplus[128];
    sprintf(sfitmuplus,"[0]*%f/(%f-%f)+[1]*%f/[2]*exp(-x/[2])/(exp(-%f/[2])-exp(-%f/[2]))",bw,tmax,tmin,bw,tmin,tmax);
    TF1* ffitmuplus=new TF1("fdtmuplus",sfitmuplus,tmin,tmax);
    ffitmuplus->SetParNames("Nbkg","NSig","tau");
    ffitmuplus->SetParameters(10,100,2);
    dtmuplus->Fit(ffitmuplus,"l","",tmin,tmax);
    
    tmin=time_calib_q;
    nbins=1000;
    bw=(tmax-tmin)/nbins;

    dtmu = new TH1F("dtmupm","Segnali mu+ e mu-",nbins,tmin-bw/2,tmax-bw/2);	
    for (unsigned int i=0; i<xV.size(); i++) {
      dtmu->Fill(xV[i]);
    }
    dtmu->Draw("e");
    
    char sfitmu[512];
    sprintf(sfitmu,"[0]*%f/(%f-%f)+[1]*%f/[2]*exp(-x/[2])/(exp(-%f/[2])-exp(-%f/[2])) + [3]*%f/[4]*exp(-x/[4])/(exp(-%f/[4])-exp(-%f/[4]))",bw,tmax,tmin,bw,tmin,tmax,bw,tmin,tmax);
    TF1* ffitmu=new TF1("fdt",sfitmu,tmin,tmax);
    ffitmu->SetParNames("Nbkg","NSig_p","tau_p","NSig_m","tau_m");
    ffitmu->SetParameters(10,100,2,100,0.1);

    dtmu->GetXaxis()->SetTitle("t (microsecondi)");
    dtmu->GetYaxis()->SetTitle("conteggi");
    dtmu->Fit(ffitmu,"l","",tmin,tmax);

    dtmuplus->Draw("e");
    
    return;
   
}
