import sys

inputfile=str(sys.argv[1])
outputfile=str(sys.argv[2])

import numpy as np
from ROOT import TFile, TTree
from array import array
prediction=np.load(inputfile)

f1=TFile(outputfile,"recreate")
tree1=TTree("mt2_friend","mt2_friend")
score=array('d',[0])
tree1.Branch("score_V01",score,"score_V01/D")
for i in range(len(prediction)):
    score[0]=prediction[i]
    tree1.Fill()
f1.Write()
f1.Close()
