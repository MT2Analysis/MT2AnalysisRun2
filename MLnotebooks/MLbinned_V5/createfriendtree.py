import sys

inputfile=str(sys.argv[1])
outputfile=str(sys.argv[2])
signaltype=str(sys.argv[3])


import numpy as np
from ROOT import TFile, TTree
from array import array
from binning import selections


def binstring(bin_number):
    info=selections[bin_number]
    if info[0][1]==np.Inf: htstring="HT"+str(info[0][0])+"toInf"
    else: htstring="HT"+str(info[0][0])+"to"+str(info[0][1])

    njetstring="j"+str(info[1][0])
    if (info[1][1]-1)!=info[1][0]:
        if info[1][1]==np.Inf: njetstring=njetstring+"toInf"
        else: njetstring=njetstring+"to"+str(info[1][1]-1)

    nbjetstring="b"+str(info[2][0])
    if (info[2][1]-1)!=info[2][0]:
        if info[2][1]==np.Inf: nbjetstring=nbjetstring+"toInf"
        else: nbjetstring=nbjetstring+"to"+str(info[2][1]-1)
    return htstring+"_"+njetstring+"_"+nbjetstring

prediction=np.load(inputfile)
(nscores,nentries)=prediction.shape
if (nentries==0 or nscores==0):
    print("the file is empty")
    sys.exit()
f1=TFile(outputfile,"recreate")
tree1=TTree("mt2_friend","mt2_friend")

bins=[]
if signaltype=="T1bbbb":
    bins=[13,15,16,18,19,20,23,25,26,32,33,34,45,46,54,55,57,64,65]
elif signaltype=="T1qqqq":
    bins=[17,18,24,25,26,28,31,32,33,43,44,48,53,54,63,64,65,68]
elif signaltype=="T2bb":
    bins=[2,5,8,9,11,12,13,15,16,18,19,20,21,22,23,25,26,28,29,31,32,33,34,44,45,46,54,55,56,60,64,65]
elif signaltype=="T2qq":
    bins=[0,8,10,11,14,15,17,18,19,21,22,24,25,26,28,29,31,32,39,43,44,45,48,49,53,54,63,64,65]
#    bins=[0,3,7,8,10,11,14,15,17,18,19,21,22,24,25,28,29,31,32,33,39,43,44,53,54,55,63,64,68]

branchnames=[]

for binname in bins:
    branchnames.append("score_bin"+str(binname)+"_"+binstring(binname))
 
for i in range(len(branchnames)):
    print(branchnames[i])


'''
branchnames=["score_bin13_HT450to575_j2to6_b3toInf",
             "score_bin15_HT575to1200_j2to3_b1",
             "score_bin18_HT575to1200_j4to6_b1",
             "score_bin19_HT575to1200_j4to6_b2",
             "score_bin20_HT575to1200_j2to6_b3toInf",
             "score_bin23_HT1200to1500_j2to3_b2",
             "score_bin25_HT1200to1500_j4to6_b1",
             "score_bin26_HT1200to1500_j4to6_b2",
             "score_bin32_HT1500toInf_j4to6_b1",
             "score_bin33_HT1500toInf_j4to6_b2",
             "score_bin34_HT1500toInf_j2to6_b3toInf",
             "score_bin45_HT575to1200_j7to9_b2",
             "score_bin46_HT575to1200_j7to9_b3",
             "score_bin54_HT1200to1500_j7to9_b1",
             "score_bin55_HT1200to1500_j7to9_b2",
             "score_bin57_HT1200to1500_j7to9_b4toInf",
             "score_bin64_HT1500toInf_j7to9_b1",
             "score_bin65_HT1500toInf_j7to9_b2"]
'''
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
