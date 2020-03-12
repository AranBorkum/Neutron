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
#include <math.h>

// double neutronMass = 939.56541;
double structureDimX[2] = {-836,  836};
double structureDimY[2] = {-737,  826};
double structureDimZ[2] = {-382, 5980};

double cavernDimX[2] = {-990,   990};
double cavernDimY[2] = {-847,  1467};
double cavernDimZ[2] = {-536, 14524};

// double APASpanX[2] = {-733.6944, 733.6944};
// double APASpanY[2] = {-606.0   , 606.0   };
// double APASpanZ[2] = {-443.8   , 5789.8  };

std::vector<std::string> names = {"Left"  ,
				  "Right" ,
				  "Front" ,
				  "Back"  ,
				  "Floor" ,
				  "Ceiling"};

double GetSurfaceArea(int simType) {
  // --- Get the legths of each slab
  double TotalSurfaceArea = 0;
  
  double archLen      = 0; double archSfA      = 0;
  double xSpan        = 0; double ySpan        = 0; double zSpan        = 0;
  double xSurfaceArea = 0; double ySurfaceArea = 0; double zSurfaceArea = 0;

  if (simType == 0) {
    // --- Get the length of the sides
    xSpan = abs(structureDimX[0]) + abs(structureDimX[1]);
    ySpan = abs(structureDimY[0]) + abs(structureDimY[1]);
    zSpan = abs(structureDimZ[0]) + abs(structureDimZ[1]);
    
    // --- Get the volume of each slab
    xSurfaceArea = ySpan * zSpan;
    ySurfaceArea = xSpan * zSpan;
    zSurfaceArea = xSpan * ySpan;
    
    // --- Get the total surface area
    TotalSurfaceArea = (2.0*xSurfaceArea) + (2.0*ySurfaceArea) + (2.0*zSurfaceArea);
  }

  if (simType == 1) {
    // --- Get the length of the sides
    xSpan = abs(cavernDimX[0]) + abs(cavernDimX[1]);
    ySpan = abs(cavernDimY[0]) + abs(cavernDimY[1]);
    zSpan = abs(cavernDimZ[0]) + abs(cavernDimZ[1]);
    
    // --- Get the lengths of the sides
    xSurfaceArea = ySpan * zSpan;
    ySurfaceArea = xSpan * zSpan;
    zSurfaceArea = xSpan * ySpan;

    // --- Work out the surface area of the ceiling
    archLen = M_PI * 12.84;
    archSfA = archLen * zSpan;
    
    // --- Get the total surface area
    TotalSurfaceArea = (2.0*xSurfaceArea) + (ySurfaceArea) + (2.0*zSurfaceArea) + (archSfA);
    
  }

  if (simType == 2) {
    double midSlab = 1390.0 * 1200.0;
    double fbSlab  = 350.0  * 1200.0;
    double tbSlab  = 350.0  * 1390.0;
    
    TotalSurfaceArea = midSlab + (2.0*tbSlab) +(2.0*fbSlab);
  }

  return TotalSurfaceArea;
}

int NeutronGenWall(int simType, double x, double y, double z) {
  // Definition of sim type: 
  //  0: legacy
  //  1: rock layer
  //  2: mcc11

  // Key:
  //   0: Left wall
  //   1: Right wall
  //   2: Front wall
  //   3: Back wall
  //   4: Floor
  //   5: Ceiling
  int SourceArea;
  bool inNegXSlab = false;
  bool inNegYSlab = false;
  bool inNegZSlab = false;
  bool inPosXSlab = false;
  bool inPosYSlab = false;
  bool inPosZSlab = false;

  if (simType == 0) {
    inNegXSlab = (x<structureDimX[0]);
    inNegYSlab = (y<structureDimY[0]);
    inNegZSlab = (z<structureDimZ[0]);
    inPosXSlab = (x>structureDimX[1]);
    inPosYSlab = (y>structureDimY[1]);
    inPosZSlab = (z>structureDimZ[1]);
  } // Analyse for legacy

  if (simType == 1) {
    inNegXSlab = (x<cavernDimX[0]);
    inNegYSlab = (y<cavernDimY[0]);
    inNegZSlab = (z<cavernDimZ[0]);
    inPosXSlab = (x>cavernDimX[1]);
    inPosYSlab = (y>cavernDimY[1]);
    inPosZSlab = (z>cavernDimZ[1]);
  } // Analyse for rock layer

  if (simType != 2) {
    if (inNegXSlab) SourceArea = 0;
    if (inPosXSlab) SourceArea = 1;
    if (inNegZSlab && !inNegXSlab && !inPosXSlab && !inNegYSlab && !inPosYSlab) SourceArea = 2;
    if (inPosZSlab && !inNegXSlab && !inPosXSlab && !inNegYSlab && !inPosYSlab) SourceArea = 3;
    if (inNegYSlab && !inNegXSlab && !inPosXSlab && !inNegZSlab && !inPosZSlab) SourceArea = 4;
    if (inPosYSlab && !inNegXSlab && !inPosXSlab && !inNegZSlab && !inPosZSlab) SourceArea = 5;
    
    return SourceArea;
  } else { return 0; }
  
}

