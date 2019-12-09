#include <iostream>

class Hit {
private:
  int    fView; 
  int    fSize; 
  int    fTPC ;
  int    fChan; 
  double fTime; 
  int    fRMS ;
  int    fSADC;

public:
  Hit(int    cView,
      int    cSize,
      int    cTPC ,
      int    cChan,
      double cTime,
      int    cRMS ,
      int    cSADC) {
      fView = cView,
      fSize = cSize,
      fTPC  = cTPC ,
      fChan = cChan,
      fTime = cTime,
      fRMS  = cRMS ,
      fSADC = cSADC; }

  ~Hit() {}

  int    GetView () { return fView; }
  int    GetSize () { return fSize; }
  int    GetTPC  () { return fTPC ; }
  int    GetChan () { return fChan; }
  double GetTime () { return fTime; }
  int    GetRMS  () { return fRMS ; }
  int    GetSADC () { return fSADC; }

};
