import numpy as np
import pandas as pd
import sys

inputfile=str(sys.argv[1])
outputfile=str(sys.argv[2])
treename=str(sys.argv[3])
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
from binning import selections

bins=[13,19,20,23,27,32,33,45,47,54,57,64,66]
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
            features=feature_choice_zll(selections[binindex][1][1]-1)
        else:
            features=feature_choice(selections[binindex][1][1]-1)
        for i,ib in enumerate(features):
#            print('Reading branch=', ib)
            ars[ib] = events.array(ib)
            ars[ib] = ars[ib].reshape((ars[ib].shape[0],1))
            if i==0: X = ars[ib]
            else: X = np.hstack((X, ars[ib]))

        print('X=', X)
        print('shape=', X.shape)
        print('\nLoading model')
        model='weights_T1bbbb_bin_'+str(binindex)+'.best.hdf5'
        loaded_model = keras.models.load_model(model)
        prediction = loaded_model.predict(X)

        prediction = prediction.reshape(prediction.shape[0])
        print('shape=',prediction.shape )
        prediction_list.append(prediction)
    prediction_list=np.vstack(prediction_list)
    np.save(filename_friend,prediction_list)
    print(filename_friend," saved")
