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

  if(vm.count("help") || vm.empty()) {
    std::cout << desc << "\n"; return 1;
  }

  art::InputTag daq_tag{ vm["tag"].as<std::string>() };
  std::vector<std::string> filenames(1, vm["input"].as<std::string>());

  bool hitDumped = false;
  for (gallery::Event e(filenames); !e.atEnd(); e.next()) {
    auto& digits = *e.getValidHandle<std::vector<raw::RawDigit>>(daq_tag); 
    for (auto const& digit: digits) {
      raw::RawDigit::ADCvector_t ADCs(digit.Samples());
      raw::Uncompress(digit.ADCs(), ADCs, digit.Compression());
      for (auto const& adc: ADCs) {std::cout << adc << " "; }
      std::cout << std::endl;
    }

    // raw::RawDigit::ADCvector_t ADCs(digits[0].Samples());
    // std::cout << ADCs.size() << std::endl;
    // for(size_t i=0; i<digits.size(); ++i) {
    //   raw::Uncompress((digits)[i].ADCs(), ADCs,
    // 		      (digits)[i].Compression());
    //   // ADCs contains the uncompressed version now
    // }
    // std::cout << ADCs.size() << std::endl;
    // for (auto const& adc: ADCs)
    //   std::cout << adc << " ";
    // std::cout << std::endl;

  }





  //   auto& digits = *e.getValidHandle<std::vector<raw::RawDigit>>(daq_tag);
  //   for (auto const& digit: digits) {
  //     std::cout << digit.Channel() << " ";
  //     int counter=0;
  //     for (auto const& adc: digit.ADCs()) {
  // 	std::cout << adc << " ";
  //     }
  //     std::cout << std::endl;
  //   } continue;
  // } 
}
