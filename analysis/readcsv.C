/* forward declarations */
#include <iostream>
#include <string>
using namespace std;

/*
  Class to read a csv file and import it in a histogram or a tree
  Usage:

    .L readcsv.C
    csvreader csv("ph-sipm-xxx-32.3.csv")
    TH1F* h=csv.getHistogram()
    hph->Draw()
    searchpeaks(hph)
    TTree* t= csv.getTree()
    t->Draw("dndV:V")
  
 */

class csvReader {
public:
  csvReader(const char* filename, float xscale=1.);
  ~csvReader(){delete hph; delete t;};
  void scanHeader(std::ifstream* input, float xscale = 1.);
  void readHistogram(std::ifstream* input, float xscale = 1.);
  float getNBins(){return npoints;};
  float getXStep(){return fabs(xstep);};
  float getXmin(){return xmin-xstep*0.5;};
  float getXmax(){return xmin+xstep*(npoints+0.5);};
  char* getUnit(){return unit;};  TH1F* getHistogram(){return hph;};
private:
  const char* fName;
  int npoints;
  float xmin;
  float xstep;
  char* unit;
  TH1F * hph;
  TTree* t;
};

/* assign defaule values to data members, 
   scan header, 
   initializes histogram and 
   read histogram data */
csvReader::csvReader(const char* filename, float xscale){

  //set default values
  fName=filename;
  npoints=100;
  xmin=0.5;
  xstep=1;
  unit="";
  t=0;
  
  //open stream
  std::ifstream input(filename);

  //scan header fields
  scanHeader(&input, xscale);


  //read histogram data
  hph = new TH1F("hph","pulse height distribution",
		 npoints,xmin,getXmax());
  readHistogram(&input, xscale);

  //close file
  input.close();

}

  //read header section of csv, until Data keyword is found
  //Saves histogram pararmeters
void
csvReader::scanHeader(std::ifstream* inputptr, float xscale){
  std::string line;
  std::string key,val;
  while( std::getline( *inputptr, key ,':') && key.compare("Data")!=0) {
    std::getline(*inputptr,val,'\n');
    //    printf("header %s    %s\n",key.c_str(),val.c_str());
      if (key.compare("Points")==0)
      npoints=atoi(val.c_str());
    
    if (key.compare("XInc")==0)
      xstep=atof(val.c_str())*fabs(xscale);

    if (key.compare("XOrg")==0){
      float xorg=atof(val.c_str());
      xmin = xscale>0 ? xorg : xorg*xscale - (npoints-1)*xstep ;
    }
    
    if (key.compare("XUnits")==0)
      unit=val.c_str();
  }
  if (inputptr->eof()) std::cout<<"no data "<<endl;

}


/* Read the csv and fills the histogram */
void csvReader::readHistogram(std::ifstream *inputptr, float xscale){

  float x,y;
  int bin;

  std::string line;
  while( std::getline( *inputptr, line ) ) {
    sscanf(line.c_str(),"%f,%f",&x,&y);
    //    printf("%f    %f\n",x,y);
    bin = hph->FindBin(x*xscale);
    hph->SetBinContent(bin,y);
  }
  //  fclose(pFile);    
  if (!inputptr->eof()) std::cout<<"Error reading "<<filename<<endl;
}

/*
TTree* csvReader::getTree(float xscale){
  //open stream
  std::ifstream input(fName);

  //scan header fields
  scanHeader(&input, xscale);

  //read data in TTree, determine min and max
  t=new TTree;
  t->ReadStream(input,"V:dndV",',');
  input.close();
  return t;
}
*/


//helper function
void searchpeaks(TH1F* hph){
  hph->Smooth();
  TSpectrum s(10,1e-3);
  s.Search(hph,2);
  Float_t* xpeaks=s.GetPositionX();
  for (int i=0;i<s.GetNPeaks()-1;i++){
    printf("Guadagno %dphe-%dphe = %.3e\n",i+1,i,xpeaks[i]-xpeaks[i+1]);
  }
}
