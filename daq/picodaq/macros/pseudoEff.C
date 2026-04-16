#include <string>
#include <fstream>
#include <TGraph.h>
#include <vector>
#include <TAxis.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TStyle.h>

using namespace std;

void pseudoEff(string filename) 
{
	ifstream fin(filename.c_str());
	double xTemp,yTemp;
	vector<double> x,y;
	while(fin >> xTemp >> yTemp)
	{
		if(!fin.eof())
		{
			x.push_back(10.1 + 4.56*xTemp);
			y.push_back(1000*yTemp);
		}
	}
	
	TGraph* g = new TGraph(x.size(), &x[0], &y[0]);
	g->GetXaxis()->SetTitle("Time width [ns]");
	g->GetYaxis()->SetTitle("Freq [Hz]");
	g->SetTitle("");
	g->SetMarkerStyle(2);
	
	TF1* f = new TF1("fit", "pol1");
	g->Fit(f, "EMQ");
	
	TCanvas* c = new TCanvas();
	gStyle->SetOptFit(true);
	g->Draw("ap");
	string imgname = "graphs/" + filename + ".png";
	c->SaveAs(imgname.c_str());
	imgname = "graphs/" + filename + ".tex";
	c->SaveAs(imgname.c_str());
	imgname = "graphs/" + filename + ".C";
	c->SaveAs(imgname.c_str());
}
			
	