int ValidationAna(ParsedXML xml, int nEvent, int SimType) {

  // --- Read in the input TFile and open the TTree
  FullGeoAnaInputManager *im = new FullGeoAnaInputManager();
  im->SetInputFile((xml.GetDirectory() + xml.GetFile()).c_str());
  im->SetInputTree(xml.GetTree());
  im->LoadTree();

  int fNEvent = nEvent;
  if (fNEvent != -1) { fNEvent = std::min(fNEvent, (int)im->GetEntries()); } 
  else               { fNEvent = im->GetEntries();                         }

  // --- Define the plots to be made
  TH1D *NeutronStartPosition = new TH1D("NeutronStartPosition", "", 6, 0, 6);
  TH1D *NeutronCapBackTrack  = new TH1D("NeutronCapBackTrack" , "", 6, 0, 6);

  // --- Define counting quatities to be aware of
  int NeutronsGenerated  = 0;
  int NeutronLArCaptures = 0;
  int NeutronCapturesFid = 0;

  // --- Start the event by event loop
  for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
    im->GetEntry(CurrentEvent);
    
    std::vector<int>  PrimaryNeutrons = {};
    std::vector<int>  ProducerWall    = {};
    std::vector<bool> GetsCaptured    = {};
    for (size_t it=0; it<im->True_Bck_PDG->size(); ++it) {
      bool isNeutron  = (*im->True_Bck_PDG)        [it] ==  2112;
      bool isPrimary  = (*im->True_Bck_Mother)     [it] ==     0;
      bool isCaptured = (*im->True_Bck_EndProcess) [it] ==    10;
      bool isInLAr    = (*im->True_Bck_EndMaterial)[it] == "LAr";
      if (isNeutron && isPrimary) {
	PrimaryNeutrons.push_back((*im->True_Bck_ID)[it]);
	ProducerWall.push_back(NeutronGenWall(SimType, 
					      (*im->True_Bck_VertX)[it],
					      (*im->True_Bck_VertY)[it],
					      (*im->True_Bck_VertZ)[it]));
	GetsCaptured.push_back(0);
      }
      if (isNeutron && !isPrimary) {
	for (int i=0; i<PrimaryNeutrons.size(); ++i) {
	  if ((*im->True_Bck_Mother)[it] == PrimaryNeutrons[i]) {
	    PrimaryNeutrons[i] = (*im->True_Bck_Mother)[it];
	    GetsCaptured   [i] = isCaptured && isInLAr;
	  }
	}
      }
    }
    
    for (int i=0; i<PrimaryNeutrons.size(); ++i) 
      if (GetsCaptured[i])
	NeutronCapBackTrack->Fill(ProducerWall[i]);
    
    for (size_t it=0; it<im->True_Bck_PDG->size(); ++it) {
      bool isNeutron  = (*im->True_Bck_PDG)        [it] ==  2112;
      bool isPrimary  = (*im->True_Bck_Mother)     [it] ==     0;
      bool isCaptured = (*im->True_Bck_EndProcess) [it] ==    10;
      bool isInLAr    = (*im->True_Bck_EndMaterial)[it] == "LAr";

      if (isNeutron && isPrimary) {
	int Wall = NeutronGenWall(SimType, 
				  (*im->True_Bck_VertX)[it],
				  (*im->True_Bck_VertY)[it],
				  (*im->True_Bck_VertZ)[it]);
	NeutronStartPosition->Fill(Wall);
	NeutronsGenerated += 1;
      }
      
      if (isNeutron && isCaptured && isInLAr) {
	NeutronLArCaptures += 1;
	bool inActiveRegion = isInCryostat((*im->True_Bck_EndX)[it],
					   (*im->True_Bck_EndY)[it],
					   (*im->True_Bck_EndZ)[it]);
	if (inActiveRegion) NeutronCapturesFid += 1;
      }


    } // Looping through truth information
  } // Looping through the events

  // --- Calculate the neutron flux ---
  int    SecondsInYear    = 60 * 60 * 24 * 365.25;                 // [s/year]
  double TotalTime        = (double)fNEvent * 2.246e-3 * 2.0;      // [s]
  double ProductionRate   = (double)NeutronsGenerated / TotalTime; // [neutrons/s]
  double ProdRateYear     = ProductionRate * SecondsInYear;
  double RatePerCCPerYear = ProdRateYear / GetSurfaceArea(SimType);
  double RatePerCCPerSec  = RatePerCCPerYear / SecondsInYear;
    
  // --- Dump all the plots into a PDF
  TCanvas *c = new TCanvas("c", "c");
  gErrorIgnoreLevel = kError;
  std::string outFileDir  = xml.GetOutdir();
  std::string OutFileName = xml.GetSim() + "_ValidationAna";

  const char* path = outFileDir.c_str();
  boost::filesystem::path dir(path);
  if (boost::filesystem::create_directory(dir))
    std::cerr << "Directory Created: " << outFileDir << std::endl;
  c->Print((outFileDir+OutFileName + ".pdf[").c_str());

  // Neutron start position 
  NeutronStartPosition->SetStats(0);
  NeutronStartPosition->SetTitle("Generated neutron origin wall");
  NeutronStartPosition->GetYaxis()->SetTitle("Number of neutrons");
  NeutronStartPosition->SetBit(TAxis::kLabelsHori);
  NeutronStartPosition->GetXaxis()->SetLabelSize(0.04);
  for (size_t it=0; it<names.size(); ++it)
    NeutronStartPosition->GetXaxis()->SetBinLabel(it+1, names[it].c_str());
  NeutronStartPosition->Draw();
  c->Print((outFileDir+OutFileName + ".pdf") .c_str());

  NeutronCapBackTrack->SetStats(0);
  NeutronCapBackTrack->SetTitle("Captured neutron backtracked origin wall");
  NeutronCapBackTrack->GetYaxis()->SetTitle("Number of neutrons");
  NeutronCapBackTrack->SetBit(TAxis::kLabelsHori);
  NeutronCapBackTrack->GetXaxis()->SetLabelSize(0.04);
  for (size_t it=0; it<names.size(); ++it)
    NeutronCapBackTrack->GetXaxis()->SetBinLabel(it+1, names[it].c_str());
  NeutronCapBackTrack->Draw();
  c->Print((outFileDir+OutFileName + ".pdf") .c_str());
  c->Print((outFileDir+OutFileName + ".pdf]").c_str());

  std::ofstream file;
  file.open((outFileDir+OutFileName+".txt").c_str());
  double CaptureRate    = (double)NeutronLArCaptures / ((double)fNEvent * 2.246e-3 * 2.0);
  double CaptureRateFid = (double)NeutronCapturesFid / ((double)fNEvent * 2.246e-3 * 2.0);
  file << "Number of neutrons generated:      " << NeutronsGenerated		<< std::endl;
  file << "Total simulation time:             " << TotalTime << " [s]"		<< std::endl;
  file << "Neutron production rate:           " << ProductionRate << " [Hz]"	<< std::endl;
  file << "Neutron production rate:           " << ProdRateYear <<  " [n/y]"	<< std::endl;
  file << "Neutron flux from rock:            " << RatePerCCPerYear << " [n/y/cc]" << std::endl;
  file << "Neutron flux from rock:            " << RatePerCCPerSec << " [n/s/cc]"  << std::endl;
  file <<                                                                             std::endl;
  file << "Results calculated from GEANT information"				<< std::endl;
  file << "Neutron Capture Rate on LAr :      " << CaptureRate    << " [Hz]"	<< std::endl;
  file << "Neutron Capture Rate fiducialised: " << CaptureRateFid << " [Hz]"	<< std::endl;
  file.close();
  return 0;
}
