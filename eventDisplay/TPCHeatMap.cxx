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
    ("help,h"   , "produce help message")
    ("input,i"  , po::value<std::string>(), "Input ROOT file")
    ("tag,g"    , po::value<std::string>()->default_value("daq"), "raw::RawDigit tag");


  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  art::InputTag daq_tag{ vm["tag"].as<std::string>() };
  std::vector<std::string> filenames(1, vm["input"].as<std::string>());

  TH2D *TPC = new TH2D("", "", 400, 0, 4492, 300, 0, 3000);

  for (gallery::Event e(filenames); !e.atEnd(); e.next()) {
    auto& digits = *e.getValidHandle<std::vector<raw::RawDigit>>(daq_tag); 
    for (auto const& digit: digits) {
      for (size_t i=0; i<digit.ADCs().size(); ++i) {
	TPC->Fill(i, digit.Channel(), digit.ADCs()[i]);
      }
    }
    continue;
  }
  
  TCanvas *c = new TCanvas("c", "c");
  c->Print("heatmap.png[");
  TPC->Draw("COLZ");
  c->Print("heatmap.png");
  c->Print("heatmap.png]");

  

}
