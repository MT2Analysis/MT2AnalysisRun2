import tensorflow as tf
import numpy as np
import pandas as pd
from tensorflow.python.keras.layers import Dense
from tensorflow.python.keras.models import Sequential
from tensorflow.python.keras.losses import binary_crossentropy
from tensorflow.python.keras import optimizers
from tensorflow.python.keras import initializers
import matplotlib.pyplot as plt
from tensorflow.python.keras.callbacks import ModelCheckpoint
import os
from tensorflow.python import keras
import tempfile
import matplotlib as mpl
from sklearn.model_selection import train_test_split
import seaborn as sns
import sys
import math 
import random as rd
from sklearn.model_selection import KFold
import statistics
from math import sqrt

METRICS = [
      keras.metrics.TruePositives(name='tp'),
      keras.metrics.FalsePositives(name='fp'),
      keras.metrics.TrueNegatives(name='tn'),
      keras.metrics.FalseNegatives(name='fn'), 
      keras.metrics.BinaryAccuracy(name='accuracy'),
      keras.metrics.Precision(name='precision'),
      keras.metrics.Recall(name='recall'),
      keras.metrics.AUC(name='auc'),
]

bkgfiles_2016=['/scratch/wjin/featurereduced4/bkg/preselection_2016_singletop_skim.npy',
'/scratch/wjin/featurereduced4/bkg/preselection_2016_tt_skim.npy',
'/scratch/wjin/featurereduced4/bkg/preselection_2016_wjets_skim.npy',
'/scratch/wjin/featurereduced4/bkg/preselection_2016_zinv_skim.npy']

bkgfiles_2017=['/scratch/wjin/featurereduced4/bkg/preselection_2017_singletop_skim.npy',
'/scratch/wjin/featurereduced4/bkg/preselection_2017_tt_skim.npy',
'/scratch/wjin/featurereduced4/bkg/preselection_2017_wjets_skim.npy',
'/scratch/wjin/featurereduced4/bkg/preselection_2017_zinv_skim.npy']

bkgfiles_2018=['/scratch/wjin/featurereduced4/bkg/preselection_2018_singletop_skim.npy',
'/scratch/wjin/featurereduced4/bkg/preselection_2018_tt_skim.npy',
'/scratch/wjin/featurereduced4/bkg/preselection_2018_wjets_skim.npy',
'/scratch/wjin/featurereduced4/bkg/preselection_2018_zinv_skim.npy']

sigfiles_T1bbbb_2016=['/scratch/wjin/featurereduced4/sig/preselection_2016_T1bbbb_94x_1_skim.npy',
                     '/scratch/wjin/featurereduced4/sig/preselection_2016_T1bbbb_94x_2_skim.npy',
                     '/scratch/wjin/featurereduced4/sig/preselection_2016_T1bbbb_94x_skim.npy']

sigfiles_T1bbbb_2017=['/scratch/wjin/featurereduced4/sig/preselection_2017_T1bbbb_1_skim.npy',
                     '/scratch/wjin/featurereduced4/sig/preselection_2017_T1bbbb_2_skim.npy',
                     '/scratch/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_1_skim.npy',
                     '/scratch/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_2_skim.npy',
                     '/scratch/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_3_skim.npy',
                     '/scratch/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_4_skim.npy',
                     '/scratch/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_skim.npy',
                     '/scratch/wjin/featurereduced4/sig/preselection_2017_T1bbbb_skim.npy']

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
massskim=1
mass1con=1500
mass2con=800

def significance(data,m1,m2,sigfra,bkgfra,cut=0.5,weight=b'absweight',sigtag=b'sigtag',score=b'MLscore'):
    b=np.sum(data[(data[sigtag]==0)&(data[score]>cut)][weight])/bkgfra
    if b==0:
        return -1
    s=np.sum(data[(data[sigtag]==1)&(data[b'GenSusyMScan1']==m1)
            &(data[b'GenSusyMScan2']==m2)&(data[score]>cut)][weight])/sigfra
    q0=2*((s+b)*np.log(1+s/b)-s)
    return np.sqrt(q0)

