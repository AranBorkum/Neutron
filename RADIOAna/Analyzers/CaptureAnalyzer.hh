#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TError.h"

#include "../Utilities/Helper.h"
#include "InputManager.hh"
#include "Capture.hh"
#include "boost/filesystem.hpp"

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <cmath>

double neutronMass = 939.56541;
double cryoDimX[2] = {-947,  947};
double cryoDimY[2] = {-887,  887};
double cryoDimZ[2] = {-493, 6091};

double APASpanX[2] = {-733.6944, 733.6944};
double APASpanY[2] = {-606.0   , 606.0   };
double APASpanZ[2] = {-443.8   , 5789.8  };

bool isInCryostat(double x, double y, double z) {

  bool isWithin  = false;
  bool isWithinX = (x<APASpanX[1] && x>APASpanX[0]);
  bool isWithinY = (y<APASpanY[1] && y>APASpanY[0]);
  bool isWithinZ = (z<APASpanZ[1] && z>APASpanZ[0]);

  if (isWithinX && isWithinY && isWithinZ)
    isWithin = true;
  return isWithin;
}

int CaptureAna(ParsedXML xml, int nEvent) {

  // --- Read in the input TFile and open the TTree
  FullGeoAnaInputManager *im = new FullGeoAnaInputManager();
  im->SetInputFile((xml.GetDirectory() + xml.GetFile()).c_str());
  im->SetInputTree(xml.GetTree());
  im->LoadTree();

  int fNEvent = nEvent;
  if (fNEvent != -1) { fNEvent = std::min(fNEvent, (int)im->GetEntries()); } 
  else               { fNEvent = im->GetEntries();                         }
  
  int nPrimaryNeutrons   = 0;
  int nSecondaryNeutrons = 0;
  int nCapturesOnLAr     = 0;
  std::vector<std::string> materials;
  std::vector<Capture*>     Captures;

  TH1D     *neutronGenE     = new TH1D    ("neutronGenE"    , "neutronGenE"    ,  200, 0, 10);
  TH1D     *gammaCapE       = new TH1D    ("gammaCapE"      , "gammaCapE"      , 1000, 0, 10);
  TH1D     *endMaterial     = new TH1D    ("endMaterial"    , "endMaterial"    ,   11, 0, 11);
  TH1D     *nGamma          = new TH1D    ("nGamma"         , "nGamma"         ,   15, 0, 15);
  TH1D     *nNeutonsScatter = new TH1D    ("nNeutronScatter", "nNeutronScatter",   15, 0, 15);
  TH2D     *nCaptureXY      = new TH2D    ("nCaptureXY"     , "nCaptureXY"     ,  500, -1300,  1300,  500, -1500, 3000);
  TH2D     *nCaptureYZ      = new TH2D    ("nCaptureYZ"     , "nCaptureYZ"     , 2000, -1000,  15000, 2000, -1300, 3000);
  TH2D     *nStartXY        = new TH2D    ("nStartXY"       , "nStartXY"       ,  500, -1300,  1300,  500, -1500, 3000);
  TH2D     *nStartYZ        = new TH2D    ("nStartYZ"       , "nStartYZ"       , 2000, -1000,  15000, 2000, -1300, 3000);
  TH2D     *nCaptureXYInLAr = new TH2D    ("nCaptureXYInLAr", "nCaptureXYInLAr", 100, -1000, 1000, 100, -1000, 1000);
  TH2D     *nCaptureYZInLAr = new TH2D    ("nCaptureYZInLAr", "nCaptureYZInLAr", 200, -1000, 7000, 100, -1000, 1000);
  TProfile *capTypeE        = new TProfile("capTypeE"       , "capTypeE"       ,   11,     0,    11,    0,    20);

  std::map<int, TH1D*> nTravelInMatter;

  // --- Get list of all of the materials in the TTree
  for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
    im->GetEntry(CurrentEvent);

    // std::cout << "Filling list of materials present in file" << " ";
    // float prog = (float)CurrentEvent / (float)fNEvent;
    // progress(prog);

    for (auto const& it: *im->True_Bck_EndMaterial) {
      bool inVector = false;
      std::string CurrentMaterial = it;
      if (it.substr(0, 5) == "Radio")
	CurrentMaterial = it.substr(5, -1);
      for (auto const itt: materials)
	if (CurrentMaterial == itt) inVector = true;
      if (!inVector) {
	materials.push_back(CurrentMaterial);
      }
    }
  }
  // std::cout << std::endl;

  for (size_t i=0; i<materials.size(); ++i) {
    nTravelInMatter[i] = new TH1D(Form("TravelIn%s", materials[i].c_str()), ";Travel Distance [mm];", 200, 0, 100);
  }


  // --- Main Analysis Loop -----------------------------------------------------
  for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
    im->GetEntry(CurrentEvent);

    // std::cout << "NOW PROCESSING EVENT " << CurrentEvent << " " << std::flush;
    // float prog = (float)CurrentEvent / (float)fNEvent;
    // progress(prog);

    std::vector<int> MotherNeutronIds = {};
    std::vector<int> ScattersForEach  = {};
    for (size_t it=0; it<im->True_Bck_PDG->size(); ++it) {

      bool isNeutron  = (*im->True_Bck_PDG)[it]        == 2112;
      bool isCaptured = (*im->True_Bck_EndProcess)[it] ==   10;
      bool isMother   = (*im->True_Bck_Mother)[it]     ==    0;

      if (isNeutron && isMother) {MotherNeutronIds.push_back((*im->True_Bck_ID)[it]); ScattersForEach.push_back(0);}
      if (isNeutron && !isCaptured) {
	int myID = (*im->True_Bck_ID)[it];
	for (size_t i=0; i<MotherNeutronIds.size(); ++i) {
	  if (myID == MotherNeutronIds[i]) {
	    MotherNeutronIds[i] = myID;
	    ScattersForEach[i]++;
	  }
	}
      }
    }

    for (auto const& i: ScattersForEach) nNeutonsScatter->Fill(i);

    std::vector<double> empty;
    std::vector<double> photons;
    for (size_t it=0; it<im->True_Bck_PDG->size(); ++it) {
    
      bool isNeutron  = (*im->True_Bck_PDG)[it]        == 2112;
      bool isCaptured = (*im->True_Bck_EndProcess)[it] ==   10;

      // --- Fill the capture type histogram
      if (isCaptured && isNeutron)
	for (size_t itt=0; itt<materials.size(); ++itt) {
	  std::string WorkingMaterial = (*im->True_Bck_EndMaterial)[it];
	  if ((*im->True_Bck_EndMaterial)[it].substr(0, 5) == "Radio")
	    WorkingMaterial = (*im->True_Bck_EndMaterial)[it].substr(5, -1);
	  if (WorkingMaterial == materials[itt])
	    endMaterial->Fill(itt);

	}

      // --- Count the number of primary and secondary neutrons
      if (isNeutron && (*im->True_Bck_Mother)[it] == 0) { 
	nPrimaryNeutrons++; 
	neutronGenE->Fill((*im->True_Bck_Energy)[it]*1000 - neutronMass);
	nStartXY->Fill((*im->True_Bck_VertX)[it], (*im->True_Bck_VertY)[it]);
	nStartYZ->Fill((*im->True_Bck_VertZ)[it], (*im->True_Bck_VertY)[it]);
      }
      if (isNeutron && (*im->True_Bck_Mother)[it] != 0) 
	{ nSecondaryNeutrons++; }

      // --- Filling a vector of all the photons post capture
      if (isNeutron && isCaptured) 
	for (size_t itt=0; itt<im->True_Bck_Mother->size(); ++itt) 
	  if ((*im->True_Bck_Mother)[itt] == (*im->True_Bck_ID)[it] && (*im->True_Bck_PDG)[itt] == 22) 
	    photons.push_back((*im->True_Bck_Energy)[itt]);

      // --- Find the neutron travel information
      std::string startMaterial; std::string endMaterial;
      if ((*im->True_Bck_StartMaterial)[it].substr(0, 5) == "Radio") startMaterial = (*im->True_Bck_StartMaterial)[it].substr(5, -1);
      else                                                           startMaterial = (*im->True_Bck_StartMaterial)[it];
      if ((*im->True_Bck_EndMaterial)[it]  .substr(0, 5) == "Radio") endMaterial = (*im->True_Bck_EndMaterial)[it].substr(5, -1);
      else                                                           endMaterial = (*im->True_Bck_EndMaterial)[it];

      if (isNeutron && isCaptured && startMaterial == endMaterial) {
      	double dX	= (double)(*im->True_Bck_VertX)[it] - (double)(*im->True_Bck_EndX)[it];
      	double dY	= (double)(*im->True_Bck_VertY)[it] - (double)(*im->True_Bck_EndY)[it];
      	double dZ	= (double)(*im->True_Bck_VertZ)[it] - (double)(*im->True_Bck_EndZ)[it];
      	double dTotal	= pow(dX, 2) + pow(dY, 2) + pow(dZ, 2);

	for (size_t mat=0; mat<materials.size(); ++mat)
	  if (materials[mat] == startMaterial)
	    nTravelInMatter[mat]->Fill(sqrt(dTotal));
      }

      // --- Count the number of captures on LAr
      if (isNeutron && isCaptured && (*im->True_Bck_EndMaterial)[it] == "LAr")
	  // && isInCryostat((*im->True_Bck_EndX)[it], 
	  // 		  (*im->True_Bck_EndY)[it], 
	  // 		  (*im->True_Bck_EndZ)[it]))
	nCapturesOnLAr++;
	    
      if (isNeutron && isCaptured) {
	std::string thisMaterial;
	if ((*im->True_Bck_EndMaterial)[it].substr(0, 5) == "Radio")
	  thisMaterial = (*im->True_Bck_EndMaterial)[it].substr(5, -1);
	else
	  thisMaterial = (*im->True_Bck_EndMaterial)[it];
	
	Capture *cap = new Capture(thisMaterial,
				   (*im->True_Bck_EndX)       [it],
				   (*im->True_Bck_EndY)       [it],
				   (*im->True_Bck_EndZ)       [it],
				   (*im->True_Bck_Energy)     [it],
				   (*im->True_Bck_EndE)       [it],
				   photons);
      
	Captures.push_back(cap);
	photons = empty; 
      }
    }
  }

  // --- Fill the histograms
  for (auto const& it: Captures) {
    neutronGenE->Fill(it->GetCaptureStartE  ()*1000 - neutronMass);

    if (it->GetNPhotons()) {
      gammaCapE  ->Fill(it->GetSumPhotonEnergy());
      nGamma     ->Fill(it->GetNPhotons       ());
    }
    nCaptureXY ->Fill(it->GetEndPointX(), it->GetEndPointY());
    nCaptureYZ ->Fill(it->GetEndPointZ(), it->GetEndPointY());
    
    if (it->GetEndMaterial() == "LAr") {
      nCaptureXYInLAr ->Fill(it->GetEndPointX(), it->GetEndPointY());
      nCaptureYZInLAr ->Fill(it->GetEndPointZ(), it->GetEndPointY());
    }

    for (size_t itt=0; itt<materials.size(); ++itt) 
      if (it->GetEndMaterial() == materials[itt])
	capTypeE->Fill(itt,  it->GetSumPhotonEnergy());

  }

  for (auto& it: Captures) { delete it; it = NULL; } ; Captures.clear();

  // --- Print everything to the TCanvas
  TCanvas *c = new TCanvas("c", "c");
  gErrorIgnoreLevel = kError;
  std::string outFileDir  = xml.GetOutdir();
  std::string OutFileName = xml.GetSim() + "_CaptureAna";

  const char* path = outFileDir.c_str();
  boost::filesystem::path dir(path);
  if (boost::filesystem::create_directory(dir))
    std::cerr << "Directory Created: " << outFileDir << std::endl;

  c->Print((outFileDir+OutFileName+".pdf[").c_str());

  nCaptureXY->Draw("COLZ");
  nCaptureXY->SetStats(0);
  nCaptureXY->GetXaxis()->SetTitle("X Coordinate [cm]");
  nCaptureXY->GetYaxis()->SetTitle("Y Coordinate [cm]");
  c->Print((outFileDir+OutFileName+".pdf").c_str());
 
  nCaptureYZ->Draw("COLZ");
  nCaptureYZ->SetStats(0);
  nCaptureYZ->GetXaxis()->SetTitle("Z Coordinate [cm]");
  nCaptureYZ->GetYaxis()->SetTitle("Y Coordinate [cm]");
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  nStartXY->Draw("COLZ");
  nStartXY->SetStats(0);
  nStartXY->GetXaxis()->SetTitle("X Coordinate [cm]");
  nStartXY->GetYaxis()->SetTitle("Y Coordinate [cm]");
  c->Print((outFileDir+OutFileName+".pdf").c_str());
 
  nStartYZ->Draw("COLZ");
  nStartYZ->SetStats(0);
  nStartYZ->GetXaxis()->SetTitle("Z Coordinate [cm]");
  nStartYZ->GetYaxis()->SetTitle("Y Coordinate [cm]");
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  nCaptureXYInLAr->Draw("COLZ");
  nCaptureXYInLAr->SetStats(0);
  nCaptureXYInLAr->GetXaxis()->SetTitle("X Coordinate [cm]");
  nCaptureXYInLAr->GetYaxis()->SetTitle("Y Coordinate [cm]");
  c->Print((outFileDir+OutFileName+".pdf").c_str());
 
  nCaptureYZInLAr->Draw("COLZ");
  nCaptureYZInLAr->SetStats(0);
  nCaptureYZInLAr->GetXaxis()->SetTitle("Z Coordinate [cm]");
  nCaptureYZInLAr->GetYaxis()->SetTitle("Y Coordinate [cm]");
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  neutronGenE->Draw();
  neutronGenE->SetStats(0);
  neutronGenE->GetXaxis()->SetTitle("Kinetic Energy [MeV]");
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  nGamma->Draw();
  nGamma->SetStats(0);
  nGamma->GetXaxis()->SetTitle("Number of Photons");
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  gammaCapE->Draw();
  gammaCapE->SetStats(0);
  gammaCapE->GetXaxis()->SetTitle("Sum Photon Energy [MeV]");
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  c->SetLogy(1);
  endMaterial->Draw();
  endMaterial->SetStats(0);
  endMaterial->GetXaxis()->SetLabelSize(0.04);
  endMaterial->SetBit(TAxis::kLabelsHori);
  for (size_t it=0; it<materials.size(); ++it)
    endMaterial->GetXaxis()->SetBinLabel(it+1, materials[it].c_str());
  c->Print((outFileDir+OutFileName+".pdf").c_str());
  
  c->SetLogy(0);
  capTypeE->Draw();
  capTypeE->SetStats(0);
  capTypeE->GetYaxis()->SetTitle("Sum Photon Energy [MeV]");
  capTypeE->SetBit(TAxis::kLabelsHori);
  capTypeE->GetXaxis()->SetLabelSize(0.04);
  for (size_t it=0; it<materials.size(); ++it)
    capTypeE->GetXaxis()->SetBinLabel(it+1, materials[it].c_str());
  c->Print((outFileDir+OutFileName+".pdf").c_str());
  
  nNeutonsScatter->Draw();
  c->Print((outFileDir+OutFileName+".pdf").c_str());

  int i = 0;
  const std::vector<int> color = getColors();
  TLegend* legend = new TLegend(0.1, 0.1, 0.9, 0.9);
  for (auto const& it: nTravelInMatter) {
    nTravelInMatter[it.first]->SetStats(0);
    nTravelInMatter[it.first]->SetMarkerColor(color.at(i));
    nTravelInMatter[it.first]->SetLineColor  (color.at(i));
    legend->AddEntry(nTravelInMatter[it.first], materials[it.first].c_str(), "L");
    i++;
  }

  c->SetLogy(1);
  PlotAll(nTravelInMatter);
  c->Print((outFileDir+OutFileName+".pdf" ).c_str());

  c->Clear();
  legend->Draw();
  c->Print((outFileDir+OutFileName+".pdf" ).c_str());
  c->Print((outFileDir+OutFileName+".pdf]").c_str());

  // std::cout << std::endl;
  // std::cout << "Neutron capture statistics" << std::endl;
  // std::cout << "-----------------------------------------" << std::endl;
  // std::cout << std::endl;
  // std::cout << "Number of primary neutrons:        \t" << nPrimaryNeutrons << std::endl; 
  // std::cout << "Number of captures on LAr:         \t" << nCapturesOnLAr   << std::endl;
  // std::cout << "Radiological neutron capture rate: \t" << (double)nCapturesOnLAr / ((double)fNEvent * 2 * 2.246e-3) 
  // 	    << " [Hz]" << std::endl;
  // std::cout << std::endl;

  std::ofstream file;
  file.open((outFileDir+OutFileName+".txt").c_str());
  file << "Neutron capture statistics" << std::endl;
  file << "-----------------------------------------" << std::endl;
  file << std::endl;
  file << "Number of primary neutrons:        \t" << nPrimaryNeutrons << std::endl; 
  file << "Number of captures on LAr:         \t" << nCapturesOnLAr   << std::endl;
  file << "Radiological neutron capture rate: \t" << (double)nCapturesOnLAr / ((double)fNEvent * 2 * 2.246e-3) 
       << " [Hz]" << std::endl;
  file.close();
  return 0;
}
