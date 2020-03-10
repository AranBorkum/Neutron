#include <iostream>
#include <fstream>
#include <unistd.h>

#include "XMLParser.hh"
#include "CaptureAnalyzer.hh"
#include "ValidationAnalyzer.hh"
#include "GeometricAnalyzer.hh"

#include "boost/program_options.hpp"

namespace po = boost::program_options;

void RunAnalyzer(ParsedXML xml, po::variables_map vm) {

  std::string ana = xml.GetAnalyzer();
  if      (ana == "Capture")     CaptureAna   (xml, vm["nevents"].as<int>());
  else if (ana == "Validation0") ValidationAna(xml, vm["nevents"].as<int>(), 0);
  else if (ana == "Validation1") ValidationAna(xml, vm["nevents"].as<int>(), 1);
  else if (ana == "Validation2") ValidationAna(xml, vm["nevents"].as<int>(), 2);
  else                          {;}
}

int main(int argc, char** argv) {
  
  // --- Argument Parser ---
  po::options_description desc("Allowed Options");
  desc.add_options()
    ("help,h"   , "produce help message")
    ("config,c" , po::value<std::string>(), "Configurations contained in XML")
    ("nevents,n", po::value<int>()->default_value(10), "Number of events to run" )
    ("nskip,k"  , po::value<int>()->default_value(0) , "Number of events to skip");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help") || vm.empty()) {
    std::cout << desc << "\n"; return 1;
  }

  std::vector<ParsedXML> xmls = XMLParser(vm["config"].as<std::string>());
  for (int i=0; i<xmls.size(); ++i) {
    std::cout << "Running " << xmls[i].GetAnalyzer() << " on " << xmls[i].GetSim();
    RunAnalyzer(xmls[i], vm);	
    std::cout << "\tDONE \u2713" << std::endl;
  }

  return 0;

}