def plot_metrics(history):
    metrics =  ['loss', 'auc', 'tp', 'accuracy']
#    mpl.rcParams['figure.figsize'] = (12, 10)
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
    fig=plt.figure(figsize=(12,10))
    for n, metric in enumerate(metrics):
        name = metric.replace("_"," ").capitalize()
        ax=fig.add_subplot(2,2,n+1)
        ax.plot(history.epoch,  history.history[metric], color=colors[0], label='Train')
        ax.plot(history.epoch, history.history['val_'+metric],
             color=colors[0], linestyle="--", label='Val')
        ax.set_xlabel('Epoch')
        ax.set_ylabel(name)
        ax.legend()
    return fig

features_exclude=[b'nJet30',b'nJet40',b'nBJet20',b'nBJet30',b'nBJet40',b'ht',b'mt2',b'mht_pt',b'met_pt',b'mht_phi',b'met_phi',b'njet']


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

databkg_2016=[]
for i in range(4):
    databkg_2016.append(pd.DataFrame(np.load(bkgfiles_2016[i],allow_pickle=True).item()))
    databkg_2016[i][b'bkgtype']=i
databkg_2016=pd.concat(databkg_2016,axis=0)

datasig_T1bbbb_2016=pd.concat([pd.DataFrame(np.load(i,allow_pickle=True).item()) for i in sigfiles_T1bbbb_2016],axis=0)
if massskim:
    datasig_T1bbbb_2016=datasig_T1bbbb_2016[(datasig_T1bbbb_2016[b'GenSusyMScan1']>=mass1con)|(datasig_T1bbbb_2016[b'GenSusyMScan2']>=mass2con)]
datasig_T1bbbb_2016[b'bkgtype']=-1

databkg_2016 = databkg_2016.replace(np.nan, 0)
datasig_T1bbbb_2016= datasig_T1bbbb_2016.replace(np.nan,0)

databkg_2016[b'evt_scale1fb'] = databkg_2016[b'evt_scale1fb'].abs()
databkg_2016[b'absweight'] = databkg_2016[b'absweight'].abs()
datasig_T1bbbb_2016[b'absweight'] = datasig_T1bbbb_2016[b'absweight'].abs()

pd.options.mode.chained_assignment = None
databkg_2016[b'sigtag']=0
datasig_T1bbbb_2016[b'sigtag']=1

databkg_2017=[]
for i in range(4):
    databkg_2017.append(pd.DataFrame(np.load(bkgfiles_2017[i],allow_pickle=True).item()))
    databkg_2017[i][b'bkgtype']=i
databkg_2017=pd.concat(databkg_2017,axis=0)

datasig_T1bbbb_2017=pd.concat([pd.DataFrame(np.load(i,allow_pickle=True).item()) for i in sigfiles_T1bbbb_2017],axis=0)
if massskim:
    datasig_T1bbbb_2017=datasig_T1bbbb_2017[(datasig_T1bbbb_2017[b'GenSusyMScan1']>=mass1con)|(datasig_T1bbbb_2017[b'GenSusyMScan2']>=mass2con)]
datasig_T1bbbb_2017[b'bkgtype']=-1

databkg_2017 = databkg_2017.replace(np.nan, 0)
datasig_T1bbbb_2017= datasig_T1bbbb_2017.replace(np.nan,0)

databkg_2017[b'evt_scale1fb'] = databkg_2017[b'evt_scale1fb'].abs()
databkg_2017[b'absweight'] = databkg_2017[b'absweight'].abs()
datasig_T1bbbb_2016[b'absweight'] = datasig_T1bbbb_2016[b'absweight'].abs()

pd.options.mode.chained_assignment = None
databkg_2017[b'sigtag']=0
datasig_T1bbbb_2017[b'sigtag']=1

databkg_2018=[]
for i in range(4):
    databkg_2018.append(pd.DataFrame(np.load(bkgfiles_2018[i],allow_pickle=True).item()))
    databkg_2018[i][b'bkgtype']=i
