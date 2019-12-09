#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TColor.h"
#include "TStyle.h"
#include "TPad.h"

#include "DataAnaInputManager.hh"

#include <unistd.h>

int main(int argc, char** argv) {

  int opt;
  extern char *optarg;
  extern int   optopt;

  int         nEvent      = -1;
  int         nSkip       =  0;
  std::string InFileName  = "";
  std::string OutFileName = "";

  while ( (opt = getopt(argc, argv, "i:o:n:k:")) != -1 ) {
    switch (opt) {
    case 'i':
      InFileName = optarg;
      break;
    case 'o':
      OutFileName = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case 'k':
      nSkip = atoi(optarg);
      break;
    case '?':
      std::cerr << "Unknown option '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  std::ofstream outputFile("HitInformation.txt");

  std::map<int, DataAnaInputManager*> aaim;
  aaim[1]  = new DataAnaInputManager(); aaim[1] ->SetInputTree( "dataAnarunsum1/DataTree");
  aaim[2]  = new DataAnaInputManager(); aaim[2] ->SetInputTree( "dataAnarunsum2/DataTree");
  aaim[3]  = new DataAnaInputManager(); aaim[3] ->SetInputTree( "dataAnarunsum3/DataTree");
  aaim[4]  = new DataAnaInputManager(); aaim[4] ->SetInputTree( "dataAnarunsum4/DataTree");
  aaim[5]  = new DataAnaInputManager(); aaim[5] ->SetInputTree( "dataAnarunsum5/DataTree");
  aaim[6]  = new DataAnaInputManager(); aaim[6] ->SetInputTree( "dataAnarunsum6/DataTree");
  aaim[7]  = new DataAnaInputManager(); aaim[7] ->SetInputTree( "dataAnarunsum7/DataTree");
  aaim[8]  = new DataAnaInputManager(); aaim[8] ->SetInputTree( "dataAnarunsum8/DataTree");
  aaim[9]  = new DataAnaInputManager(); aaim[9] ->SetInputTree( "dataAnarunsum9/DataTree");
  aaim[10] = new DataAnaInputManager(); aaim[10]->SetInputTree("dataAnarunsum10/DataTree");

  for (auto&& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  int Threshold[10];
  int nTotalHits[10];
  int nSignalHits[10];
  int nBackgroundHits[10];

  for (auto const& it: aaim) {
    int fNEvent             = nEvent;
    int sigma_threshold     = it.first;
    DataAnaInputManager* im = it.second;

    if (fNEvent != -1) fNEvent = std::min(fNEvent, (int)im->GetEntries());
    else               fNEvent = im->GetEntries();

    outputFile << sigma_threshold << "\t";
    for (int CurrentEvent=nSkip; CurrentEvent<fNEvent+nSkip; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);

      int NIndHit        = 0;
      int NSignalHit     = 0;
      int NBackgroundHit = 0;

      for (size_t it=0; it<im->Hit_View->size(); ++it) {
	int RMS = (*im->Hit_RMS)[it];
	NIndHit++;
	if (RMS > 2) NSignalHit++;
	else         NBackgroundHit++;
      }

      outputFile << NIndHit << "\t" << NSignalHit <<  "\t" << NBackgroundHit << std::endl;
      Threshold      [sigma_threshold] = sigma_threshold;
      nTotalHits     [sigma_threshold] = NIndHit;
      nSignalHits    [sigma_threshold] = NSignalHit;
      nBackgroundHits[sigma_threshold] = NBackgroundHit;

    } // --- End of current event loop
    
  } // --- End of aaim loop

  TGraph *indHits = new TGraph(10, Threshold, nTotalHits     );
  TGraph *sigHits = new TGraph(10, Threshold, nSignalHits    );
  TGraph *bacHits = new TGraph(10, Threshold, nBackgroundHits);


  TCanvas *c = new TCanvas();
  c->Print((OutFileName+".pdf[").c_str());
  c->SetLogy(1);
  indHits->Draw("AP*"); indHits->SetMarkerColor(kBlack); indHits->GetXaxis()->SetRangeUser(0, 11);
  sigHits->Draw("P*");  sigHits->SetMarkerColor(kRed  ); sigHits->GetXaxis()->SetRangeUser(0, 11);
  bacHits->Draw("P*");  bacHits->SetMarkerColor(kBlue ); bacHits->GetXaxis()->SetRangeUser(0, 11);
  c->Print((OutFileName+".pdf").c_str());
  c->Print((OutFileName+".pdf]").c_str());

}
