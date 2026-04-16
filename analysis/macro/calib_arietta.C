#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <TGraphErrors.h>
#include <TF1.h>
#include <TH1.h>

using namespace std;
int calib_arietta(const string& folder = "./")
{
	const string filename = "time_database";
	string path = folder + filename;
	ifstream f(path.c_str(), fstream::in);
	if (!f)
	{
		cerr << "failure opening database\n";
		return 1;
	}
	//map<string, pair<double, double>> files;
	//vector<data> files;
	
	string calibfilename;
	double t, et;
	
	vector<double> meanvec;
	vector<double> errvec;
	vector<double> timevec;
	vector<double> timeerrvec;
	
	
	const string prefix = "time_";
	const string postfix = "-2017.11.22.dat";
	
	while( f >> calibfilename >> t >> et)
	{
		if (f.eof())
			continue;
		string filename = prefix + calibfilename + postfix;
		
		ifstream datafile(filename.c_str(), fstream::in);
		int value;
		long sum = 0;
		long sumq = 0;
		int numvalues = 0;
		while(datafile >> value)
		{
			if (datafile.eof())
				continue;
			sum += value;
			sumq += value*value;
			numvalues ++;
		}
		double mean = sum *1./numvalues;
		double err = sqrt((1.*sumq/numvalues - mean*mean)/(numvalues - 1));
		
		meanvec.push_back(mean);
		errvec.push_back(err);
		timevec.push_back(t);
		timeerrvec.push_back(et);
	}
	
	clog << *meanvec.data() << endl;
	TGraphErrors * h = new TGraphErrors(meanvec.size(), meanvec.data(), timevec.data(), errvec.data(), timeerrvec.data());
	h->SetTitle("");
	h->GetHistogram()->SetYTitle("time [#mus]");
	h->GetHistogram()->SetXTitle("cycles");
	h->Draw("AP");
	TF1 * fit = new TF1("linear_fit", "pol1", 0, 1000);
	h->Fit(fit, "EMVR", "");
		
	return 0;	
}