databkg_2018=pd.concat(databkg_2018,axis=0)
databkg_2018 = databkg_2018.replace(np.nan, 0)
databkg_2018[b'evt_scale1fb'] = databkg_2018[b'evt_scale1fb'].abs()
databkg_2018[b'absweight'] = databkg_2018[b'absweight'].abs()
databkg_2018[b'sigtag']=0
print("dataset prepared")
databkg_bin=[]
datasig_T1bbbb_bin=[]
for i in range(len(selections)):
    databkg_bin.append(pd.concat([databkg_2016[(databkg_2016[b'ht']>=selections[i][0][0])
                                              &(databkg_2016[b'ht']<selections[i][0][1])
                                              &(databkg_2016[b'nJet30']>=selections[i][1][0])
                                              &(databkg_2016[b'nJet30']<selections[i][1][1])
                                              &(databkg_2016[b'nBJet20']>=selections[i][2][0])
                                              &(databkg_2016[b'nBJet20']<selections[i][2][1])],
                                                  databkg_2017[(databkg_2017[b'ht']>=selections[i][0][0])
                                              &(databkg_2017[b'ht']<selections[i][0][1])
                                              &(databkg_2017[b'nJet30']>=selections[i][1][0])
                                              &(databkg_2017[b'nJet30']<selections[i][1][1])
                                              &(databkg_2017[b'nBJet20']>=selections[i][2][0])
                                              &(databkg_2017[b'nBJet20']<selections[i][2][1])],
                                                databkg_2018[(databkg_2018[b'ht']>=selections[i][0][0])
                                              &(databkg_2018[b'ht']<selections[i][0][1])
                                              &(databkg_2018[b'nJet30']>=selections[i][1][0])
                                              &(databkg_2018[b'nJet30']<selections[i][1][1])
                                              &(databkg_2018[b'nBJet20']>=selections[i][2][0])
                                              &(databkg_2018[b'nBJet20']<selections[i][2][1])]]))
    datasig_T1bbbb_bin.append(pd.concat([datasig_T1bbbb_2016[(datasig_T1bbbb_2016[b'ht']>=selections[i][0][0])
                                              &(datasig_T1bbbb_2016[b'ht']<selections[i][0][1])
                                              &(datasig_T1bbbb_2016[b'nJet30']>=selections[i][1][0])
                                              &(datasig_T1bbbb_2016[b'nJet30']<selections[i][1][1])
                                              &(datasig_T1bbbb_2016[b'nBJet20']>=selections[i][2][0])
                                              &(datasig_T1bbbb_2016[b'nBJet20']<selections[i][2][1])],
                                                datasig_T1bbbb_2017[(datasig_T1bbbb_2017[b'ht']>=selections[i][0][0])
                                              &(datasig_T1bbbb_2017[b'ht']<selections[i][0][1])
                                              &(datasig_T1bbbb_2017[b'nJet30']>=selections[i][1][0])
                                              &(datasig_T1bbbb_2017[b'nJet30']<selections[i][1][1])
                                              &(datasig_T1bbbb_2017[b'nBJet20']>=selections[i][2][0])
                                              &(datasig_T1bbbb_2017[b'nBJet20']<selections[i][2][1])]]))

del datasig_T1bbbb_2016
del datasig_T1bbbb_2017
del databkg_2016
del databkg_2017
del databkg_2018
print("dataset binned")
models_forbin={'mass':[[1400,1350],[2000,1600],[2400,1400],[2400,200]],
               'binindex':[[33,19,20,26,25,16,27,34,18,65,23,32,66,55,15,45,46,54,84], 
                          [20,47,13,46,27,57,34,66],
                          [34,27,33,66,67,65],
                          [33,34,65,32,66,64,67]],
               'significance':[],
               'traintag':[],
               'trainfraction':[]
}

