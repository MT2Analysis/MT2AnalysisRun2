import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import os
from tensorflow.python import keras
import sys
import math
import binning
import loaddata
import methods
import initialize
signaltype=str(sys.argv[1])
if signaltype!="T1bbbb" and signaltype!="T1qqqq" and signaltype!="T2bb" and signaltype!="T2qq":
    print("wrong signal type, please input T1bbbb or T1qqqq or T2bb or T2qq")
    sys.exit(0)

bkg =loaddata.loadbkg()
data=loaddata.loadSRdata()
d1=np.load("significance_"+signaltype+"_output.npy",allow_pickle=True)
models_forbin=d1.item()
binsused=[]
for j in range(len(models_forbin['mass'])):
    binrank=-1
    for i in models_forbin['binindex'][j]:
        binrank=binrank+1
        features_train=methods.feature_choice(min(binning.selections[i][1][1],16)-1)
        if i not in binsused:
            binsused.append(i)
            trainfra=models_forbin['trainfraction'][j][binrank][0]
            print("bin "+str(i)+" train fraction: ",trainfra)
            MCtest=bkg[i][bkg[i][b'MLtag']>=20*trainfra]
            filepath="weights_"+signaltype+"_bin_"+str(i)+".best.hdf5"
            model=keras.models.load_model(filepath)
            print("model for bin "+str(i)+" loaded")
            MCtest[b'MLscore']=model.predict(MCtest[features_train[:]],batch_size=20000)
            data[i][b'MLscore']=model.predict(data[i][features_train[:]],batch_size=20000)
            print("prediction completed")
            fig_compare=methods.plot_data_MC_compare(MCtest,data[i],trainfra,'data MC comparison of SR in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            fig_compare.savefig("SRcompare_"+signaltype+"_bin"+str(i)+".png")
            fig_compare.savefig("SRcompare_"+signaltype+"_bin"+str(i)+".pdf")
            print("saving figures")
