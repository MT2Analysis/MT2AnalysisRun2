import numpy as np
from binning import selections
import sys
filename=str(sys.argv[1])
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

binmap=[]

for i in range(len(selections)):
    binmap.append(binstring(i))


with open(filename) as f:
    for line in f:
        rank=line.split()
        print 'bin ', binmap.index(rank[0]),' ',rank[2],rank[0]