datasig_T1bbbb_ML_bin=[]
databkg_ML_bin=[]
MLuse_bkg=[]
MLuse_sig=[]
MLusetag_bkg=[]
MLusetag_sig=[]
for i in range(len(selections)):
    if databkg_bin[i].shape[0]<20000:
        databkg_ML_bin.append(databkg_bin[i][databkg_bin[i][b'MLtag']<10])
        MLuse_bkg.append(0.5)
        MLusetag_bkg.append([0,1,2,3,4,5,6,7,8,9])
    elif databkg_bin[i].shape[0]<40000:
        databkg_ML_bin.append(databkg_bin[i][databkg_bin[i][b'MLtag']<5])
        MLuse_bkg.append(0.25)
        MLusetag_bkg.append([0,1,2,3,4])
    elif databkg_bin[i].shape[0]<80000:
        databkg_ML_bin.append(databkg_bin[i][databkg_bin[i][b'MLtag']<3])
        MLuse_bkg.append(0.15)
        MLusetag_bkg.append([0,1,2])
    else:
        databkg_ML_bin.append(databkg_bin[i][databkg_bin[i][b'MLtag']<2])
        MLuse_bkg.append(0.1)
        MLusetag_bkg.append([0,1])
    if datasig_T1bbbb_bin[i].shape[0]<20000:
        datasig_T1bbbb_ML_bin.append(datasig_T1bbbb_bin[i][datasig_T1bbbb_bin[i][b'MLtag']<10])
        MLuse_sig.append(0.5)
        MLusetag_sig.append([0,1,2,3,4,5,6,7,8,9])
    elif datasig_T1bbbb_bin[i].shape[0]<40000:
        datasig_T1bbbb_ML_bin.append(datasig_T1bbbb_bin[i][datasig_T1bbbb_bin[i][b'MLtag']<5])
        MLuse_sig.append(0.25)
        MLusetag_sig.append([0,1,2,3,4])
    elif datasig_T1bbbb_bin[i].shape[0]<80000:
        datasig_T1bbbb_ML_bin.append(datasig_T1bbbb_bin[i][datasig_T1bbbb_bin[i][b'MLtag']<3])
        MLuse_sig.append(0.15)
        MLusetag_sig.append([0,1,2])
    else:
        datasig_T1bbbb_ML_bin.append(datasig_T1bbbb_bin[i][datasig_T1bbbb_bin[i][b'MLtag']<2])
        MLuse_sig.append(0.1)
        MLusetag_sig.append([0,1])

data_input=[]

for i in range(len(selections)):
    data_input.append([databkg_ML_bin[i],datasig_T1bbbb_ML_bin[i]])
    data_input[i][0]=data_input[i][0].reset_index(drop=True)
    data_input[i][1]=data_input[i][1].reset_index(drop=True)

