#ifndef XMLPARSER_HH
#define XMLPARSER_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <sstream>

class ParsedXML {

private:
  std::string fSim;
  std::string fDirectory;
  std::string fFile;
  std::string fFilelist;
  std::string fTree;
  std::string fOutdir;
  std::string fAnalyzer;

public:
  ParsedXML(std::string cSim,
	    std::string cDirectory,
	    std::string cFile,
	    std::string cFilelist,
	    std::string cTree,
	    std::string cOutdir,
	    std::string cAnalyzer) {
      fSim       = cSim      ,
      fDirectory = cDirectory,
      fFile      = cFile     ,
      fFilelist  = cFilelist ,
      fTree      = cTree     ,
      fOutdir    = cOutdir   ,
      fAnalyzer  = cAnalyzer ;
  }

  ~ParsedXML() { };

  std::string GetSim      () { return fSim      ; }
  std::string GetDirectory() { return fDirectory; }
  std::string GetFile     () { return fFile     ; }
  std::string GetFilelist () { return fFilelist ; }
  std::string GetTree     () { return fTree     ; }
  std::string GetOutdir   () { return fOutdir   ; }
  std::string GetAnalyzer () { return fAnalyzer ; }

  friend std::ostream& operator<<(std::ostream& os, ParsedXML a) {
    if (a.GetFile() != "" && a.GetFilelist() == "") {
      os << "Simulation name:       " << a.GetSim()       << "\n"
	 << "Data source directory: " << a.GetDirectory() << "\n"
	 << "Data file:             " << a.GetFile()      << "\n"
	 << "Information TTree:     " << a.GetTree()      << "\n"
	 << "Output Directory:      " << a.GetOutdir()    << "\n"
	 << "Running analyzer:      " << a.GetAnalyzer()  << std::endl;
    }
    if (a.GetFile() == "" && a.GetFilelist() != "" ) {
      os << "Simulation name:       " << a.GetSim()       << "\n"
	 << "Data source directory: " << a.GetDirectory() << "\n"
	 << "Data file list:        " << a.GetFilelist()  << "\n"
	 << "Information TTree:     " << a.GetTree()      << "\n"
	 << "Output Directory:      " << a.GetOutdir()    << "\n"
	 << "Running analyzer:      " << a.GetAnalyzer()  << std::endl;
    }
    if (a.GetFile() != "" && a.GetFilelist() != "") {
      os << "Input file AND input file list are defined. Please specify only one" << std::endl;
    }
    return os;
  }
};


std::vector<ParsedXML> XMLParser(std::string config) {
  
  bool simIsOpen = false; bool simWasOpen = false;
  std::vector<ParsedXML>   ParsedXMLs;
  std::vector<std::string> MemberNames = {"sim", "directory", "file", "analyzers", "tree", "outdir", "filelist"};
  std::vector<std::string> Analyzers;
  std::map<std::string, std::string> ParserMembers;
  for (auto const& name: MemberNames) { ParserMembers[name] = ""; }

  std::string line;
  std::ifstream configFile (config);
  
  if (configFile.is_open()) {
    while (getline (configFile, line)) {

      // --- Ignore comments
      if (line.find("<!--") != std::string::npos) continue;

      // --- Get the stage
      std::string field = "";
      for (auto const& i: MemberNames)
	if (line.find(i) != std::string::npos) field = i;

      std::string header  = "<"  + field + ">";
      std::string tail    = "</" + field + ">";

      // --- open/close the sim
      if (field == "sim") {
	simIsOpen = !simIsOpen;
	header = "<"  + field + " name=\"";
	tail   = "\">";
      }
      
      // --- Erase whitespace
      if (field != "sim") line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

      // --- Remove the headers and tails
      if (line.find(header) != std::string::npos) { line.erase(0, header.size()); } // remove the front  
      size_t found = line.find(tail);
      if (found != std::string::npos) line.erase(found, tail.size());
      
      // --- Fill the analyzers vector
      if (field == "analyzers") {
	std::stringstream ss(line);
	while (ss.good()) { std::string substr; getline(ss, substr, ','); Analyzers.push_back(substr); }
      }

      // --- Fill the map
      if (simIsOpen)
	ParserMembers[field] = line;
	
      if (simWasOpen && !simIsOpen) {
	for (auto const& i: Analyzers)
	  ParsedXMLs.push_back(ParsedXML(ParserMembers["sim"], 
					 ParserMembers["directory"], 
					 ParserMembers["file"], 
					 ParserMembers["filelist"],
					 ParserMembers["tree"],
					 ParserMembers["outdir"],
					 i));
	Analyzers    .clear();
	ParserMembers.clear();
      }

      simWasOpen = simIsOpen;

    } // Looping line by line
  } // If XML is open
	
  return ParsedXMLs;

}

#endif
