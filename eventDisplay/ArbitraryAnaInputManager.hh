#ifndef ARBITRARYANAINPUTMANAGER_HH
#define ARBITRARYANAINPUTMANAGER_HH

#include "InputManager.hh"
#include "TTree.h"

using std::string;

class ArbitraryAnaInputManager : public InputManager {

public:
  int Run   ;
  int SubRun;
  int Event ;

  int NTotHit ;
  int NColHit ;
  int NIndHit ;
  int NHitNoBT;

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
  
  std::vector<bool>   * Hit_True_GenType ; 
  std::vector<int>    * Hit_True_TrackID ;
  std::vector<int>    * Hit_True_GenIndex;
  std::vector<double> * Hit_True_Energy	 ;
  std::vector<double> * Hit_True_nElec	 ;
  std::vector<int>    * Hit_True_nIDEs   ;

  std::vector<int>    * PDS_OpHit_OpChannel  ;
  std::vector<double> * PDS_OpHit_X	     ;
  std::vector<double> * PDS_OpHit_Y	     ;
  std::vector<double> * PDS_OpHit_Z	     ;
  std::vector<double> * PDS_OpHit_PeakTimeAbs;
  std::vector<double> * PDS_OpHit_PeakTime   ;
  std::vector<int>    * PDS_OpHit_Frame	     ;
  std::vector<double> * PDS_OpHit_Width	     ;
  std::vector<double> * PDS_OpHit_Area	     ;
  std::vector<double> * PDS_OpHit_Amplitude  ;
  std::vector<double> * PDS_OpHit_PE	     ;
  std::vector<double> * PDS_OpHit_FastToTotal;

  std::vector<bool>   * PDS_OpHit_True_GenType ;
  std::vector<int>    * PDS_OpHit_True_TrackID ;
  std::vector<int>    * PDS_OpHit_True_GenIndex;
  std::vector<double> * PDS_OpHit_True_Energy  ;
  std::vector<double> * PDS_OpHit_True_PE      ;

  std::vector<int>    * True_PDG    ; 
  std::vector<int>    * True_ID	    ;
  std::vector<double> * True_E	    ;
  std::vector<double> * True_VertX  ;
  std::vector<double> * True_VertY  ;
  std::vector<double> * True_VertZ  ;
  std::vector<double> * True_VertexT;
  std::vector<double> * True_Px	    ;
  std::vector<double> * True_Py	    ;
  std::vector<double> * True_Pz	    ;
  std::vector<double> * True_Dirx   ;
  std::vector<double> * True_Diry   ;
  std::vector<double> * True_Dirz   ;
  std::vector<double> * True_Time   ;

  std::vector<int   > * True_Geant4_TruthIndex   ;
  std::vector<string> * True_Geant4_StartMaterial;
  std::vector<string> * True_Geant4_Process      ;
  std::vector<double> * True_Geant4_VertX        ;
  std::vector<double> * True_Geant4_VertY        ;
  std::vector<double> * True_Geant4_VertZ        ;
  std::vector<double> * True_Geant4_Time         ;
  std::vector<double> * True_Geant4_Energy       ;
  std::vector<string> * True_Geant4_EndMaterial  ;
  std::vector<string> * True_Geant4_EndProcess   ;
  std::vector<double> * True_Geant4_EndX         ;
  std::vector<double> * True_Geant4_EndY         ;
  std::vector<double> * True_Geant4_EndZ         ;
  std::vector<double> * True_Geant4_EndTime      ;
  std::vector<double> * True_Geant4_EndEnergy    ;
  std::vector<int   > * True_Geant4_PDG          ;
  std::vector<int   > * True_Geant4_ID           ;
  std::vector<int   > * True_Geant4_Mother       ;

  ArbitraryAnaInputManager():
    Run         (0),  
    SubRun      (0),
    Event       (0),
    NTotHit     (0),
    NColHit     (0),
    NIndHit     (0),
    NHitNoBT    (0),

    Hit_View                     (NULL),
    Hit_Size                     (NULL),
    Hit_TPC	                 (NULL),    
    Hit_Chan                     (NULL),
    Hit_LocalType                (NULL),
    Hit_X_start                  (NULL),
    Hit_Y_start                  (NULL),
    Hit_Z_start                  (NULL),
    Hit_X_end                    (NULL),
    Hit_Y_end                    (NULL),
    Hit_Z_end                    (NULL),
    Hit_Time                     (NULL),
    Hit_RMS	                 (NULL),    
    Hit_SADC                     (NULL),
    Hit_Int	                 (NULL),    
    Hit_Peak                     (NULL),

