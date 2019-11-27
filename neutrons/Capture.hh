#ifndef CAPTURE_HH
#define CAPTURE_HH

class Capture {
private:
  std::string         fCaptureMaterial;
  double              fCapturePointX;  
  double              fCapturePointY;  
  double              fCapturePointZ;  
  double              fParticleStartE;
  double              fParticleEndE; 
  std::vector<double> fPhotonEnergies;

public:
  Capture(std::string         cCaptureMaterial, 
	  double              cCapturePointX, 
	  double              cCapturePointY, 
	  double              cCapturePointZ,
	  double              cParticleStartE, 
	  double              cParticleEndE, 
	  std::vector<double> cPhotonEnergies) {
    fCaptureMaterial = cCaptureMaterial,
      fCapturePointX   = cCapturePointX,
      fCapturePointY   = cCapturePointY,
      fCapturePointZ   = cCapturePointZ,
      fParticleStartE  = cParticleStartE,
      fParticleEndE    = cParticleEndE,
      fPhotonEnergies  = cPhotonEnergies; 
  }
  
  ~Capture() { };

  std::string         GetEndMaterial    ();
  double              GetEndPointX      ();
  double              GetEndPointY      ();
  double              GetEndPointZ      ();
  double              GetCaptureStartE  ();
  double              GetCaptureEndE    ();
  int                 GetNPhotons       ();
  double              GetSumPhotonEnergy();
  std::vector<double> GetPhotons        ();

};

std::string         Capture::GetEndMaterial    () { return fCaptureMaterial;       }
double              Capture::GetEndPointX      () { return fCapturePointX;         }
double              Capture::GetEndPointY      () { return fCapturePointY;         }
double              Capture::GetEndPointZ      () { return fCapturePointZ;         }
double              Capture::GetCaptureStartE  () { return fParticleStartE;        }
double              Capture::GetCaptureEndE    () { return fParticleEndE;          }
int                 Capture::GetNPhotons       () { return fPhotonEnergies.size(); }
double              Capture::GetSumPhotonEnergy() {
  double sumPhotonEnergy = 0;
  for (auto const& it: fPhotonEnergies)
    sumPhotonEnergy += it*1000;
  return sumPhotonEnergy;
}


std::vector<double> Capture::GetPhotons        () { return fPhotonEnergies;        }


#endif
