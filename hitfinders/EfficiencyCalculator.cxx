#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TColor.h"
#include "TStyle.h"
#include "TPad.h"
#include "TEfficiency.h"

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"

#include "lardataobj/Simulation/SimChannel.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RawData/RDTimeStamp.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"
#include "lardataobj/RawData/raw.h"

#include "DataAnaInputManager.hh"
#include "Helper.h"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

std::vector<std::string> HitFinders = {"runsum800",
				       "runsum1200",
				       "runsum1400",
				       "runsum1600",
				       "runsum1800",
				       "runsum2000",
				       "runsum2200",
				       "runsum2400",
				       "runsum2600",
				       "runsum2800",
				       "runsum3000",
				       "runsum3200"};

int main(int argc, char** argv) {

  // --- Command Line Flags -----------------------
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"   , "produce help message")
    ("input,i"  , po::value<std::string>(), "Name of ROOT file without _$ or extension"    )
    ("window,w" , po::value<int>        ()->default_value(10), "Hit timewindiw (in ticks)" )
    ("output,o" , po::value<std::string>(), "The name for the output pdf file"             )
    ("threshold", "Flag if using run_runsum_MCAna.fcl"                                     )
    ("sigma"    , "Flag if using run_runsumsigma_MCAna.fcl"                                );

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help") || vm.empty()) {
    std::cout << desc << "\n"; return 1;
  }
  
  // --- Read in the information from the root file
  std::map<int, DataAnaInputManager*> raim;
  std::map<int, DataAnaInputManager*> caim;
  if (vm.count("threshold")) {
    caim[1]    = new DataAnaInputManager(); caim[1]   ->SetInputTree("dataAnarunsum1600/DataTree");
    raim[800]  = new DataAnaInputManager(); raim[800] ->SetInputTree("dataAnarunsum800/DataTree" );
    raim[1200] = new DataAnaInputManager(); raim[1200]->SetInputTree("dataAnarunsum1200/DataTree");
    raim[1400] = new DataAnaInputManager(); raim[1400]->SetInputTree("dataAnarunsum1400/DataTree");
    raim[1600] = new DataAnaInputManager(); raim[1600]->SetInputTree("dataAnarunsum1600/DataTree");
    raim[1800] = new DataAnaInputManager(); raim[1800]->SetInputTree("dataAnarunsum1800/DataTree");
    raim[2000] = new DataAnaInputManager(); raim[2000]->SetInputTree("dataAnarunsum2000/DataTree");
    raim[2200] = new DataAnaInputManager(); raim[2200]->SetInputTree("dataAnarunsum2200/DataTree");
    raim[2400] = new DataAnaInputManager(); raim[2400]->SetInputTree("dataAnarunsum2400/DataTree");
    raim[2600] = new DataAnaInputManager(); raim[2600]->SetInputTree("dataAnarunsum2600/DataTree");
    raim[2800] = new DataAnaInputManager(); raim[2800]->SetInputTree("dataAnarunsum2800/DataTree");
    raim[3000] = new DataAnaInputManager(); raim[3000]->SetInputTree("dataAnarunsum3000/DataTree");
    raim[3200] = new DataAnaInputManager(); raim[3200]->SetInputTree("dataAnarunsum3200/DataTree");
  }

  // --- Set all the inputs for the input as defined above
  for (auto& it: raim) {
    if (vm.count("threshold"))
      it.second->SetInputFile((vm["input"].as<std::string>()+"_threshold.root").c_str());
    it.second->LoadTree();
  }
  for (auto& it: caim) {
    it.second->SetInputFile((vm["input"].as<std::string>()+"_clean.root").c_str());
    it.second->LoadTree();
  }

  // --- Definne the plots to be filled by the script
  TH1D        *HitView    = new TH1D       ("HitView"   , ""              ,   3, 0, 3);
  TH1D        *nEonWire   = new TH1D       ("nEonWire"  , ""              ,  20, 0, 7000);
  TH1D        *WireE      = new TH1D       ("WireE"     , ";Energy [MeV];",  50, 0,  3);
  TH1D        *Holder     = new TH1D       ("Holder"    , ""              ,  13, 0, 13);
  TEfficiency *ZPurity    = new TEfficiency("ZPurity"   , ";;Purity"      ,  13, 0, 13);
  TEfficiency *VPurity    = new TEfficiency("VPurity"   , ";;Purity"      ,  13, 0, 13);
  TEfficiency *UPurity    = new TEfficiency("UPurity"   , ";;Purity"      ,  13, 0, 13);
  TEfficiency *ZFound     = new TEfficiency("ZFound"    , ";;Found"       ,  13, 0, 13);
  TEfficiency *VFound     = new TEfficiency("VFound"    , ";;Found"       ,  13, 0, 13);
  TEfficiency *UFound     = new TEfficiency("UFound"    , ";;Found"       ,  13, 0, 13);
  TH2D        *CtoE       = new TH2D       ("CtoE"      , ";Charge;Energy",  20, 0, 7000, 20, 0, 1);  
  std::map<int, TEfficiency*> EfficiencyCharge;
  std::map<int, TEfficiency*> EfficiencyEnergy;

  std::vector<double> chargeOnWire;
  std::vector<double> energyOnWire;

  // --- Start interacting with the data
  int HitFinderCounter = 0;
  DataAnaInputManager* clean = caim[1];
  for (auto const& it: raim) {
    int adc_threshold       = it.first;
    DataAnaInputManager* im = it.second;

    EfficiencyCharge[adc_threshold] = new TEfficiency(Form("EffCh_Th%i", adc_threshold), "",  20, 0, 7000);
    EfficiencyEnergy[adc_threshold] = new TEfficiency(Form("EffEn_Th%i", adc_threshold), "",  20, 0,    3);

    for (int CurrentEvent=0; CurrentEvent<im->GetEntries(); ++CurrentEvent) {
      im->GetEntry(CurrentEvent);
      clean->GetEntry(CurrentEvent);
      // Fill the hit view
      for (int i=0; i<im->Hit_View->size(); ++i) {
	HitView->Fill((*im->Hit_View)[i]);
      }

      // Match Hits with noise
      int nHitsZ     = 0; int nHitsTrueZ = 0; int nMatchedZ  = 0;
      int nHitsV     = 0; int nHitsTrueV = 0; int nMatchedV  = 0;
      int nHitsU     = 0; int nHitsTrueU = 0; int nMatchedU  = 0;

      for (int j=0; j<clean->Hit_View->size(); ++j) {
	if ((*im->Hit_View)[j] == 2) nHitsTrueZ++;
	if ((*im->Hit_View)[j] == 1) nHitsTrueV++;
	if ((*im->Hit_View)[j] == 0) nHitsTrueU++;
      }

      int window = vm["window"].as<int>();
      for (int j=0; j<im->Hit_View->size(); ++j) {
	if ((*im->Hit_View)[j] == 2) nHitsZ++;
	if ((*im->Hit_View)[j] == 1) nHitsV++;
	if ((*im->Hit_View)[j] == 0) nHitsU++;
	for (int i=0; i<clean->Hit_View->size(); ++i) {      
	  bool matchChan = (*im->Hit_Chan)[j] == (*clean->Hit_Chan)[i];
	  bool matchTime = ((*im->Hit_Time)[j]-window <= (*clean->Hit_Time)[i] &&
			    (*im->Hit_Time)[j]+window+(*im->Hit_RMS)[j] >= (*clean->Hit_Time)[i]);
	  if (matchChan && matchTime && (*im->Hit_View)[j]==2) nMatchedZ++;
	  if (matchChan && matchTime && (*im->Hit_View)[j]==1) nMatchedV++;
	  if (matchChan && matchTime && (*im->Hit_View)[j]==0) nMatchedU++;
	}
      }

      for (int i=0; i<nHitsZ; ++i) 
	ZPurity->Fill(i<nMatchedZ, HitFinderCounter);
      for (int i=0; i<nHitsV; ++i) 
	VPurity->Fill(i<nMatchedV, HitFinderCounter);
      for (int i=0; i<nHitsU; ++i) 
	UPurity->Fill(i<nMatchedU, HitFinderCounter);

      for (int i=0; i<nHitsZ; ++i) 
	ZFound->Fill(i<nHitsTrueZ, HitFinderCounter);
      for (int i=0; i<nHitsV; ++i) 
	VFound->Fill(i<nHitsTrueV, HitFinderCounter);
      for (int i=0; i<nHitsU; ++i) 
	UFound->Fill(i<nHitsTrueU, HitFinderCounter);

      // Extraction of SimChannel information
      std::vector<std::string> filenames(1, (vm["input"].as<std::string>()+".root").c_str());
      int eventNum = 0;
      for (gallery::Event e(filenames); !e.atEnd(); e.next()) {
	if (eventNum != CurrentEvent) {eventNum++; continue;}

	auto const& truth_handle_tpc=e.getValidHandle<std::vector<sim::SimChannel>>("largeant");
	auto& truth_tpc_in =*truth_handle_tpc;

	for (auto&& truth: truth_tpc_in) {
	  for (int i=0; i<4492; ++i) {
	    int    Channel    = truth.Channel();
	    double nElectrons = truth.Charge(i); 
	    double Energy     = truth.Energy(i);

	    if (nElectrons) {
	      nEonWire->Fill(nElectrons); 
	      WireE   ->Fill(Energy    ); 
	      CtoE    ->Fill(nElectrons, Energy);
	      bool matched = false;
	      for (int hit=0; hit<im->Hit_View->size(); ++hit) {
		if ((*im->Hit_Chan)[hit] != Channel) continue;
		if ((*im->Hit_Time)[hit]-window <= i &&
		    (*im->Hit_Time)[hit]+window+(*im->Hit_RMS)[hit] >= i) {
		  matched = true;
		  break;
		}
	      }
	      EfficiencyCharge[adc_threshold]->Fill(matched, nElectrons);
	      EfficiencyEnergy[adc_threshold]->Fill(matched, Energy    );
	    }
	  }
	}

	eventNum++;
      }


      HitFinderCounter++;
    } // Loop through the events


  } // Loop through raim

  




  const std::vector<int> color = getColors();
  TLegend* legend = new TLegend(0.1,0.1,0.9,0.9);
  Holder  ->SetStats(0);
  nEonWire->SetStats(0);
  WireE   ->SetStats(0);
  CtoE    ->SetStats(0);
  ZPurity ->SetLineColor  (color.at(0));
  VPurity ->SetLineColor  (color.at(1));
  UPurity ->SetLineColor  (color.at(2));
  ZFound  ->SetLineColor  (color.at(0));
  VFound  ->SetLineColor  (color.at(1));
  UFound  ->SetLineColor  (color.at(2));

  ZPurity->SetMarkerColor(color.at(0));
  VPurity->SetMarkerColor(color.at(1));
  UPurity->SetMarkerColor(color.at(2));
  ZFound ->SetMarkerColor(color.at(0));
  VFound ->SetMarkerColor(color.at(1));
  UFound ->SetMarkerColor(color.at(2));
  legend->AddEntry(ZPurity, "Z - Plane", "L");
  legend->AddEntry(VPurity, "V - Plane", "L");
  legend->AddEntry(UPurity, "U - Plane", "L");
  
  int i = 0;
  TLegend* legend1 = new TLegend(0.1,0.1,0.9,0.9);
  for (auto const& it: EfficiencyCharge) {
    EfficiencyCharge[it.first]->SetLineColor  (color.at(i));
    EfficiencyEnergy[it.first]->SetLineColor  (color.at(i));
    EfficiencyCharge[it.first]->SetMarkerColor(color.at(i));
    EfficiencyEnergy[it.first]->SetMarkerColor(color.at(i));
    legend1->AddEntry(EfficiencyCharge[it.first], Form("ADC Threshold = %i", it.first/100), "L");
    i++;
  }

  TCanvas *c = new TCanvas("c", "c");
  c->Print((vm["output"].as<std::string>() + ".pdf[").c_str());

  HitView->Draw();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  // ---
  TPad *p1 = new TPad("p1", "", 0, 0, 1, 1);
  TPad *p2 = new TPad("p2", "", 0, 0, 1, 1);
  p2->SetFillStyle(4000); // will be transparent
  p1->Draw();
  p1->cd();
  nEonWire->Draw();
  gPad->Update();

  std::map<int, TGraphAsymmErrors*> effChargeGraphs;
  for (auto const& it: EfficiencyCharge)
    effChargeGraphs[it.first] = it.second->CreateGraph();

  Double_t xmin = p1->GetUxmin();
  Double_t xmax = p1->GetUxmax();
  Double_t dx = (xmax - xmin) / 0.8; // 10 percent margins left and right

  Double_t ymin = DBL_MAX;
  Double_t ymax = DBL_MIN;
  for (auto const& it: effChargeGraphs) {
    if (it.second->GetHistogram()->GetMinimum() < ymin) ymin = it.second->GetHistogram()->GetMinimum();
    if (it.second->GetHistogram()->GetMaximum() > ymax) ymax = it.second->GetHistogram()->GetMaximum();
  }
  Double_t dy = (ymax - ymin) / 0.8; // 10 percent margins top and bottom
  p2->Range(xmin-0.1*dx, ymin-0.1*dy, xmax+0.1*dx, ymax+0.1*dy);

  p2->Draw();
  p2->cd();
  for (auto const& it: effChargeGraphs)
    it.second->Draw("P");

  gPad->Update();

  TGaxis *axis = new TGaxis(xmax, ymin, xmax, ymax, ymin, ymax, 510, "+L");
  axis->SetLineColor(kBlack);
  axis->SetLabelColor(kBlack);
  axis->Draw();
  gPad->Update();
  c->cd();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
  // ---

  // ---
  TPad *p3 = new TPad("p3", "", 0, 0, 1, 1);
  TPad *p4 = new TPad("p4", "", 0, 0, 1, 1);
  p4->SetFillStyle(4000); // will be transparent
  p3->Draw();
  p3->cd();
  WireE->Draw();
  gPad->Update();


  std::map<int, TGraphAsymmErrors*> effEnergyGraphs;
  for (auto const& it: EfficiencyEnergy)
    effEnergyGraphs[it.first] = it.second->CreateGraph();

  xmin = p3->GetUxmin();
  xmax = p3->GetUxmax();
  dx = (xmax - xmin) / 0.8; // 10 percent margins left and right
  ymin = DBL_MAX;
  ymax = DBL_MIN;
  for (auto const& it: effEnergyGraphs) {
    if (it.second->GetHistogram()->GetMinimum() < ymin) ymin = it.second->GetHistogram()->GetMinimum();
    if (it.second->GetHistogram()->GetMaximum() > ymax) ymax = it.second->GetHistogram()->GetMaximum();
  }
  dy = (ymax - ymin) / 0.8; // 10 percent margins top and bottom
  p4->Range(xmin-0.1*dx, ymin-0.1*dy, xmax+0.1*dx, ymax+0.1*dy);

  p4->Draw();
  p4->cd();
  for (auto const& it: effEnergyGraphs)
    it.second->Draw("P");

  gPad->Update();

  TGaxis *axis1 = new TGaxis(xmax, ymin, xmax, ymax, ymin, ymax, 510, "+L");
  axis1->SetLineColor(kBlack);
  axis1->SetLabelColor(kBlack);
  axis1->Draw();
  gPad->Update();
  c->cd();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
  // ---

  CtoE->Draw("COLZ");
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  PlotAll(EfficiencyCharge);
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  PlotAll(EfficiencyEnergy);
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
  
  c->Clear();
  legend1->Draw();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  Holder ->Draw();
  ZPurity->Draw("SAME");
  VPurity->Draw("SAME");
  UPurity->Draw("SAME");
  Holder->GetYaxis()->SetTitle("Purity");
  Holder->SetBit(TAxis::kLabelsHori);
  Holder->GetXaxis()->SetLabelSize(0.04);
  for (size_t it=0; it<HitFinders.size(); ++it)
    Holder->GetXaxis()->SetBinLabel(it+1, HitFinders[it].c_str());
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  c->Clear();
  legend->Draw();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
  c->Print((vm["output"].as<std::string>() + ".pdf]").c_str());



}
