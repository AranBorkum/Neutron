#include <iostream>
#include <vector>
#include <string>

class ProgressTable {

private:
  int fNUpdates = 0;
  std::vector<ParsedXML> fXMLs;

public:

  ProgressTable(std::vector<ParsedXML> cXMLs) {fXMLs = cXMLs;}
  ~ProgressTable() {
    for (int i=0; i<fXMLs.size()+2; ++i) std::cout << std::endl;
    title("Processes Complete");
  }

  void Create();
  void Update();
  std::vector<ParsedXML> GetXMLs() { return fXMLs; };

};

void ProgressTable::Create() {

  std::vector<ParsedXML> xmls = GetXMLs();
  // --- Write the header to the table
  std::cout << std::setw(15) << std::left << "Analyzer"
	    << std::setw(25) << std::left << "Simulation Type" << "Status\r";
  std::cout << "\n-----------------------------------------------------\r";
  
  for (int it=0; it<xmls.size(); ++it) {
    std::cout << "\n";
    std::cout << std::setw(15) << std::left << xmls[it].GetAnalyzer() 
	      << std::setw(25) << std::left << xmls[it].GetSim();
  }

  for (int it=0; it<xmls.size()+2; ++it) { std::cout << "\033[F"; }
  std::cout << std::flush;

}

void ProgressTable::Update() {
  
  std::vector<ParsedXML> xmls = GetXMLs();
  // --- Write the header to the table
  std::cout << std::setw(15) << std::left << "Analyzer"
	    << std::setw(25) << std::left <<" Simulation Type" << " Status\r";
  std::cout << "\n-----------------------------------------------------\r";
  
  for (int it=0; it<xmls.size(); ++it) {
    std::cout << "\n";
    std::cout << std::setw(15) << std::left << xmls[it].GetAnalyzer() 
	      << std::setw(25) << std::left << xmls[it].GetSim();

    if      (it == fNUpdates) { std::cout << "Running\r";       }
    if      (it <  fNUpdates) { std::cout << "DONE \u2713  \r"; }

  }

  fNUpdates++;
  for (int it=0; it<xmls.size()+2; ++it) { std::cout << "\033[F"; }
  std::cout << std::flush;
}
