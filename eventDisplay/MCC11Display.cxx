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
    ("help,h"   , "Produce help messages")
    ("input,i"  , po::value<std::string>(), "Input MCC11 ROOT file"          )
    ("output,o" , po::value<std::string>(), "Output file name"               )
    ("text,t"   , po::value<std::string>(), "Input text file"                )
    ("threshold", po::value<std::string>()->default_value("5"), "Threshold used by the hitfinder")
    ("nevent,n" , po::value<int        >()->default_value(1)  , "Starting event number"    )
    ("nskip,k"  , po::value<int        >()->default_value(0)  , "Number of events to skip" )
    ("tpc"      , po::value<int        >()->default_value(0)  , "TPC number to show events");

  po::variables_map vm; 
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  // Display of help messages
  if (vm.count("help") || vm.empty()) { std::cout << desc << "\n"; return 1; }

  DataAnaInputManager *im = new DataAnaInputManager();
  im->SetInputFile(vm["input"].as<std::string>());
  im->SetInputTree(("dataAnarunsum"+vm["threshold"].as<std::string>()+"/DataTree").c_str());
  im->LoadTree();

  std::cout << "INPUT CONFIGURATION"                        << std::endl;
  std::cout << "---------------------------------------"    << std::endl;
  std::cout << "Reading from input tree:   dataAnarunsum"   << vm["threshold"].as<std::string>() 
	    << "/DataTree"                                  << std::endl;

  int TimeU_max = 0; int ChannelU_max = 0;
  int TimeV_max = 0; int ChannelV_max = 0;
  int TimeZ_max = 0; int ChannelZ_max = 0;

  std::vector<int> Times_U    = {};  std::vector<int> Times_V    = {};  std::vector<int> Times_Z    = {};
  std::vector<int> Channels_U = {};  std::vector<int> Channels_V = {};  std::vector<int> Channels_Z = {};

  int NColHits    = 0;
  int NIndHits    = 0;
  int startNumber = vm["nskip"].as<int>();
  int endNumber   = vm["nevent"] .as<int>() + startNumber;

  for (int CurrentEvent=startNumber; CurrentEvent<endNumber; ++CurrentEvent) {
    im->GetEntry(CurrentEvent);
    for (size_t it=0; it<im->Hit_View->size(); ++it) {
      if ((*im->Hit_View)[it] == 2) NColHits++;
      else                          NIndHits++;
    }

  }

  std::cout << "Number of collection hits: " << NColHits << std::endl;
  std::cout << "Number of induction hits:  " << NIndHits << std::endl;
  std::cout << std::endl;

  // int TimeU_min = *std::min_element(Times_U.begin(), Times_U.end()); 
  // int TimeV_min = *std::min_element(Times_V.begin(), Times_V.end()); 
  // int TimeZ_min = *std::min_element(Times_Z.begin(), Times_Z.end()); 
  // int ChannelU_min = *std::min_element(Channels_U.begin(), Channels_U.end());
  // int ChannelV_min = *std::min_element(Channels_V.begin(), Channels_V.end());
  // int ChannelZ_min = *std::min_element(Channels_Z.begin(), Channels_Z.end());

  // TH2D *eventDisplay_U = new TH2D("eventDisplay_U", "eventDisplay_U", 600, TimeU_min, TimeU_max, 250, ChannelU_min, ChannelU_max);
  // TH2D *eventDisplay_V = new TH2D("eventDisplay_V", "eventDisplay_V", 600, TimeV_min, TimeV_max, 250, ChannelV_min, ChannelV_max);
  // TH2D *eventDisplay_Z = new TH2D("eventDisplay_Z", "eventDisplay_Z", 600, TimeZ_min, TimeZ_max, 250, ChannelZ_min, ChannelZ_max);

  // std::ifstream infile(vm["text"].as<std::string>());
  // std::string line;

  // while (std::getline(infile, line)) {
  //   std::istringstream iss(line); int n; std::vector<int> v;
  //   while (iss >> n) v.push_back(n);

  //   int ChannelNumber = v[1];
  //   bool inU = false; bool inV = false; bool inZ = false;
  //   for (auto const& it: Channels_U)
  //     if (it == ChannelNumber) inU = true;
  //   for (auto const& it: Channels_V)
  //     if (it == ChannelNumber) inV = true;
  //   for (auto const& it: Channels_Z)
  //     if (it == ChannelNumber) inZ = true;

  //   if (inU) {
  //     for (int i=3; i<v.size(); ++i) {
  // 	if (v[0] == event)
  // 	  eventDisplay_U->Fill(i, ChannelNumber, v[i]);
  //     }}
  //   if (inV) {
  //     for (int i=3; i<v.size(); ++i) {
  // 	if (v[0] == event)
  // 	  eventDisplay_V->Fill(i, ChannelNumber, v[i]);
  //     }}
  //   if (inZ) {
  //     for (int i=3; i<v.size(); ++i) {
  // 	if (v[0] == event)
  // 	  eventDisplay_Z->Fill(i, ChannelNumber, v[i]);
  //     }}
  // }



  // TCanvas *c = new TCanvas("c", "c", 1600, 900);
  // c->Print((vm["output"].as<std::string>() + ".pdf[").c_str());
  // int n_u = Times_U.size(); int n_v = Times_V.size(); int n_z = Times_Z.size();
  // int t_u [Times_U.size()]; int t_v [Times_V.size()]; int t_z [Times_Z.size()];
  // int c_u [Times_U.size()]; int c_v [Times_V.size()]; int c_z [Times_Z.size()];
  // for (int i=0; i<n_u; ++i) { t_u[i] = Times_U[i]; c_u[i] = Channels_U[i]; }
  // for (int i=0; i<n_v; ++i) { t_v[i] = Times_V[i]; c_v[i] = Channels_V[i]; }
  // for (int i=0; i<n_z; ++i) { t_z[i] = Times_Z[i]; c_z[i] = Channels_Z[i]; }

  // TGraph *gU = new TGraph(n_u, t_u, c_u);
  // TGraph *gV = new TGraph(n_v, t_v, c_v);
  // TGraph *gZ = new TGraph(n_z, t_z, c_z);


  // // gU->Draw("AP");
  // // gV->Draw("P");
  // // gZ->Draw("P");


  // PlotTH2D(eventDisplay_U);
  // c->Print((vm["output"].as<std::string>() + ".pdf").c_str());

  // PlotTH2D(eventDisplay_V);
  // c->Print((vm["output"].as<std::string>() + ".pdf").c_str());

  // PlotTH2D(eventDisplay_Z);
  // c->Print((vm["output"].as<std::string>() + ".pdf").c_str());


  // c->Print((vm["output"].as<std::string>() + ".pdf]").c_str());




}
