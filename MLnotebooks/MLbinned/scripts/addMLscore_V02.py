import numpy as np
import pandas as pd
import sys

inputfile=str(sys.argv[1])
outputfile=str(sys.argv[2])
#model=str(sys.argv[3])

import uproot
#from keras.models import load_model
from tensorflow.python import keras
#from branches_V01 import features_train_1
#from branches_V01 import features_train_2
#from ROOT import TFile, TTree
filename_evaluate=inputfile
filename_friend=outputfile


htbin=[[250,450],[450,575],[575,1200],[1200,1500],[1500,np.Inf],[250,350],[350,450],[575,700],[700,1000],[1000,1200],[1200,np.Inf],[700,np.Inf]]
nJet30bin=[[2,4],[4,7],[7,10],[10,16],[1,2]]
nBJet20bin=[[0,1],[1,2],[2,3],[3,4],[4,16]]
selections=[]
for htrange in range(5):
    selections.append([htbin[htrange],nJet30bin[0],nBJet20bin[0]])
    selections.append([htbin[htrange],nJet30bin[0],nBJet20bin[1]])
    selections.append([htbin[htrange],nJet30bin[0],nBJet20bin[2]])
    selections.append([htbin[htrange],nJet30bin[1],nBJet20bin[0]])
    selections.append([htbin[htrange],nJet30bin[1],nBJet20bin[1]])
    selections.append([htbin[htrange],nJet30bin[1],nBJet20bin[2]])
    selections.append([htbin[htrange],[nJet30bin[0][0],nJet30bin[1][1]],[nBJet20bin[3][0],16]])
for htrange in [0,1]:
    selections.append([htbin[htrange],[nJet30bin[2][0],16],nBJet20bin[0]])
    selections.append([htbin[htrange],[nJet30bin[2][0],16],nBJet20bin[1]])
    selections.append([htbin[htrange],[nJet30bin[2][0],16],nBJet20bin[2]])
    selections.append([htbin[htrange],[nJet30bin[2][0],16],[nBJet20bin[3][0],16]])
for htrange in [2,3,4]:
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[0]])
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[1]])
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[2]])
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[3]])
    selections.append([htbin[htrange],nJet30bin[2],nBJet20bin[4]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[0]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[1]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[2]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[3]])
    selections.append([htbin[htrange],nJet30bin[3],nBJet20bin[4]])
selections.append([htbin[5],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[6],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[1],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[7],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[8],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[9],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[10],nJet30bin[4],nBJet20bin[0]])
selections.append([htbin[5],nJet30bin[4],nBJet20bin[1]])
selections.append([htbin[6],nJet30bin[4],nBJet20bin[1]])
selections.append([htbin[1],nJet30bin[4],nBJet20bin[1]])
selections.append([htbin[7],nJet30bin[4],nBJet20bin[1]])
selections.append([htbin[11],nJet30bin[4],nBJet20bin[1]])


def feature_choice(njet):
    features_pre=[[b'jet1_eta',b'jet1_phi',b'jet1_pt',b'jet1_btagDeepCSV'],
             [b'jet2_eta',b'jet2_phi',b'jet2_pt',b'jet2_btagDeepCSV'],
             [b'jet3_eta',b'jet3_phi',b'jet3_pt',b'jet3_btagDeepCSV'],
             [b'jet4_eta',b'jet4_phi',b'jet4_pt',b'jet4_btagDeepCSV'],
             [b'jet5_eta',b'jet5_phi',b'jet5_pt',b'jet5_btagDeepCSV'],
             [b'jet6_eta',b'jet6_phi',b'jet6_pt',b'jet6_btagDeepCSV'],
             [b'jet7_eta',b'jet7_phi',b'jet7_pt',b'jet7_btagDeepCSV'],
             [b'jet8_eta',b'jet8_phi',b'jet8_pt',b'jet8_btagDeepCSV'],
             [b'jet9_eta',b'jet9_phi',b'jet9_pt',b'jet9_btagDeepCSV'],
             [b'jet10_eta',b'jet10_phi',b'jet10_pt',b'jet0_btagDeepCSV'],
             [b'jet11_eta',b'jet11_phi',b'jet11_pt',b'jet11_btagDeepCSV'],
             [b'jet12_eta',b'jet12_phi',b'jet12_pt',b'jet12_btagDeepCSV'],
             [b'jet13_eta',b'jet13_phi',b'jet13_pt',b'jet13_btagDeepCSV'],
             [b'jet14_eta',b'jet14_phi',b'jet14_pt',b'jet14_btagDeepCSV'],
             [b'jet15_eta',b'jet15_phi',b'jet15_pt',b'jet15_btagDeepCSV']]
    features_train=[b'deltaPhiMin',b'diffMetMht',b'mht_pt',b'met_pt',b'mht_phi',b'met_phi']
    for i in range(njet):
        features_train=features_train+features_pre[i]
    return features_train


bins=[13,15,16,18,19,20,25,27,32,33,34,45,46,47,64,65,66,67,84]
prediction_list=[]

with uproot.open(filename_evaluate) as f:
    events = f["mt2"]
    if len(events.array(b'met_pt'))==0:
        print("0 event in this file")
        sys.exit()
#  events=f['Events']
    ars={}
  # column stack with the **same** order of columns as they were given to the NN
    for binindex in bins:
        print("bin",binindex)
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
        model='/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/MLnotebooks/MLbinned/scripts/weights_T1bbbb_bin_'+str(binindex)+'.best.hdf5'
        loaded_model = keras.models.load_model(model)
        prediction = loaded_model.predict(X)

        prediction = prediction.reshape(prediction.shape[0])
        print('shape=',prediction.shape )
        prediction_list.append(prediction)
    prediction_list=np.vstack(prediction_list)
    np.save(filename_friend,prediction_list)
    print(filename_friend," saved")
