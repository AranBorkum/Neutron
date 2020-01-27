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

#include "DataAnaInputManager.hh"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"    , "produce help messages")
    ("input,i"   , po::value<std::string>(), "Input ROOT file with raw::RawDigit's")
    ("output,o"  , po::value<std::string>(), "Output file name"                    )
    ("nevent,n"  , po::value<int        >()->default_value(1)    , "Number of events to plot")
    ("tag,g"     , po::value<std::string>()->default_value("daq"), "raw::RawDigit label"     )
    ("threshold" , po::value<int        >()->default_value(25)   , "Threshold for hit finder")
    ("induction" , "Plot for an induction wire")
    ("collection", "Plot for a collection wire");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help") || vm.empty()) { std::cout << desc << "\n"; return 1; }

  art::InputTag daq_tag{ vm["tag"].as<std::string>() };
  std::vector<std::string> filenames(1, vm["input"].as<std::string>());

  bool hitDumped = false;
  for (gallery::Event e(filenames); !e.atEnd(); e.next()) {
    auto& digits = *e.getValidHandle<std::vector<raw::RawDigit>>(daq_tag);
    if (hitDumped) continue;
    for (auto const& digit: digits) {
      if (hitDumped) continue;
      //---------------------------------------------
      // Pedestal subtraction
      //---------------------------------------------
      int pedestal = 0;
      for (auto const& adc: digit.ADCs()) 
	pedestal += adc;
      pedestal /= digit.ADCs().size();

      //---------------------------------------------
      // Collection Wires
      //---------------------------------------------      
      if (vm.count("collection")) {

	bool has_pos = false;
	bool has_neg = false;
	for (auto const& adc: digit.ADCs()) {
	  if ((adc - pedestal) >      vm["threshold"].as<int>()) has_pos = true;
	  if ((adc - pedestal) < -0.5*vm["threshold"].as<int>()) has_neg = true;
	}

	int times[digit.ADCs().size()];
	int adcs [digit.ADCs().size()];
	if (has_pos && !has_neg) {
	  for (size_t i=0; i<digit.ADCs().size(); ++i) {
	    times[i] = i;
	    adcs [i] = digit.ADCs()[i];
	  }

	  TCanvas *c = new TCanvas("c", "c");
	  c->Print((vm["output"].as<std::string>()+".pdf[").c_str());
	  TGraph *g = new TGraph(digit.ADCs().size(), times, adcs);
	  g->Draw("AL");
	  c->Print((vm["output"].as<std::string>()+".pdf").c_str());
	  c->Print((vm["output"].as<std::string>()+".pdf]").c_str());
	  hitDumped = true;
	}
      }

      //---------------------------------------------
      // Induction Wires
      //---------------------------------------------      
      if (vm.count("induction")) {

	bool has_pos = false;
	bool has_neg = false;
	for (auto const& adc: digit.ADCs()) {
	  if ((adc - pedestal) >  vm["threshold"].as<int>()) has_pos = true;
	  if ((adc - pedestal) < -vm["threshold"].as<int>()) has_neg = true;
	}
	int times[digit.ADCs().size()];
	int adcs [digit.ADCs().size()];
	if (has_pos && has_neg) {
	  for (size_t i=0; i<digit.ADCs().size(); ++i) {
	    times[i] = i;
	    adcs [i] = digit.ADCs()[i];
	  }
	
	  TCanvas *c = new TCanvas("c", "c");
	  c->Print((vm["output"].as<std::string>()+".pdf[").c_str());
	  TGraph *g = new TGraph(digit.ADCs().size(), times, adcs);
	  g->Draw("AL");
	  c->Print((vm["output"].as<std::string>()+".pdf").c_str());
	  c->Print((vm["output"].as<std::string>()+".pdf]").c_str());
	  hitDumped = true;	  
	}
      }
    }
  }


}
