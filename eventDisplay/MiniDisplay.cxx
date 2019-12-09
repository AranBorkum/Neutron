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

#include "boost/program_options.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"   , "produce help message")
    ("input,i"  , po::value<std::string>(), "Input ROOT file")
    ("event,n"  , po::value<int        >(), "event number to process")
    ("output,o" , po::value<std::string>()->default_value("eventDisplay"), "output name for the figure")
    ("raw-input", po::value<std::string>(), "raw input file")
    ("run-input", po::value<std::string>(), "runsum input file")
    ("tpc"      , po::value<int        >()->default_value(1), "TPC number to show")
    ("plane"    , po::value<int        >()->default_value(1), "which plane to show")
    ("limit"    , po::value<std::string>()->default_value(""), "limits for the plots");


  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help") || vm.empty()) {
    std::cout << desc << "\n";
    return 1;
  }


  std::stringstream ss(vm["limit"].as<std::string>());
  std::vector<int> lims;

  if( vm.count("limit")) {
    for (int i; ss >> i;) {
      lims.push_back(i);    
      if (ss.peek() == ',')
	ss.ignore();
    }
  }

  DataAnaInputManager *im = new DataAnaInputManager();
  im->SetInputFile(vm["input"].as<std::string>());
  im->SetInputTree("dataAnarunsum3/DataTree");
  im->LoadTree();

  std::set<int> ChannelNumbers;

  int eNum  = vm["event"].as<int>();
  int event = 0;
  int tpc   = vm["tpc"]  .as<int>();
  int view  = vm["plane"].as<int>();

  for (int CurrentEvent=eNum; CurrentEvent<eNum+1; ++CurrentEvent) {
    im->GetEntry(CurrentEvent);

    event = im->Event;
    for (int it=0; it<im->Hit_View->size(); ++it) {
      if ((*im->Hit_View)[it] == view && (*im->Hit_TPC)[it] == tpc) {
	ChannelNumbers.insert((*im->Hit_Chan)[it]);
      }
    }
  }

  int minX = *std::min_element(ChannelNumbers.begin(), ChannelNumbers.end());
  int maxX = *std::max_element(ChannelNumbers.begin(), ChannelNumbers.end());
  int bins = (maxX - minX);

  TH2D *eventDisplayRaw = new TH2D();
  TH2D *eventDisplayRun = new TH2D();
  
  if (vm["limit"].as<std::string>() == "") {
    eventDisplayRaw = new TH2D("eventDisplayRaw", "", 6000, 0, 6000, bins, minX, maxX);
    eventDisplayRun = new TH2D("eventDisplayRun", "", 6000, 0, 6000, bins, minX, maxX);
  }
  else {
    eventDisplayRaw = new TH2D("eventDisplayRaw", "", 
			       lims[1]-lims[0], lims[0], lims[1], lims[3]-lims[2], lims[2], lims[3]);
    eventDisplayRun = new TH2D("eventDisplayRun", "", 
			       lims[1]-lims[0], lims[0], lims[1], lims[3]-lims[2], lims[2], lims[3]);
  }

  std::ifstream rawInfile(vm["raw-input"].as<std::string>());
  std::string   rawLine;

  while (std::getline(rawInfile, rawLine)) {
    std::istringstream iss(rawLine); int n; std::vector<int> v;
    while (iss >> n) v.push_back(n);
    for (auto const& chan: ChannelNumbers)
      if (v[1] == chan && v[0] == event)
	for (int i=2; i<v.size(); ++i)
	  eventDisplayRaw->Fill(i-2, v[1], v[i]);    
  }

  std::ifstream runInfile(vm["run-input"].as<std::string>());
  std::string   runLine;

  while (std::getline(runInfile, runLine)) {
    std::istringstream iss(runLine); int n; std::vector<int> v;
    while (iss >> n) v.push_back(n);
    for (auto const& chan: ChannelNumbers)
      if (v[1] == chan && v[0] == event)
	for (int i=2; i<v.size(); ++i)
	  eventDisplayRun->Fill(i-2, v[1], v[i]);    
  }


  TCanvas *c = new TCanvas("c", "c");
  c->Print((vm["output"].as<std::string>() + ".pdf[").c_str());
  eventDisplayRaw->Draw("COLZ");
  eventDisplayRaw->GetXaxis()->SetTitle("Time");
  eventDisplayRaw->GetYaxis()->SetTitle("Channel");
  eventDisplayRaw->SetStats(0);
  c->Print((vm["output"].as<std::string>() + ".pdf").c_str());

  eventDisplayRun->Draw("COLZ");
  eventDisplayRun->GetXaxis()->SetTitle("Time");
  eventDisplayRun->GetYaxis()->SetTitle("Channel");
  eventDisplayRun->SetStats(0);
  c->Print((vm["output"].as<std::string>() + ".pdf").c_str());
  c->Print((vm["output"].as<std::string>() + ".pdf]").c_str());

}
