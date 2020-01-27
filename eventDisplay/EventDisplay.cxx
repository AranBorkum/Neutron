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

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"    , "produce help message")
    ("input,i"   , po::value<std::string>(), "Input ROOT file")
    ("output,o"  , po::value<std::string>(), "Output file name")
    ("nevent,n"  , po::value<int        >()->default_value(0), "Event number to process")
    ("tpc"       , po::value<int        >()->default_value(3), "TPC to make event display for")
    ("tag,g"     , po::value<std::string>()->default_value("daq"), "daq tag")
    ("compressed", "Using compressed ADC values");
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help") || vm.empty()) {
    std::cout << desc << "\n"; return 1;
  }

  // --- Read in the raw::RawDigit's from the .root file
  art::InputTag daq_tag{ vm["tag"].as<std::string>() };
  std::vector<std::string> filenames(1, vm["input"].as<std::string>());

  std::map<int, TH2D*> eventDisplay;

  int eNum = 0;
  for (gallery::Event e(filenames); !e.atEnd(); e.next()) {
    auto& digits = *e.getValidHandle<std::vector<raw::RawDigit>>(daq_tag);
    if (eNum != vm["nevent"].as<int>()) continue;

    int tpcNumber = 0;

    for (auto const digit: digits) {
      tpcNumber = int(digit.Channel() / 2560);
      int yLow  = tpcNumber*2560;
      int yHigh = (tpcNumber+1)*2560;
      eventDisplay[tpcNumber] = new TH2D(Form("CollectionPlaneEventDisplay_%i", tpcNumber),
					 ";time [tick];channel", 600, 0, 6000, 800, yLow, yHigh);
      for (int i=0; i<digit.ADCs().size(); ++i) {
	eventDisplay[tpcNumber]->Fill(i, digit.Channel(), digit.ADCs()[i]);
      }



    }










    eNum += 1;
  }

}