    Hit_True_GenType             (NULL),
    Hit_True_TrackID             (NULL),
    Hit_True_GenIndex            (NULL),
    Hit_True_Energy              (NULL),	 
    Hit_True_nElec               (NULL),	 
    Hit_True_nIDEs               (NULL),

    PDS_OpHit_OpChannel          (NULL),
    PDS_OpHit_X	     	         (NULL),
    PDS_OpHit_Y	     	         (NULL),
    PDS_OpHit_Z	     	         (NULL),
    PDS_OpHit_PeakTimeAbs        (NULL),
    PDS_OpHit_PeakTime           (NULL),
    PDS_OpHit_Frame	         (NULL),
    PDS_OpHit_Width	         (NULL),
    PDS_OpHit_Area	         (NULL),
    PDS_OpHit_Amplitude          (NULL),
    PDS_OpHit_PE	         (NULL),
    PDS_OpHit_FastToTotal        (NULL),
                              
    PDS_OpHit_True_GenType       (NULL),
    PDS_OpHit_True_TrackID       (NULL),  
    PDS_OpHit_True_GenIndex      (NULL),  
    PDS_OpHit_True_Energy        (NULL),  
    PDS_OpHit_True_PE            (NULL),  
                              
    True_PDG     	         (NULL),	  
    True_ID	    	         (NULL),	  
    True_E	    	         (NULL),	  
    True_VertX  	         (NULL),	  
    True_VertY  	         (NULL),	  
    True_VertZ  	         (NULL),	  
    True_VertexT	         (NULL),	  
    True_Px	    	         (NULL),	  
    True_Py	    	         (NULL),	  
    True_Pz	    	         (NULL),	  
    True_Dirx   	         (NULL),	  
    True_Diry   	         (NULL),	  
    True_Dirz   	         (NULL),	  
    True_Time   	         (NULL),	  
                              
    True_Geant4_TruthIndex       (NULL),
    True_Geant4_StartMaterial    (NULL),
    True_Geant4_Process          (NULL),
    True_Geant4_VertX            (NULL),
    True_Geant4_VertY            (NULL),
    True_Geant4_VertZ            (NULL),
    True_Geant4_Time             (NULL),
    True_Geant4_Energy           (NULL),
    True_Geant4_EndMaterial      (NULL),
    True_Geant4_EndProcess       (NULL),
    True_Geant4_EndX             (NULL),
    True_Geant4_EndY             (NULL),
    True_Geant4_EndZ             (NULL),
    True_Geant4_EndTime          (NULL),
    True_Geant4_EndEnergy        (NULL),
    True_Geant4_PDG              (NULL),
    True_Geant4_ID               (NULL),
    True_Geant4_Mother           (NULL) {};

