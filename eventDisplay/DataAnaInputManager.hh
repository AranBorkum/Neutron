#ifndef DATAANAINPUTMANAGER_HH
#define DATAANAINPUTMANAGER_HH

#include "InputManager.hh"
#include "TTree.h"

using std::string;

class DataAnaInputManager : public InputManager {

public:
  
  int Run   ;
  int SubRun;
  int Event ;

  int NTotHit;
  int NColHit;
  int NIndHit;

  std::vector<int>    * Hit_View     ;  
  std::vector<int>    * Hit_Size     ;
  std::vector<int>    * Hit_TPC	     ;
  std::vector<int>    * Hit_Chan     ;
  std::vector<int>    * Hit_LocalType;
  std::vector<double> * Hit_X_start  ;
  std::vector<double> * Hit_Y_start  ;
  std::vector<double> * Hit_Z_start  ;
  std::vector<double> * Hit_X_end    ;
  std::vector<double> * Hit_Y_end    ;
  std::vector<double> * Hit_Z_end    ;
  std::vector<double> * Hit_Time     ;
  std::vector<double> * Hit_RMS	     ;
  std::vector<double> * Hit_SADC     ;
  std::vector<double> * Hit_Int	     ;
  std::vector<double> * Hit_Peak     ;

  DataAnaInputManager():
    Run       (0),
    SubRun    (0),
    Event     (0),
    NTotHit   (0),
    NColHit   (0),
    NIndHit   (0),

    Hit_View         (NULL),
    Hit_Size         (NULL),
    Hit_TPC	     (NULL),    
    Hit_Chan         (NULL),
    Hit_LocalType    (NULL),
    Hit_X_start      (NULL),
    Hit_Y_start      (NULL),
    Hit_Z_start      (NULL),
    Hit_X_end        (NULL),
    Hit_Y_end        (NULL),
    Hit_Z_end        (NULL),
    Hit_Time         (NULL),
    Hit_RMS	     (NULL),    
    Hit_SADC         (NULL),
    Hit_Int	     (NULL),    
    Hit_Peak         (NULL) {};

  ~DataAnaInputManager() {

    delete Hit_View          ; Hit_View         = NULL;
    delete Hit_Size          ; Hit_Size         = NULL;
    delete Hit_TPC	     ; Hit_TPC	        = NULL;    
    delete Hit_Chan          ; Hit_Chan         = NULL;
    delete Hit_LocalType     ; Hit_LocalType    = NULL;
    delete Hit_X_start       ; Hit_X_start      = NULL;
    delete Hit_Y_start       ; Hit_Y_start      = NULL;
    delete Hit_Z_start       ; Hit_Z_start      = NULL;
    delete Hit_X_end         ; Hit_X_end        = NULL;
    delete Hit_Y_end         ; Hit_Y_end        = NULL;
    delete Hit_Z_end         ; Hit_Z_end        = NULL;
    delete Hit_Time          ; Hit_Time         = NULL;
    delete Hit_RMS	     ; Hit_RMS	        = NULL;    
    delete Hit_SADC          ; Hit_SADC         = NULL;
    delete Hit_Int	     ; Hit_Int	        = NULL;    
    delete Hit_Peak          ; Hit_Peak         = NULL;
  };
  
  void LoadTree() {
    Initialise();

    t_Input->SetBranchAddress("Run"          , &Run       );
    t_Input->SetBranchAddress("SubRun"       , &SubRun    );
    t_Input->SetBranchAddress("Event"        , &Event     );

    t_Input->SetBranchAddress("Hit_View"     , &Hit_View     );
    t_Input->SetBranchAddress("Hit_Size"     , &Hit_Size     );
    t_Input->SetBranchAddress("Hit_TPC"	     , &Hit_TPC	     );
    t_Input->SetBranchAddress("Hit_Chan"     , &Hit_Chan     );
    t_Input->SetBranchAddress("Hit_LocalType", &Hit_LocalType);
    t_Input->SetBranchAddress("Hit_X_start"  , &Hit_X_start  );
    t_Input->SetBranchAddress("Hit_Y_start"  , &Hit_Y_start  );
    t_Input->SetBranchAddress("Hit_Z_start"  , &Hit_Z_start  );
    t_Input->SetBranchAddress("Hit_X_end"    , &Hit_X_end    );
    t_Input->SetBranchAddress("Hit_Y_end"    , &Hit_Y_end    );
    t_Input->SetBranchAddress("Hit_Z_end"    , &Hit_Z_end    );
    t_Input->SetBranchAddress("Hit_Time"     , &Hit_Time     );
    t_Input->SetBranchAddress("Hit_RMS"	     , &Hit_RMS	     );
    t_Input->SetBranchAddress("Hit_SADC"     , &Hit_SADC     );
    t_Input->SetBranchAddress("Hit_Int"	     , &Hit_Int	     );
    t_Input->SetBranchAddress("Hit_Peak"     , &Hit_Peak     );
  };
};

#endif
