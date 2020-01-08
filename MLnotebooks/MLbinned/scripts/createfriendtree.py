import sys

inputfile=str(sys.argv[1])
outputfile=str(sys.argv[2])

import numpy as np
from ROOT import TFile, TTree
from array import array

prediction=np.load(inputfile)
(nscores,nentries)=prediction.shape
if (nentries==0 or nscores==0):
    print("the file is empty")
    sys.exit()
f1=TFile(outputfile,"recreate")
tree1=TTree("mt2_friend","mt2_friend")
branchnames=["score_bin13_HT450to575_j2to6_b3toInf",
             "score_bin15_HT575to1200_j2to3_b1",
             "score_bin16_HT575to1200_j2to3_b2",
             "score_bin18_HT575to1200_j4to6_b1",
             "score_bin19_HT575to1200_j4to6_b2",
             "score_bin20_HT575to1200_j2to6_b3toInf",
             "score_bin25_HT1200to1500_j4to6_b1",
             "score_bin27_HT1200to1500_j2to6_b3toInf",
             "score_bin32_HT1500toInf_j4to6_b1",
             "score_bin33_HT1500toInf_j4to6_b2",
             "score_bin34_HT1500toInf_j2to6_b3toInf",
             "score_bin45_HT575to1200_j7to9_b2",
             "score_bin46_HT575to1200_j7to9_b3",
             "score_bin47_HT575to1200_j7to9_b4toInf",
             "score_bin64_HT1500toInf_j7to9_b1",
             "score_bin65_HT1500toInf_j7to9_b2",
             "score_bin66_HT1500toInf_j7to9_b3",
             "score_bin67_HT1500toInf_j7to9_b4toInf",
             "score_bin84_HT700toInf_j1_b1toInf"]
score=[]
for iscore in range(nscores):
    score.append(array('d',[0]))
    tree1.Branch(branchnames[iscore],score[iscore],branchnames[iscore]+"/D")
#score=array('d',[0])
#tree1.Branch("score_V01",score,"score_V01/D")
for ientry in range(nentries):
    for iscore in range(nscores):
        score[iscore][0]=prediction[iscore][ientry]
    tree1.Fill()
f1.Write()
f1.Close()