  ~ArbitraryAnaInputManager() {
    delete Hit_View                     ; Hit_View                     = NULL;
    delete Hit_Size                     ; Hit_Size                     = NULL;
    delete Hit_TPC	                ; Hit_TPC	               = NULL;     
    delete Hit_Chan                     ; Hit_Chan                     = NULL;
    delete Hit_LocalType                ; Hit_LocalType                = NULL;
    delete Hit_X_start                  ; Hit_X_start                  = NULL;
    delete Hit_Y_start                  ; Hit_Y_start                  = NULL;
    delete Hit_Z_start                  ; Hit_Z_start                  = NULL;
    delete Hit_X_end                    ; Hit_X_end                    = NULL;
    delete Hit_Y_end                    ; Hit_Y_end                    = NULL;
    delete Hit_Z_end                    ; Hit_Z_end                    = NULL;
    delete Hit_Time                     ; Hit_Time                     = NULL;
    delete Hit_RMS	                ; Hit_RMS	               = NULL;     
    delete Hit_SADC                     ; Hit_SADC                     = NULL;
    delete Hit_Int	                ; Hit_Int	               = NULL;     
    delete Hit_Peak                     ; Hit_Peak                     = NULL;

    delete Hit_True_GenType             ; Hit_True_GenType             = NULL;
    delete Hit_True_TrackID             ; Hit_True_TrackID             = NULL;
    delete Hit_True_GenIndex            ; Hit_True_GenIndex            = NULL;
    delete Hit_True_Energy              ; Hit_True_Energy              = NULL;	 
    delete Hit_True_nElec               ; Hit_True_nElec               = NULL;	 
    delete Hit_True_nIDEs               ; Hit_True_nIDEs               = NULL;

    delete PDS_OpHit_OpChannel          ; PDS_OpHit_OpChannel          = NULL;
    delete PDS_OpHit_X	     	        ; PDS_OpHit_X	     	       = NULL; 
    delete PDS_OpHit_Y	     	        ; PDS_OpHit_Y	     	       = NULL; 
    delete PDS_OpHit_Z	     	        ; PDS_OpHit_Z	     	       = NULL; 
    delete PDS_OpHit_PeakTimeAbs        ; PDS_OpHit_PeakTimeAbs        = NULL;
    delete PDS_OpHit_PeakTime           ; PDS_OpHit_PeakTime           = NULL;
    delete PDS_OpHit_Frame	        ; PDS_OpHit_Frame	       = NULL; 
    delete PDS_OpHit_Width	        ; PDS_OpHit_Width	       = NULL; 
    delete PDS_OpHit_Area	        ; PDS_OpHit_Area	       = NULL; 
    delete PDS_OpHit_Amplitude          ; PDS_OpHit_Amplitude          = NULL;
    delete PDS_OpHit_PE	                ; PDS_OpHit_PE	               = NULL; 
    delete PDS_OpHit_FastToTotal        ; PDS_OpHit_FastToTotal        = NULL;

    delete PDS_OpHit_True_GenType       ; PDS_OpHit_True_GenType       = NULL;
    delete PDS_OpHit_True_TrackID       ; PDS_OpHit_True_TrackID       = NULL;  
    delete PDS_OpHit_True_GenIndex      ; PDS_OpHit_True_GenIndex      = NULL;  
    delete PDS_OpHit_True_Energy        ; PDS_OpHit_True_Energy        = NULL;  
    delete PDS_OpHit_True_PE            ; PDS_OpHit_True_PE            = NULL;  

    delete True_PDG     	        ; True_PDG     	               = NULL;	  
    delete True_ID	    	        ; True_ID	    	       = NULL; 	  
    delete True_E	    	        ; True_E	    	       = NULL; 	  
    delete True_VertX  	                ; True_VertX  	               = NULL; 	  
    delete True_VertY  	                ; True_VertY  	               = NULL; 	  
    delete True_VertZ  	                ; True_VertZ  	               = NULL; 	  
    delete True_VertexT	                ; True_VertexT	               = NULL; 	  
    delete True_Px	    	        ; True_Px	    	       = NULL; 	  
    delete True_Py	    	        ; True_Py	    	       = NULL; 	  
    delete True_Pz	    	        ; True_Pz	    	       = NULL; 	  
    delete True_Dirx   	                ; True_Dirx   	               = NULL; 	  
    delete True_Diry   	                ; True_Diry   	               = NULL; 	  
    delete True_Dirz   	                ; True_Dirz   	               = NULL; 	  
    delete True_Time   	                ; True_Time   	               = NULL; 	  

    delete True_Geant4_TruthIndex       ; True_Geant4_TruthIndex       = NULL;
    delete True_Geant4_StartMaterial    ; True_Geant4_StartMaterial    = NULL;
    delete True_Geant4_Process          ; True_Geant4_Process          = NULL;
    delete True_Geant4_VertX            ; True_Geant4_VertX            = NULL;
    delete True_Geant4_VertY            ; True_Geant4_VertY            = NULL;
    delete True_Geant4_VertZ            ; True_Geant4_VertZ            = NULL;
    delete True_Geant4_Time             ; True_Geant4_Time             = NULL;
    delete True_Geant4_Energy           ; True_Geant4_Energy           = NULL;
    delete True_Geant4_EndMaterial      ; True_Geant4_EndMaterial      = NULL;
    delete True_Geant4_EndProcess       ; True_Geant4_EndProcess       = NULL;
    delete True_Geant4_EndX             ; True_Geant4_EndX             = NULL;
    delete True_Geant4_EndY             ; True_Geant4_EndY             = NULL;
    delete True_Geant4_EndZ             ; True_Geant4_EndZ             = NULL;
    delete True_Geant4_EndTime          ; True_Geant4_EndTime          = NULL;
    delete True_Geant4_EndEnergy        ; True_Geant4_EndEnergy        = NULL;
    delete True_Geant4_PDG              ; True_Geant4_PDG              = NULL;
    delete True_Geant4_ID               ; True_Geant4_ID               = NULL;
    delete True_Geant4_Mother           ; True_Geant4_Mother           = NULL; 
    if (f_Input) f_Input->Close();
  };

