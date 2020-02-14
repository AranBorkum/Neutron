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

std::vector<std::string> HitFindersThreshold = {"GausHit",
						"TrigPrim800",
						"TrigPrim1000",
						"TrigPrim1200",
						"TrigPrim1400",
						"TrigPrim1600",
						"TrigPrim1800",
						"TrigPrim2000",
						"TrigPrim2200",
						"TrigPrim2400",
						"TrigPrim2600",
						"TrigPrim2800",
						"TrigPrim3000",
						"TrigPrim3200"};

std::vector<std::string> HitFindersSigma = {"TrigPrim1",
					    "TrigPrim2",
					    "TrigPrim3",
					    "TrigPrim4",
					    "TrigPrim5",
					    "TrigPrim6",
					    "TrigPrim7",
					    "TrigPrim8",
					    "TrigPrim10",
					    "TrigPrim12",
					    "TrigPrim16",
					    "TrigPrim20"};

void ReadDataTrees(std::map<int, DataAnaInputManager*> raim, 
		   std::map<int, DataAnaInputManager*> taim, 
		   std::string real,
		   std::string truth,
		   bool threshold, 
		   bool sigma) {

  // --- If the hit finder was based on a flat threshold
  if (threshold) {
    // --- Read in the information from the truth root file
    std::map<int, DataAnaInputManager*> taim;
    taim[1]    = new DataAnaInputManager(); taim[1]   ->SetInputTree("dataAnarunsum1600/DataTree");

    // --- Read in the information from the root file
    std::map<int, DataAnaInputManager*> raim;
    raim[1]    = new DataAnaInputManager(); raim[1]   ->SetInputTree("dataAnagaushit/DataTree"   );
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
    it.second->SetInputFile(real);
    it.second->LoadTree();
  }
  for (auto& it: taim) {
    it.second->SetInputFile(truth);
    it.second->LoadTree();
  }

}


