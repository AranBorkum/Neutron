#include <iostream>
#include <string>
#include <vector>

class Neutron {
private:
  int                      fID;
  int                      fEndProcess;
  int                      fSourceWall;
  std::string              fEndMaterial;
  std::vector<int>         fDaughterIDs;
  std::vector<int>         fDaughterEndProcess;
  std::vector<std::string> fDaughterEndMaterial;

public:
  Neutron(int                      cID,
	  int                      cEndProcess,
	  int                      cSourceWall,
	  std::string              cEndMaterial,
	  std::vector<int>         cDaughterIDs,
	  std::vector<int>         cDaughterEndProcess,
	  std::vector<std::string> cDaughterEndMaterial) {
    fID			 = cID,			
    fEndProcess		 = cEndProcess,
    fSourceWall          = cSourceWall,
    fEndMaterial	 = cEndMaterial,	
    fDaughterIDs	 = cDaughterIDs,	
    fDaughterEndProcess	 = cDaughterEndProcess,	
    fDaughterEndMaterial = cDaughterEndMaterial;
  }

  ~Neutron() { }

  int                      GetID		 (){ return fID;			};
  int                      GetEndProcess	 (){ return fEndProcess;		};
  int                      GetSourceWall	 (){ return fSourceWall;		};
  std::string              GetEndMaterial	 (){ return fEndMaterial;		};
  std::vector<int>         GetDaughterIDs	 (){ return fDaughterIDs;		};
  std::vector<int>         GetDaughterEndProcess (){ return fDaughterEndProcess;	};
  std::vector<std::string> GetDaughterEndMaterial(){ return fDaughterEndMaterial;	};

};