  void LoadTree() {
    Initialise();

    t_Input->SetBranchAddress("Run"      , &Run      );
    t_Input->SetBranchAddress("SubRun"   , &SubRun   );
    t_Input->SetBranchAddress("Event"    , &Event    );
    t_Input->SetBranchAddress("NTotHit"  , &NTotHit  );
    t_Input->SetBranchAddress("NColHit"  , &NColHit  );
    t_Input->SetBranchAddress("NIndHit"  , &NIndHit  );
    t_Input->SetBranchAddress("NHitNoBT" , &NHitNoBT );

    t_Input->SetBranchAddress("Hit_View"                     , &Hit_View                     );
    t_Input->SetBranchAddress("Hit_Size"		     , &Hit_Size                     );
    t_Input->SetBranchAddress("Hit_TPC"			     , &Hit_TPC	               	     );
    t_Input->SetBranchAddress("Hit_Chan"		     , &Hit_Chan                     );
    t_Input->SetBranchAddress("Hit_LocalType"		     , &Hit_LocalType                );
    t_Input->SetBranchAddress("Hit_X_start"		     , &Hit_X_start                  );
    t_Input->SetBranchAddress("Hit_Y_start"		     , &Hit_Y_start                  );
    t_Input->SetBranchAddress("Hit_Z_start"		     , &Hit_Z_start                  );
    t_Input->SetBranchAddress("Hit_X_end"		     , &Hit_X_end                    );
    t_Input->SetBranchAddress("Hit_Y_end"		     , &Hit_Y_end                    );
    t_Input->SetBranchAddress("Hit_Z_end"		     , &Hit_Z_end                    );
    t_Input->SetBranchAddress("Hit_Time"		     , &Hit_Time                     );
    t_Input->SetBranchAddress("Hit_RMS"			     , &Hit_RMS	               	     );
    t_Input->SetBranchAddress("Hit_SADC"		     , &Hit_SADC                     );
    t_Input->SetBranchAddress("Hit_Int"			     , &Hit_Int	               	     );
    t_Input->SetBranchAddress("Hit_Peak"		     , &Hit_Peak                     );

    t_Input->SetBranchAddress("Hit_True_GenType"	     , &Hit_True_GenType             );
    t_Input->SetBranchAddress("Hit_True_TrackID"	     , &Hit_True_TrackID             );
    t_Input->SetBranchAddress("Hit_True_GenIndex"	     , &Hit_True_GenIndex            );
    t_Input->SetBranchAddress("Hit_True_Energy"		     , &Hit_True_Energy              );
    t_Input->SetBranchAddress("Hit_True_nElec"		     , &Hit_True_nElec               );
    t_Input->SetBranchAddress("Hit_True_nIDEs"		     , &Hit_True_nIDEs               );              

    t_Input->SetBranchAddress("PDS_OpHit_OpChannel"	     , &PDS_OpHit_OpChannel          );
    t_Input->SetBranchAddress("PDS_OpHit_X"		     , &PDS_OpHit_X	     	     );  
    t_Input->SetBranchAddress("PDS_OpHit_Y"		     , &PDS_OpHit_Y	     	     );  
    t_Input->SetBranchAddress("PDS_OpHit_Z"		     , &PDS_OpHit_Z	     	     );  
    t_Input->SetBranchAddress("PDS_OpHit_PeakTimeAbs"	     , &PDS_OpHit_PeakTimeAbs        );
    t_Input->SetBranchAddress("PDS_OpHit_PeakTime"	     , &PDS_OpHit_PeakTime           );
    t_Input->SetBranchAddress("PDS_OpHit_Frame"		     , &PDS_OpHit_Frame	       	     );
    t_Input->SetBranchAddress("PDS_OpHit_Width"		     , &PDS_OpHit_Width	       	     );
    t_Input->SetBranchAddress("PDS_OpHit_Area"		     , &PDS_OpHit_Area	       	     );
    t_Input->SetBranchAddress("PDS_OpHit_Amplitude"	     , &PDS_OpHit_Amplitude          );
    t_Input->SetBranchAddress("PDS_OpHit_PE"		     , &PDS_OpHit_PE	             );  
    t_Input->SetBranchAddress("PDS_OpHit_FastToTotal"	     , &PDS_OpHit_FastToTotal        );

    t_Input->SetBranchAddress("PDS_OpHit_True_GenType"	     , &PDS_OpHit_True_GenType       );
    t_Input->SetBranchAddress("PDS_OpHit_True_TrackID"	     , &PDS_OpHit_True_TrackID       );
    t_Input->SetBranchAddress("PDS_OpHit_True_GenIndex"	     , &PDS_OpHit_True_GenIndex      );
    t_Input->SetBranchAddress("PDS_OpHit_True_Energy"	     , &PDS_OpHit_True_Energy        );
    t_Input->SetBranchAddress("PDS_OpHit_True_PE"	     , &PDS_OpHit_True_PE            );

    t_Input->SetBranchAddress("True_PDG"		     , &True_PDG     	             );  
    t_Input->SetBranchAddress("True_ID"			     , &True_ID	    	       	     );
    t_Input->SetBranchAddress("True_E"			     , &True_E	    	       	     );
    t_Input->SetBranchAddress("True_VertX"		     , &True_VertX  	             );  
    t_Input->SetBranchAddress("True_VertY"		     , &True_VertY  	             );  
    t_Input->SetBranchAddress("True_VertZ"		     , &True_VertZ  	             );  
    t_Input->SetBranchAddress("True_VertexT"		     , &True_VertexT	             );  
    t_Input->SetBranchAddress("True_Px"			     , &True_Px	    	       	     );
    t_Input->SetBranchAddress("True_Py"			     , &True_Py	    	       	     );
    t_Input->SetBranchAddress("True_Pz"			     , &True_Pz	    	       	     );
    t_Input->SetBranchAddress("True_Dirx"		     , &True_Dirx   	             );  
    t_Input->SetBranchAddress("True_Diry"		     , &True_Diry   	             );  
    t_Input->SetBranchAddress("True_Dirz"		     , &True_Dirz   	             );  
    t_Input->SetBranchAddress("True_Time"		     , &True_Time   	             );  

    t_Input->SetBranchAddress("True_Geant4_TruthIndex"	     , &True_Geant4_TruthIndex       );
    t_Input->SetBranchAddress("True_Geant4_StartMaterial"    , &True_Geant4_StartMaterial    );
    t_Input->SetBranchAddress("True_Geant4_Process"	     , &True_Geant4_Process          );
    t_Input->SetBranchAddress("True_Geant4_VertX"	     , &True_Geant4_VertX            );
    t_Input->SetBranchAddress("True_Geant4_VertY"	     , &True_Geant4_VertY            );
    t_Input->SetBranchAddress("True_Geant4_VertZ"	     , &True_Geant4_VertZ            );
    t_Input->SetBranchAddress("True_Geant4_Time"	     , &True_Geant4_Time             );
    t_Input->SetBranchAddress("True_Geant4_Energy"	     , &True_Geant4_Energy           );
    t_Input->SetBranchAddress("True_Geant4_EndMaterial"	     , &True_Geant4_EndMaterial      );
    t_Input->SetBranchAddress("True_Geant4_EndProcess"	     , &True_Geant4_EndProcess       );
    t_Input->SetBranchAddress("True_Geant4_EndX"	     , &True_Geant4_EndX             );
    t_Input->SetBranchAddress("True_Geant4_EndY"	     , &True_Geant4_EndY             );
    t_Input->SetBranchAddress("True_Geant4_EndZ"	     , &True_Geant4_EndZ             );
    t_Input->SetBranchAddress("True_Geant4_EndTime"	     , &True_Geant4_EndTime          );
    t_Input->SetBranchAddress("True_Geant4_EndEnergy"	     , &True_Geant4_EndEnergy        );
    t_Input->SetBranchAddress("True_Geant4_PDG"		     , &True_Geant4_PDG              );
    t_Input->SetBranchAddress("True_Geant4_ID"		     , &True_Geant4_ID               );
    t_Input->SetBranchAddress("True_Geant4_Mother"	     , &True_Geant4_Mother           );

  };

};

#endif
