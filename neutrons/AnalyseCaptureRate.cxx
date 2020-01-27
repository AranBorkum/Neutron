#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"

#include "Helper.h"
#include "InputManager.hh"
#include "Capture.hh"

#include <fstream>
#include <iostream>
#include <unistd.h>

double neutronMass = 939.56541;
double cryoDimX[2] = {-947,  947};
double cryoDimY[2] = {-887,  887};
double cryoDimZ[2] = {-493, 6091};

std::string outFileDir = "/dune/app/users/aborkum/Neutrons/output/";

bool isInCryostat(double x, double y, double z) {

  bool isWithin  = false;
  bool isWithinX = (x<cryoDimX[1] && x>cryoDimX[0]);
  bool isWithinY = (y<cryoDimY[1] && y>cryoDimY[0]);
  bool isWithinZ = (z<cryoDimZ[1] && z>cryoDimZ[0]);

  if (isWithinX && isWithinY && isWithinZ)
    isWithin = true;
  return isWithin;
}

int main(int argc, char** argv) {

  int opt;
  extern char *optarg;
  extern int   optopt;

  int nEvent              = -1;
  std::string InFileName  = "";
  std::string OutFileName = "";

  while( (opt = getopt(argc, argv, "i:o:n:")) != -1) {
    switch (opt) {
    case 'i':
      InFileName = optarg;
      break;
    case 'o':
      OutFileName = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case '?':
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  // --- Read in the input TFile and open the TTree
  FullGeoAnaInputManager *im = new FullGeoAnaInputManager();
  im->SetInputFile(InFileName);
  im->SetInputTree("FullGeoAna"); /*im->SetInputTree("fullgeoana/FullGeoAna");*/
  im->LoadTree();

  int fNEvent = nEvent;
  if (fNEvent != -1) { fNEvent = std::min(fNEvent, (int)im->GetEntries()); } 
  else               { fNEvent = im->GetEntries();                         }

  int nPrimaryNeutrons   = 0;
  int nSecondaryNeutrons = 0;
  int nCapturesOnLAr     = 0;
  std::vector<std::string> materials;
  std::vector<Capture*>     Captures;

  TH1D     *neutronGenE = new TH1D    ("neutronGenE", "neutronGenE",  200, 0, 10);
  TH1D     *gammaCapE   = new TH1D    ("gammaCapE"  , "gammaCapE"  , 1000, 0, 10);
  TH1D     *endMaterial = new TH1D    ("endMaterial", "endMaterial",   11, 0, 11);
  TH1D     *nGamma      = new TH1D    ("nGamma"     , "nGamma"     ,   15, 0, 15);
  TH2D     *nCaptureXY  = new TH2D    ("nCaptureXY" , "nCaptureXY" ,  500, -1300,  1300,  500, -1500, 3000);
  TH2D     *nCaptureYZ  = new TH2D    ("nCaptureYZ" , "nCaptureYZ" , 8000, -1000,  15000, 2000, -1300, 3000);
  TProfile *capTypeE    = new TProfile("capTypeE"   , "capTypeE"   ,   11,     0,    11,    0,    20);

  // --- Get list of all of the materials in the TTree
  for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
    im->GetEntry(CurrentEvent);

    std::cout << "Filling list of materials present in file" << " ";
    float prog = (float)CurrentEvent / (float)fNEvent;
    progress(prog);

    for (auto const& it: *im->True_Bck_EndMaterial) {
      bool inVector = false;
      for (auto const itt: materials)
	if (it == itt) inVector = true;
      if (!inVector) materials.push_back(it);
    }
  }
  std::cout << std::endl;

  // --- Main Analysis Loop -----------------------------------------------------
  for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
    im->GetEntry(CurrentEvent);

    std::cout << "NOW PROCESSING EVENT " << CurrentEvent << " " << std::flush;
    float prog = (float)CurrentEvent / (float)fNEvent;
    progress(prog);

    std::vector<double> empty;
    std::vector<double> photons;
    for (size_t it=0; it<im->True_Bck_PDG->size(); ++it) {
    
      bool isNeutron  = (*im->True_Bck_PDG)[it]        == 2112;
      bool isCaptured = (*im->True_Bck_EndProcess)[it] ==   10;

      // --- Fill the capture type histogram
      if (isCaptured && isNeutron)
	for (size_t itt=0; itt<materials.size(); ++itt)
	  if ((*im->True_Bck_EndMaterial)[it] == materials[itt])
	    endMaterial->Fill(itt);

      // --- Count the number of primary and secondary neutrons
      if (isNeutron && (*im->True_Bck_Mother)[it] == 0) 
	{ nPrimaryNeutrons++; neutronGenE->Fill((*im->True_Bck_Energy)[it]*1000 - neutronMass); }
      if (isNeutron && (*im->True_Bck_Mother)[it] != 0) 
	{ nSecondaryNeutrons++; }

      // --- Filling a vector of all the photons post capture
      if (isNeutron && isCaptured) 
	for (size_t itt=0; itt<im->True_Bck_Mother->size(); ++itt) 
	  if ((*im->True_Bck_Mother)[itt] == (*im->True_Bck_ID)[it] && (*im->True_Bck_PDG)[itt] == 22) 
	    photons.push_back((*im->True_Bck_Energy)[itt]);

      // --- Count the number of captures on LAr
      if (isNeutron && isCaptured && (*im->True_Bck_EndMaterial)[it] == "LAr")
	nCapturesOnLAr++;
	    
      if (isNeutron && isCaptured) {
	Capture *cap = new Capture((*im->True_Bck_EndMaterial)[it],
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
    
    for (size_t itt=0; itt<materials.size(); ++itt) 
      if (it->GetEndMaterial() == materials[itt])
	capTypeE->Fill(itt,  it->GetSumPhotonEnergy());

  }

  for (auto& it: Captures) { delete it; it = NULL; } ; Captures.clear();

  // --- Print everything to the TCanvas
  TCanvas *c = new TCanvas("c", "c");
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
  c->Print((outFileDir+OutFileName+".pdf]").c_str());

  std::cout << std::endl;
  std::cout << "Neutron capture statistics" << std::endl;
  std::cout << "-----------------------------------------" << std::endl;
  std::cout << std::endl;
  std::cout << "Number of primary neutrons:        \t" << nPrimaryNeutrons << std::endl; 
  std::cout << "Number of captures on LAr:         \t" << nCapturesOnLAr   << std::endl;
  std::cout << "Radiological neutron capture rate: \t" << (double)nCapturesOnLAr / ((double)nPrimaryNeutrons * 2 * 2.246e-3) 
	    << " [Hz]" << std::endl;

  std::ofstream file;
  file.open((outFileDir+OutFileName+".txt").c_str());
  file << "Neutron capture statistics" << std::endl;
  file << "-----------------------------------------" << std::endl;
  file << std::endl;
  file << "Number of primary neutrons:        \t" << nPrimaryNeutrons << std::endl; 
  file << "Number of captures on LAr:         \t" << nCapturesOnLAr   << std::endl;
  file << "Radiological neutron capture rate: \t" << (double)nCapturesOnLAr / ((double)nPrimaryNeutrons * 2 * 2.246e-3) 
       << " [Hz]" << std::endl;
  file.close();

}
