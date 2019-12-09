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

void PlotTH2D(TH2D* h, std::string title="") {

    Double_t max = h->GetMaximum();
    Double_t min = h->GetMinimum();
    Double_t vw  = 20.;
    Double_t pw  = (vw-min)/(max-min);
    Double_t e   = 0.01;
    Double_t l   = 0.9;

    const Int_t Number = 6;
    Double_t Red[Number]   = { 0.0, l  , 1., 1., 1.0, 1.0};
    Double_t Green[Number] = { 0.0, l  , 1., 1., l  , 0.0};
    Double_t Blue[Number]  = { 1.0, 1.0, 1., 1., l  , 0.0};
    Double_t Stops[Number] = { 0., pw-e, pw-e, pw+e, pw+e, 1. };

    Int_t nb= 256;
    h->SetContour(nb);
    h->SetStats(0);
    TColor::CreateGradientColorTable(Number,Stops,Red,Green,Blue,nb);
    h->SetTitle("");
    h->GetXaxis()->SetTitle("Time [ticks]");
    h->GetYaxis()->SetTitle("Channel");
    h->Draw("COLZ");
}

int main(int argc, char** argv) {

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"   , "produce help message")
    ("input,i"  , po::value<std::string>(), "Input ROOT file")
    ("output,o" , po::value<std::string>(), "Output file name")
    ("text,t"   , po::value<std::string>(), "Input text file")
    ("nevent,n" , po::value<int        >()->default_value(1), "Event number to process")
    ("nskip"    , po::value<int        >()->default_value(0), "Number of events to skip")
    ("tpc"      , po::value<int        >()->default_value(3), "TPC to make event display for")
    ("threshold", po::value<std::string>(), "The ADC threshold for the hit finder")
    ("plot-hits", "Plot event displays with hits")
    ("only-hits", "Plot only the hits");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  DataAnaInputManager *im = new DataAnaInputManager();
  im->SetInputFile(vm["input"].as<std::string>());
  im->SetInputTree(("dataAnarunsum"+vm["threshold"].as<std::string>()+"/DataTree").c_str());
  im->LoadTree();

  std::cout << "INPUT CONFIGURATION"                        << std::endl;
  std::cout << "---------------------------------------"    << std::endl;
  std::cout << "Reading from input tree:   dataAnarunsum"   << vm["threshold"].as<std::string>() 
	    << "/DataTree"                                  << std::endl;

  int startNumber = vm["nskip" ].as<int>();
  int endNumber   = vm["nevent"].as<int>() + startNumber;

  int event   = 0;
  int indHits = 0;
  int colHits = 0;

  int TimeU_max = 0; int ChannelU_max = 0;
  int TimeV_max = 0; int ChannelV_max = 0;
  int TimeZ_max = 0; int ChannelZ_max = 0;

  std::vector<int> Times_U    = {};  std::vector<int> Times_V    = {};  std::vector<int> Times_Z    = {};
  std::vector<int> Channels_U = {};  std::vector<int> Channels_V = {};  std::vector<int> Channels_Z = {};

  for (int CurrentEvent=startNumber; CurrentEvent<endNumber; ++CurrentEvent) {
    im->GetEntry(CurrentEvent);

    event = im->Event;
    for (int it=0; it<im->Hit_View->size(); ++it) {
      if ((*im->Hit_View)[it] == 0 && (*im->Hit_TPC)[it] == vm["tpc"].as<int>()) {
	indHits++;
	Times_U   .push_back((*im->Hit_Time)[it]); if ((*im->Hit_Time)[it] > TimeU_max   ) TimeU_max    = (*im->Hit_Time)[it];
	Channels_U.push_back((*im->Hit_Chan)[it]); if ((*im->Hit_Chan)[it] > ChannelU_max) ChannelU_max = (*im->Hit_Chan)[it];
      }
      if ((*im->Hit_View)[it] == 1 && (*im->Hit_TPC)[it] == vm["tpc"].as<int>()) {
	indHits++;
	Times_V   .push_back((*im->Hit_Time)[it]); if ((*im->Hit_Time)[it] > TimeV_max   ) TimeV_max    = (*im->Hit_Time)[it];
	Channels_V.push_back((*im->Hit_Chan)[it]); if ((*im->Hit_Chan)[it] > ChannelV_max) ChannelV_max = (*im->Hit_Chan)[it];
      }
      if ((*im->Hit_View)[it] == 2 && (*im->Hit_TPC)[it] == vm["tpc"].as<int>()) {
	colHits++;
	Times_Z   .push_back((*im->Hit_Time)[it]); if ((*im->Hit_Time)[it] > TimeZ_max   ) TimeZ_max    = (*im->Hit_Time)[it];
	Channels_Z.push_back((*im->Hit_Chan)[it]); if ((*im->Hit_Chan)[it] > ChannelZ_max) ChannelZ_max = (*im->Hit_Chan)[it];
      }
    }
  }

  std::cout << "Number of collection hits: " << colHits << std::endl;
  std::cout << "Number of induction hits:  " << indHits << std::endl;
  std::cout << std::endl;

  int TimeU_min = *std::min_element(Times_U.begin(), Times_U.end()); 
  int TimeV_min = *std::min_element(Times_V.begin(), Times_V.end()); 
  int TimeZ_min = *std::min_element(Times_Z.begin(), Times_Z.end()); 
  int ChannelU_min = *std::min_element(Channels_U.begin(), Channels_U.end());
  int ChannelV_min = *std::min_element(Channels_V.begin(), Channels_V.end());
  int ChannelZ_min = *std::min_element(Channels_Z.begin(), Channels_Z.end());

  TH2D *eventDisplay_U = new TH2D("eventDisplay_U", "eventDisplay_U", 600, TimeU_min, TimeU_max, 250, ChannelU_min, ChannelU_max);
  TH2D *eventDisplay_V = new TH2D("eventDisplay_V", "eventDisplay_V", 600, TimeV_min, TimeV_max, 250, ChannelV_min, ChannelV_max);
  TH2D *eventDisplay_Z = new TH2D("eventDisplay_Z", "eventDisplay_Z", 600, TimeZ_min, TimeZ_max, 250, ChannelZ_min, ChannelZ_max);

  std::ifstream infile(vm["text"].as<std::string>());
  std::string line;

  while (std::getline(infile, line)) {
    std::istringstream iss(line); int n; std::vector<int> v;
    while (iss >> n) v.push_back(n);

    int ChannelNumber = v[1];
    bool inU = false; bool inV = false; bool inZ = false;
    for (auto const& it: Channels_U)
      if (it == ChannelNumber) inU = true;
    for (auto const& it: Channels_V)
      if (it == ChannelNumber) inV = true;
    for (auto const& it: Channels_Z)
      if (it == ChannelNumber) inZ = true;

    if (inU) {
      for (int i=3; i<v.size(); ++i) {
	if (v[0] == event)
	  eventDisplay_U->Fill(i, ChannelNumber, v[i]);
      }}
    if (inV) {
      for (int i=3; i<v.size(); ++i) {
	if (v[0] == event)
	  eventDisplay_V->Fill(i, ChannelNumber, v[i]);
      }}
      if (inZ) {
	for (int i=3; i<v.size(); ++i) {
	  if (v[0] == event)
	    eventDisplay_Z->Fill(i, ChannelNumber, v[i]);
      }}
  }

  TCanvas *c = new TCanvas("c", "c", 1600, 900);
  c->Print((vm["output"].as<std::string>() + ".pdf[").c_str());
  int n_u = Times_U.size(); int n_v = Times_V.size(); int n_z = Times_Z.size();
  int t_u [Times_U.size()]; int t_v [Times_V.size()]; int t_z [Times_Z.size()];
  int c_u [Times_U.size()]; int c_v [Times_V.size()]; int c_z [Times_Z.size()];
  for (int i=0; i<n_u; ++i) { t_u[i] = Times_U[i]; c_u[i] = Channels_U[i]; }
  for (int i=0; i<n_v; ++i) { t_v[i] = Times_V[i]; c_v[i] = Channels_V[i]; }
  for (int i=0; i<n_z; ++i) { t_z[i] = Times_Z[i]; c_z[i] = Channels_Z[i]; }

  TGraph *gU = new TGraph(n_u, t_u, c_u);
  TGraph *gV = new TGraph(n_v, t_v, c_v);
  TGraph *gZ = new TGraph(n_z, t_z, c_z);
  
  if (vm.count("only-hits")) {
    gU->Draw("AP");
    gU->SetTitle("U-Plane");
    c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
    gV->Draw("AP");
    gV->SetTitle("V-Plane");
    c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
    gZ->Draw("AP");
    gZ->SetTitle("Z-Plane");
    c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
  }
  if (vm.count("plot-hits")) {
    PlotTH2D(eventDisplay_U);
    gU->Draw("P");
    eventDisplay_U->SetTitle("U-Plane");
    c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

    PlotTH2D(eventDisplay_V);
    gV->Draw("P");
    eventDisplay_V->SetTitle("V-Plane");
    c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

    PlotTH2D(eventDisplay_Z);
    gZ->Draw("P");
    eventDisplay_Z->SetTitle("Z-Plane");
    c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
  } 
  if (!vm.count("plot-hits") && !vm.count("only-hits")) {
    PlotTH2D(eventDisplay_U);
    eventDisplay_U->SetTitle("U-Plane");
    c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

    PlotTH2D(eventDisplay_V);
    eventDisplay_V->SetTitle("V-Plane");
    c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

    PlotTH2D(eventDisplay_Z);
    eventDisplay_Z->SetTitle("Z-Plane");
    c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
  }
  c->Print((vm["output"].as<std::string>() + ".pdf]").c_str());

}