int main(int argc, char** argv) {

  // --- Command Line Flags -----------------------
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h"   , "produce help message")
    ("input,i"  , po::value<std::string>(), "ROOT file with the artdaq information inside" )
    ("real,r"   , po::value<std::string>(), "ROOT file with noise, i.e. the realistic file")
    ("true,t"   , po::value<std::string>(), "ROOT file with the absolute truth information")
    ("window,w" , po::value<int>        ()->default_value(10), "Hit timewindiw (in ticks)" )
    ("nevent,n" , po::value<int>        ()->default_value(-1), "The event number to run "  )
    ("output,o" , po::value<std::string>(), "The name for the output pdf file"             )
    ("threshold", "Flag if using run_runsum_MCAna.fcl"                                     )
    ("sigma"    , "Flag if using run_runsumsigma_MCAna.fcl"                                );

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help") || vm.empty()) {
    std::cout << desc << "\n"; return 1;
  }

  // --- Read in the information from the truth root file
  std::map<int, DataAnaInputManager*> taim;
  taim[1]    = new DataAnaInputManager(); taim[1]   ->SetInputTree("dataAnarunsum1600/DataTree");
  // taim[1]    = new DataAnaInputManager(); taim[1]   ->SetInputTree("dataAnagaushit/DataTree");

  // --- Read in the information from the root file
  std::map<int, DataAnaInputManager*> raim;
  if (vm.count("threshold")) {
    raim[1]    = new DataAnaInputManager(); raim[1]   ->SetInputTree("dataAnagaushit/DataTree"   );
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
  if (vm.count("sigma")) {
    raim[5 ] = new DataAnaInputManager(); raim[5 ]->SetInputTree("dataAnarunsum5/DataTree" );
    raim[6 ] = new DataAnaInputManager(); raim[6 ]->SetInputTree("dataAnarunsum6/DataTree" );
    raim[7 ] = new DataAnaInputManager(); raim[7 ]->SetInputTree("dataAnarunsum7/DataTree" );
    raim[8 ] = new DataAnaInputManager(); raim[8 ]->SetInputTree("dataAnarunsum8/DataTree");
    raim[10] = new DataAnaInputManager(); raim[10]->SetInputTree("dataAnarunsum10/DataTree");
    raim[12] = new DataAnaInputManager(); raim[12]->SetInputTree("dataAnarunsum12/DataTree");
    raim[16] = new DataAnaInputManager(); raim[16]->SetInputTree("dataAnarunsum16/DataTree");
    raim[20] = new DataAnaInputManager(); raim[20]->SetInputTree("dataAnarunsum20/DataTree");
  }

  // --- Set all the inputs for the input as defined above
  for (auto& it: raim) {
    it.second->SetInputFile(vm["real"].as<std::string>());
    it.second->LoadTree();
  }
  for (auto& it: taim) {
    it.second->SetInputFile(vm["true"].as<std::string>());
    it.second->LoadTree();
  }

  // --- Define the plots to be filled by the script
  TH1D *FalseHitRateCollection = new TH1D("FalseHitRateCollection", "",  14, 0, 14);
  TH1D *FalseHitRateInduction  = new TH1D("FalseHitRateInduction" , "",  14, 0, 14);
  TH1D *IDEElectrons           = new TH1D("IDEElectrons"          , "",  50, 0, 7000);
  TH1D *IDEChannel             = new TH1D("IDEChannel"            , "",  50, 0, 40000);
  TH1D *HitChannel             = new TH1D("HitChannel"            , "",  50, 0, 40000);

  std::map<int, TEfficiency*> effHitsZ;  
  std::map<int, TEfficiency*> effHitsV;  

  int HitFinderCounter = 0;
  DataAnaInputManager* data = taim[1];
  for (auto const& it: raim) {
    int adc_threshold       = it.first;
    DataAnaInputManager* im = it.second;

    int nevent  = vm["nevent"].as<int>();
    int fNEvent = 0;
    if (nevent == -1) fNEvent = im->GetEntries();
    else              fNEvent = nevent;

    effHitsZ[adc_threshold] = new TEfficiency(Form("EffZ_Th%i", adc_threshold), 
					      "Collection Plane;nElectrons on wire;Efficiency", 50, 0, 40000);
    effHitsV[adc_threshold] = new TEfficiency(Form("EffV_Th%i", adc_threshold), 
					      "Induction Plane;nElectrons on wire;Efficiency", 50, 0, 40000);

    // Fill the IDE information histogram
    if (adc_threshold == 800) {
      for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
	im->GetEntry(CurrentEvent);
	for (int i=0; i<im->IDEChannel  ->size(); ++i) IDEChannel  ->Fill((*im->IDEChannel)  [i]);
      } // Looping through current event
    } // If ADC threshold is 800

    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      std::cout << "ADC Threshold " << adc_threshold << std::endl;
      im  ->GetEntry(CurrentEvent);
      data->GetEntry(CurrentEvent);

      // --- Store truth information ---
      std::vector<int> TrueHitView;
      std::vector<int> TrueHitChan;
      std::vector<int> TrueHitTime;
      for (size_t hit=0; hit<data->Hit_View->size(); ++hit) {
	TrueHitView.push_back((*data->Hit_View)[hit]);
	TrueHitChan.push_back((*data->Hit_Chan)[hit]);
	TrueHitTime.push_back((*data->Hit_Time)[hit]);
      } // Looping through the hits

      // --- Match hits to truth ---
      int matchedHits = 0;
      int width       = vm["window"].as<int>();
      std::vector<int> MatchedHitView(TrueHitView.size(), 0);
      std::vector<int> MatchedHitChan(TrueHitChan.size(), 0);
      std::vector<int> MatchedHitTime(TrueHitTime.size(), 0);
      for (size_t hit=0; hit<im->Hit_View->size(); ++hit) {
	for (size_t thit=0; thit<TrueHitView.size(); ++thit) {
	  bool matchedView = (*im->Hit_View)[hit] == TrueHitView[thit];
	  bool matchedChan = (*im->Hit_Chan)[hit] == TrueHitChan[thit];
	  bool matchedTime = ((*im->Hit_Time)[hit]>TrueHitTime[thit]-width &&
			      (*im->Hit_Time)[hit]<TrueHitTime[thit]+width);

	  HitChannel->Fill((*im->Hit_Chan)[hit]);
	  if (matchedView && matchedChan && matchedTime) {
	    matchedHits++;
	    MatchedHitView[thit] = (*im->Hit_View)[hit];
	    MatchedHitChan[thit] = (*im->Hit_Chan)[hit];
	    MatchedHitTime[thit] = (*im->Hit_Time)[hit];
	  } // If matched do something
	} // Looping through true hits
      } // Looping through hits    
      
      // Hit Matching?
      std::vector<std::string> filenames(1, vm["input"].as<std::string>());
      int eventNum = 0;
      for (gallery::Event e(filenames); !e.atEnd(); e.next()) {
	if (eventNum != CurrentEvent) {eventNum++; continue;}

	auto const& truth_handle_tpc=e.getValidHandle<std::vector<sim::SimChannel>>("largeant");
	auto& truth_tpc_in =*truth_handle_tpc;

	for (auto&& truth: truth_tpc_in) {
	  for(int ichge=0; ichge<4492; ++ichge){
	    double nelec  = truth.Charge(ichge);
	    double energy = truth.Energy(ichge);
	    size_t chan   = truth.Channel();
	    size_t plane  = 0;
	    if (chan%2560 < 800)                      plane = 0;
	    if (chan%2560 < 1600 && chan%2560 >= 800) plane = 1;
	    else                                      plane = 2;

	    if (nelec) {
	      bool matched = false;
	      for (size_t hit=0; hit<im->Hit_Chan->size(); ++hit) {
		if ((*im->Hit_Chan)[hit] != chan) continue;

		if ((*im->Hit_Time)[hit] - width <= ichge && 
		    ichge <= (*im->Hit_Time)[hit] + width) {
		  matched = true;
		  break;
		}
	      }
	      IDEElectrons->Fill(nelec);
	      if (plane == 2) effHitsZ[adc_threshold]->Fill(matched, nelec);
	      if (plane != 2) effHitsV[adc_threshold]->Fill(matched, nelec);

	    } // If there are electrons
	  } // Looping through ticks
	} // Looping through truth samples
	eventNum++;
      } // Looping through even


      
      // Matching hits to energy depositionn
      for (int ide=0; ide<raim[800]->IDEElectrons->size(); ++ide) {
	for (int hit=0; hit<im->Hit_Time->size(); ++hit) {
	  int   hitTime      = (*im->Hit_Time)[hit];
	  int   window       = vm["window"].as<int>();
	  float IDEStartTime = (*raim[800]->IDEStartTime)[ide];
	  float IDEEndTime   = (*raim[800]->IDEEndTime  )[ide];
	  bool  matched      = ((hitTime+window) < IDEEndTime && (hitTime-window) > IDEStartTime);

	} // Looping through hits
      } // Looping through IDEs
      



      // --- Fill the FalseHitTime histogram
      // Fill for gaushit because it's gaus-shit
      if (HitFinderCounter == 0) {
	int nHits = im->Hit_View->size() - matchedHits;
	if (nHits < 0) nHits = 0;
	FalseHitRateCollection->Fill(HitFinderCounter, nHits);
      } // If gaushit

      if (HitFinderCounter != 0) {
	for (int i=0; i<im->Hit_View->size(); ++i) {
	  if ((*im->Hit_View)[i] == 2) FalseHitRateCollection->Fill(HitFinderCounter);
	  if ((*im->Hit_View)[i] != 2) FalseHitRateInduction ->Fill(HitFinderCounter);
	}
      } // If not gaushit




    } // Looping through current event
    
    HitFinderCounter++;
    
  } // Looping through raim

  // --- Formatting the color scheme and legend
  const std::vector<int> color = getColors();
  int i=0;
  TLegend* legend = new TLegend(0.1,0.1,0.9,0.9);
  for (auto const& it: effHitsZ) {
    effHitsZ[it.first]->SetLineColor  (color.at(i));
    effHitsV[it.first]->SetLineColor  (color.at(i));
    effHitsZ[it.first]->SetMarkerColor(color.at(i));
    effHitsV[it.first]->SetMarkerColor(color.at(i));
    legend->AddEntry(effHitsZ[it.first], Form("ADC Threshold = %i",it.first/100), "L");
    i++;
  }

  // --- Plot everything and export to a PDF
  TCanvas *c = new TCanvas("c", "c");
  c->Print((vm["output"].as<std::string>() + ".pdf[").c_str());
  
  IDEElectrons->Draw();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  IDEChannel->Draw();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
  
  effHitsZ[800]->Draw();
  for (auto const& it: effHitsZ) {
    if (it.first != 1 && it.first != 800)
      it.second->Draw("SAME");
  }
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  effHitsV[800]->Draw();
  for (auto const& it: effHitsV) {
    if (it.first != 1 && it.first != 800)
      it.second->Draw("SAME");
  }
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  c->SetLogy(1);
  FalseHitRateCollection->SetStats(0);
  FalseHitRateCollection->SetBit(TAxis::kLabelsHori);
  FalseHitRateCollection->GetYaxis()->SetTitle("False hits / 10kt module / s");
  FalseHitRateCollection->GetXaxis()->SetLabelSize(0.04);

  std::vector<std::string> HitFinders;
  if (vm.count("threshold")) HitFinders = HitFindersThreshold;
  if (vm.count("sigma"))     HitFinders = HitFindersSigma;
  for (size_t it=0; it<HitFinders.size(); ++it)
    FalseHitRateCollection->GetXaxis()->SetBinLabel(it+1, HitFinders[it].c_str());
  FalseHitRateCollection->Scale(1 / (raim[1]->GetEntries() * 0.002246));
  FalseHitRateCollection->Draw();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  FalseHitRateInduction->SetStats(0);
  FalseHitRateInduction->SetBit(TAxis::kLabelsHori);
  FalseHitRateInduction->GetYaxis()->SetTitle("False hits / 10kt module / s");
  FalseHitRateInduction->GetXaxis()->SetLabelSize(0.04);
  for (size_t it=0; it<HitFinders.size(); ++it)
    FalseHitRateInduction->GetXaxis()->SetBinLabel(it+1, HitFinders[it].c_str());
  FalseHitRateInduction->Scale(1 / (raim[1]->GetEntries() * 0.002246));
  FalseHitRateInduction->Draw();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());

  c->Clear();
  legend->Draw();
  c->Print((vm["output"].as<std::string>() + ".pdf" ).c_str());
  c->Print((vm["output"].as<std::string>() + ".pdf]").c_str());

}
