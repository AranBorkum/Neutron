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

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

#include "lardataobj/Simulation/SimChannel.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/RDTimeStamp.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"
#include "lardataobj/RawData/raw.h"

#include "DataAnaInputManager.hh"
#include "Helper.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {

  // --- Command Line Flags -----------------------
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"   , "produce help message")
    ("ideal,i"  , po::value<std::string>(), "ROOT file with no nosie, i.e. the ideal  file")  
    ("real,r"   , po::value<std::string>(), "ROOT file with noise, i.e. the realistic file")
    ("event,e"  , po::value<int        >(), "The event number you want to run for")
    ("tag,g"    , po::value<std::string>()->default_value("daq"), "raw::RawDigit tag");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help") || vm.empty()) {
    std::cout << desc << "\n"; return 1;
  }

  // --- Define the output file ------------------
  std::ofstream myfile;
  myfile.open("HitMatching.txt");

  // --- Read in the ideal file ------------------
  std::map<int, DataAnaInputManager*> iaim;
  iaim[10 ] = new DataAnaInputManager(); iaim[10 ]->SetInputTree("dataAnarunsum10/DataTree");

  // --- Read in the real file -------------------
  std::map<int, DataAnaInputManager*> raim;
  // raim[1] = new DataAnaInputManager(); raim[1]->SetInputTree("dataAnarunsum1/DataTree");
  // // raim[2] = new DataAnaInputManager(); raim[2]->SetInputTree("dataAnarunsum2/DataTree");
  // raim[3] = new DataAnaInputManager(); raim[3]->SetInputTree("dataAnarunsum3/DataTree");
  // raim[4] = new DataAnaInputManager(); raim[4]->SetInputTree("dataAnarunsum4/DataTree");
  // raim[5] = new DataAnaInputManager(); raim[5]->SetInputTree("dataAnarunsum5/DataTree");
  // raim[6] = new DataAnaInputManager(); raim[6]->SetInputTree("dataAnarunsum6/DataTree");
  // raim[7] = new DataAnaInputManager(); raim[7]->SetInputTree("dataAnarunsum7/DataTree");
  // raim[8] = new DataAnaInputManager(); raim[8]->SetInputTree("dataAnarunsum8/DataTree");
  // raim[10] = new DataAnaInputManager(); raim[10]->SetInputTree("dataAnarunsum10/DataTree");
  // raim[12] = new DataAnaInputManager(); raim[12]->SetInputTree("dataAnarunsum12/DataTree");
  // raim[16] = new DataAnaInputManager(); raim[16]->SetInputTree("dataAnarunsum16/DataTree");
  // raim[20] = new DataAnaInputManager(); raim[20]->SetInputTree("dataAnarunsum20/DataTree");

  // raim[10] = new DataAnaInputManager(); raim[10]->SetInputTree("dataAnarunsum10/DataTree");
  // raim[20] = new DataAnaInputManager(); raim[20]->SetInputTree("dataAnarunsum20/DataTree");
  raim[40] = new DataAnaInputManager(); raim[40]->SetInputTree("dataAnarunsum40/DataTree");
  raim[60] = new DataAnaInputManager(); raim[60]->SetInputTree("dataAnarunsum60/DataTree");
  raim[80] = new DataAnaInputManager(); raim[80]->SetInputTree("dataAnarunsum80/DataTree");
  raim[100] = new DataAnaInputManager(); raim[100]->SetInputTree("dataAnarunsum100/DataTree");
  raim[200] = new DataAnaInputManager(); raim[200]->SetInputTree("dataAnarunsum200/DataTree");




  // --- Set all the input -----------------------
  for (auto& it: raim) {
    it.second->SetInputFile(vm["real"].as<std::string>());
    it.second->LoadTree();
  }

  for (auto& it: iaim) {
    it.second->SetInputFile(vm["ideal"].as<std::string>());
    it.second->LoadTree();
  }
  

  std::set<int> TrueChannels;
  std::vector<int> inductionHitChan;
  std::vector<int> inductionHitTime;
  std::vector<int> collectionHitChan;
  std::vector<int> collectionHitTime;


  for (auto const& it: iaim) {
    DataAnaInputManager* im = it.second;
    int event = vm["event"].as<int>();

    for (int CurrentEvent=event; CurrentEvent<event+1; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);
      int CollectionHits = 0;
      int InductionHits  = 0;
      for (size_t i=0; i<im->Hit_View->size(); ++i) {
	if ((*im->Hit_View)[i] == 2) {
	  CollectionHits++;
	  collectionHitChan.push_back((*im->Hit_Chan)[i]);
	  collectionHitTime.push_back((*im->Hit_Time)[i]);
	}
	if ((*im->Hit_View)[i] != 2) {
	  InductionHits++;
	  inductionHitChan.push_back((*im->Hit_Chan)[i]);
	  inductionHitTime.push_back((*im->Hit_Time)[i]);
	}
	TrueChannels.insert((*im->Hit_Chan)[i]);
      }
      std::cout << "No noise \t" << "Collection Hits: " << CollectionHits << "\tInduction Hits: " << InductionHits << std::endl;
    }
  }

  for (auto const& it: raim) {
    DataAnaInputManager* im = it.second;
    int event = vm["event"].as<int>();

    for (int CurrentEvent=event; CurrentEvent<event+1; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);
      int CollectionHits = 0; int matchedCol = 0;
      int InductionHits  = 0; int matchedInd = 0;
      for (size_t i=0; i<im->Hit_View->size(); ++i) {
	if ((*im->Hit_View)[i] == 2) {
	  CollectionHits++;
	  for (auto const chan: TrueChannels) {
	    if (chan == (*im->Hit_Chan)[i])
	      matchedCol++;
	    continue;
	  }
	  
	  
	}
	if ((*im->Hit_View)[i] != 2) {
	  InductionHits++;
	  for (int ii=0; ii<inductionHitChan.size(); ++ii) {
	    if (inductionHitChan[ii] == (*im->Hit_Chan)[i])
	      if (inductionHitTime[ii] > (*im->Hit_Time)[i]-10 && inductionHitTime[ii] < (*im->Hit_Time)[i]+10)
		matchedInd++;
	  }
	}
      }
      std::cout << it.first << " sigma \t" 
		<< "Collection Hits: "  << CollectionHits
		<< "\tInduction Hits: " << InductionHits 
		<< "\tMatched Col: "    << matchedCol
		<< "\tMatched Ind: "    << matchedInd << std::endl;
      myfile    << it.first       << "\t"
		<< CollectionHits << "\t"
		<< InductionHits  << "\t"
		<< matchedCol     << "\t"
		<< matchedInd << std::endl;

    }
  }
}
