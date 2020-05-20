import numpy as np
import pandas as pd
import sys
from sklearn.preprocessing import StandardScaler
from joblib import dump,load
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
num_fold=9
bins=[]
if signaltype=="T1bbbb":
    bins=[13,15,16,18,19,20,23,25,26,32,33,34,45,46,54,55,57,64,65]
elif signaltype=="T1qqqq":
    bins=[17,18,24,25,26,28,31,32,33,43,44,48,53,54,63,64,65,68]
elif signaltype=="T2bb":
    bins=[2,5,8,9,11,12,13,15,16,18,19,20,21,22,23,25,26,28,29,31,32,33,34,44,45,46,54,55,56,60,64,65]
elif signaltype=="T2qq":
    bins=[0,7,8,10,11,14,15,17,18,19,21,22,24,25,26,28,29,31,32,39,43,44,45,48,49,53,54,63,64,65]
#    bins=[0,3,7,8,10,11,14,15,17,18,19,21,22,24,25,28,29,31,32,33,39,43,44,53,54,55,63,64,68]

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
    for binindex in bins:
        print("bin",binindex)
        features=[]
        if "rootfiles_zinvcontrol" in filename_evaluate:
            features=feature_choice_zll(min(selections[binindex][1][1],16)-1)
        else:
            features=feature_choice(min(selections[binindex][1][1],16)-1)
        for i,ib in enumerate(features):
#            print('Reading branch=', ib)
            ars[ib] = events.array(ib)
            ars[ib] = np.nan_to_num(ars[ib])
            ars[ib] = ars[ib].reshape((ars[ib].shape[0],1))
            if i==0: X = ars[ib]
            else: X = np.hstack((X, ars[ib]))

        print('X=', X)
        print('shape=', X.shape)
        print('\nLoading model')
        models=[]
        for foldindex in range(num_fold):
            models.append(load('model_'+signaltype+'_bin'+str(binindex)+"_"+str(foldindex)+'.joblib'))
        d1=np.load("significance_"+signaltype+"_output.npy",allow_pickle=True)
        dataset=d1.item()
#        for i in range(len(dataset['mass'])):
#            if binindex in dataset['binindex'][i]:
#                scaler=dataset['scalers'][i][dataset['binindex'][i].index(binindex)]
#                print('find scaler of bin ',binindex,', mass ',dataset['mass'][i][0],' ',dataset['mass'][i][1])
#                break

        prediction = np.mean([models[foldindex].predict(X) for foldindex in range(num_fold)],axis=0)

        prediction = prediction.reshape(prediction.shape[0])
        print('shape=',prediction.shape )
        prediction_list.append(prediction)
    prediction_list=np.vstack(prediction_list)
    np.save(filename_friend,prediction_list)
    print(filename_friend," saved")