print("dataset sampled")
binsused=[]
valindex=[]
for j in range(4):
    m1=models_forbin['mass'][j][0]
    m2=models_forbin['mass'][j][1]
    print("for mass1= ",m1,", mass2= ",m2)
    models_forbin['significance'].append([])
    models_forbin['traintag'].append([])
    models_forbin['trainfraction'].append([])
    for i in models_forbin['binindex'][j]:
        features_train=feature_choice(selections[i][1][1]-1)
        X0=data_input[i][0][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype'])[:]]
        y0=data_input[i][0][b'sigtag']
        X1=data_input[i][1][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype'])[:]]
        y1=data_input[i][1][b'sigtag']
        models_forbin['traintag'][j].append([MLusetag_bkg[i],MLusetag_sig[i]])
        models_forbin['trainfraction'][j].append([MLuse_bkg[i],MLuse_sig[i]])
        if i not in binsused:
            X0_train, X0_val, y0_train, y0_val = train_test_split(X0, y0, test_size=0.1, random_state=42)
            X1_train, X1_val, y1_train, y1_val = train_test_split(X1, y1, test_size=0.1, random_state=90)
            binsused.append(i)
            valindex.append([list(X0_val.index.values),list(X1_val.index.values)])
            X0_train=X0_train.reset_index(drop=True)
            y0_train=y0_train.reset_index(drop=True)
            X0_val=X0_val.reset_index(drop=True)
            y0_val=y0_val.reset_index(drop=True)
            X1_train=X1_train.reset_index(drop=True)
            y1_train=y1_train.reset_index(drop=True)
            X1_val=X1_val.reset_index(drop=True)
            y1_val=y1_val.reset_index(drop=True)
            X_val=pd.concat([X0_val,X1_val])
            y_val=pd.concat([y0_val,y1_val])
            val=pd.concat([X_val,y_val],axis=1)
            print("train on ",X0_train.shape[0]," background", X1_train.shape[0]," signals, validate on ",X0_val.shape[0],"background",X1_val.shape[0],"signals")
            nsample=3*np.max([X0_train.shape[0],X1_train.shape[0]])
            ids0=np.arange(X0_train.shape[0])
            ids1=np.arange(X1_train.shape[0])
    #choices = np.random.choice(ids, data_input[i][0].shape[0])
            choices0 = np.random.choice(ids0, nsample)
            choices1 = np.random.choice(ids1, nsample)
    #res_data=pd.concat([data_input[i][1].iloc[choices,:],data_input[i][0]])
            res_X_train=pd.concat([X0_train.iloc[choices0,:],X1_train.iloc[choices1,:]])
            res_y_train=pd.concat([y0_train.iloc[choices0],y1_train.iloc[choices1]])
            res_X_train=res_X_train.reset_index(drop=True)
            res_y_train=res_y_train.reset_index(drop=True)
            nsample=3*np.max([X0_val.shape[0],X1_val.shape[0]])
            ids0=np.arange(X0_val.shape[0])
            ids1=np.arange(X1_val.shape[0])
            choices0 = np.random.choice(ids0, nsample)
            choices1 = np.random.choice(ids1, nsample)
    #res_data=pd.concat([data_input[i][1].iloc[choices,:],data_input[i][0]])
            res_X_val=pd.concat([X0_val.iloc[choices0,:],X1_val.iloc[choices1,:]])
            res_y_val=pd.concat([y0_val.iloc[choices0],y1_val.iloc[choices1]])
            res_X_train=res_X_train.reset_index(drop=True)
            res_y_train=res_y_train.reset_index(drop=True)
            res_X_val=res_X_val.reset_index(drop=True)
            res_y_val=res_y_val.reset_index(drop=True)
            print (res_y_train[res_y_train==0].shape[0], res_y_train[res_y_train==1].shape[0], res_y_val[res_y_val==0].shape[0], res_y_val[res_y_val==1].shape[0])

            def make_model(metrics = METRICS, output_bias=None):
                if output_bias is not None:
                    output_bias = tf.keras.initializers.Constant(output_bias)
                model = keras.Sequential([
                    keras.layers.Dense(
                    30, kernel_initializer='normal', activation='sigmoid',
                     input_shape=(len(features_train),)),
                     keras.layers.Dropout(0.5),
                     keras.layers.Dense(
                     10, kernel_initializer='normal', activation='sigmoid'),
                     keras.layers.Dense(
                     10, kernel_initializer='normal', activation='sigmoid'),
                      keras.layers.Dense(1, activation='sigmoid',
                         bias_initializer=output_bias),
                ])
                adam=optimizers.Adam(lr=0.001)
                model.compile(
                     optimizer='Adam',
                    loss='binary_crossentropy',
                     metrics=metrics)
                return model

            filepath="weights_T1bbbb_bin_"+str(i)+".best.hdf5"
            checkpoint = ModelCheckpoint(filepath, monitor='val_accuracy', verbose=1, save_best_only=True, mode='max')
            callbacks_list = [checkpoint]
            model = make_model()
            print("start training on bin ",i)
            model_history=model.fit(x=res_X_train[features_train[:]],
            y=res_y_train,
            batch_size=20000,
            epochs=200,
            verbose=0,
            callbacks = callbacks_list,
            validation_data=(res_X_val[features_train[:]], res_y_val))
            print("plotting history")
            histroyplot=plot_metrics(model_history)
            histroyplot.savefig("learning_curve_bin"+str(i)+".png")
            histroyplot.savefig("learning_curve_bin"+str(i)+".pdf")
            print("plotting score distribution")
            model.load_weights(filepath)
            figscore = plt.figure(figsize=(20,10))
            axscore = figscore.add_subplot(111)
            val[b'MLscore']=model.predict(val[features_train[:]], batch_size=20000)
            bkgscore=[val[(val[b'bkgtype']==0)|(val[b'bkgtype']==1)][b'MLscore'], \
               val[val[b'bkgtype']==2][b'MLscore'],
               val[val[b'bkgtype']==3][b'MLscore']]
            bkgweight=[val[(val[b'bkgtype']==0)|(val[b'bkgtype']==1)][b'evt_scale1fb'], \
               val[val[b'bkgtype']==2][b'evt_scale1fb'],
               val[val[b'bkgtype']==3][b'evt_scale1fb']]
            bkgname=['top','wjets','zinv']
            axscore.hist(bkgscore,bins=np.arange(0,1.025,0.025),stacked=True,alpha=0.5,label=bkgname,density=True,weights=bkgweight)
            axscore.hist(val[val[b'sigtag']==1][b'MLscore'],bins=np.arange(0,1.025,0.025),alpha=1,label='signal '+str(m1)+" "+str(m2),density=True,histtype='step',linewidth=2)
            axscore.legend(loc='upper left',prop={'size': 12})
            axscore.set_title("T1bbbb score distribution of signal and background for validation set--bin"+str(i)+" ht "+str(selections[i][0][0])
                       +"-"+str(selections[i][0][1])+" njet "+str(selections[i][1][0])+"-"+str(selections[i][1][0]-1)
                       +" nbjet "+str(selections[i][2][0])+"-"+str(selections[i][2][1]),fontsize=20)
            axscore.set_xlabel('Score',fontsize=18)
            axscore.set_ylabel('fraction',fontsize=18)
            figscore.savefig("scorecompare_bin_"+str(i)+".png")
            figscore.savefig("scorecompare_bin_"+str(i)+".pdf")
        else:
            filepath="weights_T1bbbb_bin_"+str(i)+".best.hdf5"
            model=keras.models.load_model(filepath)
            valindex_thisbin=valindex[binsused.index(i)]
            X_val=pd.concat([X0.loc[valindex_thisbin[0]],X1.loc[valindex_thisbin[1]]])
            y_val=pd.concat([y0.loc[valindex_thisbin[0]],y1.loc[valindex_thisbin[1]]])
            val=pd.concat([X_val,y_val],axis=1)
            val[b'MLscore']=model.predict(val[features_train[:]], batch_size=20000)
        midsig=[]
        for cut in np.arange(0, 1.005,0.005):
            midsig.append(significance(val,m1,m2,MLuse_sig[i]*0.1,MLuse_bkg[i]*0.1,cut,sigtag=b'sigtag',score=b'MLscore'))
        models_forbin['significance'][j].append(midsig)
        print("plotting significance")
        figmidsig=plt.figure(figsize=(16,10))
        axmidsig = figmidsig.add_subplot(111)
        axmidsig.plot(np.arange(0, 1.005,0.005),midsig)
        axmidsig.set_title("significance change of "+"mass "+str(m1)+" "+str(m2) +" with cut--bin"+str(i)+" ht "+str(selections[i][0][0])
                       +"-"+str(selections[i][0][1])+" njet "+str(selections[i][1][0])+"-"+str(selections[i][1][0]-1)
                       +" nbjet "+str(selections[i][2][0])+"-"+str(selections[i][2][1]),fontsize=18)
        axmidsig.set_xlabel('Score cut',fontsize=18)
        axmidsig.set_ylabel('approximate median significance',fontsize=18)
        figmidsig.savefig("significance_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".png")
        figmidsig.savefig("significance_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".pdf")
        print("finished plotting")
np.save("significance_output.npy",models_forbin)
