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

#include "/dune/app/users/aborkum/v08_33_00_prof_e17/srcs/dunetpc/dune/DAQSimAna/AlgParts.h"

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

#include "lardataobj/Simulation/SimChannel.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/RDTimeStamp.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"   , "produce help message")
    ("input,i"  , po::value<std::string>(), "Input ROOT file")
    ("tag,g"    , po::value<std::string>()->default_value("daq"), "raw::RawDigit tag")
    ("nevent,n" , po::value<int        >()->default_value(-1)   , "Event number to display");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  
  // Print help messages if -h is the only command line flag
  if (vm.count("help") || vm.empty()) { std::cout << desc << "\n" ; return 1; }

  // Reading in the _detsim.root file
  std::ofstream myfile;
  myfile.open("filtered.txt");
  art::InputTag daq_tag { vm["tag"].as<std::string>() };
  std::vector<std::string> filenames(1, vm["input"].as<std::string>());
  
  // Cycling through the Events
  for (gallery::Event e(filenames); !e.atEnd(); e.next()) {

    // Reading in the raw ADC values
    auto& digits = *e.getValidHandle<std::vector<raw::RawDigit>>(daq_tag);
    for (auto const& digit: digits) {

      //---------------------------------------------
      // Pedestal subtraction
      //---------------------------------------------
      const std::vector<short>& pedestal=true ?
	frugal_pedestal_sigkill(digit.ADCs(),
				5,
				15,
				1) :
	frugal_pedestal(digit.ADCs(), 10);
      std::vector<short> pedsub(digit.ADCs().size(), 0);
      for(size_t i=0; i<pedsub.size(); ++i)
	pedsub[i]=digit.ADCs()[i]-pedestal[i];

      //---------------------------------------------
      // Filtering
      //---------------------------------------------
      std::vector<short> TAPS = {2, 9, 23, 31, 23, 9, 2};
      const size_t ntaps = TAPS.size();
      const short*  taps = TAPS.data();
      std::vector<short> filtered(true ?
				  apply_fir_filter(pedsub, ntaps, taps) :
				  pedsub);

      for (auto const& i: filtered) 
	myfile << i << " ";
      myfile << std::endl;
    }
    myfile.close();
  }
}
