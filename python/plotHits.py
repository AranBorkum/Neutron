import ROOT
import argparse

from ROOT import TFile
from ROOT import TTree

parser = argparse.ArgumentParser()

parser.add_argument("-i", "--input-file", type=str,
                    help="Input file for the plotting script")
parser.add_argument("-o", "--output-file", type=str,
                    help="Output file for the plotting script")
parser.add_argument("-t", "--tree-name", type=str,
                    help="Tree name")
args = parser.parse_args()

# --- Import the propper TTree
f = TFile.Open(args.input_file, "READ")
t = f.Get(args.tree_name)

for i in t.Hit_View:
    print i
