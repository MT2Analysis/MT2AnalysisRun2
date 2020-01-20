import numpy as np
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

d1=np.load("significance_output.npy",allow_pickle=True)
interval=0.005
dataset=d1.item()
listlen=[]
binsused=[]
f=open("cfgtry.txt","w")

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
        f.write(region+" "+str(cut)+" "+str(trainfra[0])+" "+str(trainfra[1])+" "+scorebranch+"\n")
f.close()
        
        














