import sys
import numpy as np
from binning import selections
#This file reads the significances from the directories saved from training, choose the best cut position, and writes the configuration file for cutting in the analysis
#USAGE: python readsign.py <signal type>
#<signal type>=T1bbbb/T1qqqq/T2bb/T2qq

signaltype=str(sys.argv[1])

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
if signaltype=="T1bbbb" or signaltype=="T1qqqq":
    d1=np.load("significance_"+signaltype+"_output.npy",allow_pickle=True)
elif signaltype=="T2bb":
    d1=np.load("significance_"+signaltype+"_output.npy",allow_pickle=True)
else:
    d1=np.load("significance_"+signaltype+"_output.npy",allow_pickle=True)
interval=0.005
dataset=d1.item()
listlen=[]
binsused=[]
f1=open("cutinfBDTV3_2_1_"+signaltype+".txt","w")
f2=open("cutinfBDTV3_2_5_"+signaltype+".txt","w")
f3=open("cutinfBDTV3_2_10_"+signaltype+".txt","w")

for i in range(len(dataset['mass'])):
    listlen.append(len(dataset['binindex'][i]))

for ibin in range(max(listlen)):
    binssamerank=[]; masssamerank=[]
    for i in range(len(dataset['mass'])):
        if len(dataset['binindex'][i]) < ibin+1: continue
        bin_number=dataset['binindex'][i][ibin]
        if bin_number in binsused: continue
        thismass=dataset['mass'][i]
        if bin_number in binssamerank:
            massused=masssamerank[binssamerank.index(bin_number)]
            if np.divide(thismass[0],thismass[1])<np.divide(massused[0],massused[1]): continue
            masssamerank[binssamerank.index(bin_number)]=thismass
        else:
            binssamerank.append(bin_number)
            masssamerank.append(thismass)
#        binsused.append(bin_number)
    for i in range(len(binssamerank)):
        sig=dataset['significance'][dataset['mass'].index(masssamerank[i])][ibin]
        cut=interval*sig.index(max(sig))
        if cut==0: continue
        region=binstring(binssamerank[i])
        binsused.append(binssamerank[i])
        print("bin ",binssamerank[i]," mass ",masssamerank[i][0]," ",masssamerank[i][1]," ",region)
        trainfra=dataset['trainfraction'][dataset['mass'].index(masssamerank[i])][ibin]
        scorebranch="score_bin"+str(binssamerank[i])+"_"+region
        f1.write(region+" "+str(cut)+" "+str(trainfra[0])+" "+str(trainfra[1])+" "+scorebranch+"\n")

    for i in range(len(binssamerank)):
        sig=dataset['significance'][dataset['mass'].index(masssamerank[i])][ibin]
        zlldataCR=dataset['zlldataCR'][dataset['mass'].index(masssamerank[i])][ibin]
        llepdataCR=dataset['llepdataCR'][dataset['mass'].index(masssamerank[i])][ibin]
        sig_abovethreshold=[sig_value for (sig_value,zll_value,llep_value) in zip(sig,zlldataCR,llepdataCR) if zll_value >=5 and llep_value >=5]
        if len(sig_abovethreshold)==0:continue
        cut=interval*sig.index(max(sig_abovethreshold))
        if cut==0: continue
        region=binstring(binssamerank[i])
        binsused.append(binssamerank[i])
        print("bin ",binssamerank[i]," mass ",masssamerank[i][0]," ",masssamerank[i][1]," ",region)
        trainfra=dataset['trainfraction'][dataset['mass'].index(masssamerank[i])][ibin]
        scorebranch="score_bin"+str(binssamerank[i])+"_"+region
        f2.write(region+" "+str(cut)+" "+str(trainfra[0])+" "+str(trainfra[1])+" "+scorebranch+"\n")

    for i in range(len(binssamerank)):
        sig=dataset['significance'][dataset['mass'].index(masssamerank[i])][ibin]
        zlldataCR=dataset['zlldataCR'][dataset['mass'].index(masssamerank[i])][ibin]
        llepdataCR=dataset['llepdataCR'][dataset['mass'].index(masssamerank[i])][ibin]
        sig_abovethreshold=[sig_value for (sig_value,zll_value,llep_value) in zip(sig,zlldataCR,llepdataCR) if zll_value >=10 and llep_value >=10]
        if len(sig_abovethreshold)==0:continue
        cut=interval*sig.index(max(sig_abovethreshold))
        if cut==0: continue
        region=binstring(binssamerank[i])
        binsused.append(binssamerank[i])
        print("bin ",binssamerank[i]," mass ",masssamerank[i][0]," ",masssamerank[i][1]," ",region)
        trainfra=dataset['trainfraction'][dataset['mass'].index(masssamerank[i])][ibin]
        scorebranch="score_bin"+str(binssamerank[i])+"_"+region
        f3.write(region+" "+str(cut)+" "+str(trainfra[0])+" "+str(trainfra[1])+" "+scorebranch+"\n")
f1.close()
f2.close()      
f3.close() 
        














