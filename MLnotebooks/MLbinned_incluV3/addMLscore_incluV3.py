import numpy as np
import pandas as pd
import sys
from sklearn.preprocessing import StandardScaler

inputfile=str(sys.argv[1])
outputfile=str(sys.argv[2])
treename=str(sys.argv[3])
signaltype=str(sys.argv[4])
#model=str(sys.argv[3])

import uproot
#from keras.models import load_model
from tensorflow.python import keras
#from branches_V01 import features_train_1
#from branches_V01 import features_train_2
#from ROOT import TFile, TTree
filename_evaluate=inputfile
filename_friend=outputfile

from methods import feature_choice
from methods import feature_choice_zll
from binning import selections
from binning import selections_inclu
import binning
bins=[]
if signaltype=="T1bbbb":
#    bins=[13,15,16,18,19,20,23,25,26,32,33,34,45,46,54,55,57,64,65]
    bins=[13,15,16,18,19,20,23,25,26,32,33,34,45,46,54,55]
elif signaltype=="T1qqqq":
#    bins=[17,18,24,25,26,28,31,32,33,43,44,48,53,54,63,64,65,68]
#    bins=[17,18,24,25,26,28,31,32,33,43,44,48,53,54,63,64,65]
    bins=[17,18,24,25,28,31,32,43,44,48,53,54,63,68]
elif signaltype=="T2bb":
#    bins=[2,5,8,9,11,12,13,15,16,18,19,20,21,22,23,25,26,28,29,31,32,33,34,44,45,46,54,55,56,60,64,65]
#    bins=[2,5,8,9,11,12,13,15,16,18,19,20,21,22,23,25,26,28,29,31,32,33,34,44,45,46,54,55,56,60]
    bins=[2,5,8,9,11,12,13,15,18,19,20,21,25,28,31,32,44,45,46,54,55,56,60,64,65]
elif signaltype=="T2qq":
#    bins=[0,7,8,10,11,14,15,17,18,19,21,22,24,25,28,29,31,32,39,43,44,45,48,49,53,54,63,64,65]
#    bins=[0,3,7,8,10,11,14,15,17,18,19,21,22,24,25,28,29,31,32,33,39,43,44,53,54,55,63,64,68]
    bins=[0,7,8,10,11,14,15,17,18,19,21,22,24,25,28,29,31,32,39,43,44,45,48,49,53,54,63]

prediction_list=[]

with uproot.open(filename_evaluate) as f:
#    events = f["mt2"] #for signal and background of mc
    events=f[treename]#for data
    if len(events.array(b'met_pt'))==0:
        print("0 event in this file")
        sys.exit()
#  events=f['Events']
    ars={}
  # column stack with the **same** order of columns as they were given to the NN
    features=[]
    if "rootfiles_zinvcontrol" in filename_evaluate:
        features=feature_choice_zll(min(selections_inclu[0][1][1],16)-1)
    else:
        features=feature_choice(min(selections_inclu[0][1][1],16)-1)
    for i,ib in enumerate(features):
        ars[ib] = events.array(ib)
        ars[ib] = ars[ib].reshape((ars[ib].shape[0],1))
        if i==0: X = ars[ib]
        else: X = np.hstack((X, ars[ib]))

    print('X=', X)
    print('shape=', X.shape)
    print('\nLoading model')
    model='weights_'+signaltype+'_mergebin_0.best.hdf5'
    d1=np.load("significance_"+signaltype+"_output.npy",allow_pickle=True)
    dataset=d1.item()
    scaler=dataset['scalers'][0][0]
    loaded_model = keras.models.load_model(model,compile=False)
    prediction = loaded_model.predict(scaler.transform(X))

    prediction = prediction.reshape(prediction.shape[0])
    print('shape=',prediction.shape )
    prediction_list.append(prediction)
    prediction_list=np.vstack(prediction_list)
    np.save(filename_friend,prediction_list)
    print(filename_friend," saved")
