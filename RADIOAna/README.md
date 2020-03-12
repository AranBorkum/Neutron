# RADIOAna
### A software collection for analysing radiological backgrounds on the DUNE far detector

### Configurations
The input for the programs is an XML file. The structure is as follows:
```xml
<sim name="name-of-Simulation">
    <directory>/path/to/input/file/</directory>
    <file>inputFile.root</file>
    <tree>tree/name</tree>
    <outdir>/path/to/output/directory/</outdir>
    <analyzers>Analyzer1,Analyzer2,Analyzer3</analyzers>
</sim>
```
A dedicated XML parser writed for this collection parses the various attributes and costructs a `ParsedXML` object. You can specify as many analyzers as you want in the configuration. The XML parser will construct a `ParsedXML` object for each analyzer.

### Analyzers
Currently there are two analzers included. These are included in the `Analyzers/` directory. More can be added as required. The convention currently is that the analyzers must be named with a descriptor followed by `Analyzer.hh`. Instead of a main function the process that is ultimately executed is names as the descriptor followed by `Ana(<arguments>)`.
##### CaptureAna
This process produces outputs that show the capture location and various kinematic features of neutron capture on Argon. It is also designed to calculate the radiological neutron capture rate in the simulation.
##### ValidationAna
The process is desigend to show that the simulation is running as expected. It looks at which regions of the detector neutrons are being produced. It also looks at from which region the neutrons that ultimatly contribute to the overall capture rate come from. This is important in validating the production configuration. It also calculates the neutron flux per unit area to make sure we're simulating the correct physics

### Future planned updates
- Write a file, most likely in python, to construct a `RADIOAna.cxx` file given the contents of the `Analyzers/` directory and the functions within the files.
- Write an analyzer that works on hit level information to test the detector responce to neutron capture photon emissions
- Write an analyzer that performs clustering and calculates a capture rate based on the clusters associated with neutrons
