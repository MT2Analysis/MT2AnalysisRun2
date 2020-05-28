import tensorflow as tf
import numpy as np
from numpy.random import seed
seed(1)
tf.random.set_seed(2)
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
from sklearn.model_selection import StratifiedKFold
import statistics
from math import sqrt
from statistics import mean
import binning
import loaddata
import methods
import initialize
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import GradientBoostingRegressor
from joblib import dump,load
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

#Some options for the training, 
#massskim: whether to do the skim on the signal mass spectrum, 
#usehalfspliting: whether to use half of the samples for machine learning, otherwise the number of samples for ML is based on total sample size, 
#dozllmerge: whether to merge some high Ht bins for zll and zinv samples
#usedataexp: whether to use expectations from simulation instead of CR data in background estimation
#num_fold: number of folds in cross validation
massskim=True
usehalfspliting=False
dozllmerge=True
usedataexp=False
num_fold=9

signaltype=str(sys.argv[1])
if signaltype!="T1bbbb" and signaltype!="T1qqqq" and signaltype!="T2bb" and signaltype!="T2qq":
    print("wrong signal type, please input T1bbbb or T1qqqq or T2bb or T2qq")
    sys.exit(0)
#features_exclude=[b'nJet30',b'nJet40',b'nBJet20',b'nBJet30',b'nBJet40',b'ht',b'mt2',b'mht_pt',b'met_pt',b'mht_phi',b'met_phi',b'njet']

#import datasets

bkg=loaddata.loadbkg()
sig=loaddata.loadsig(massskim=massskim,sig=signaltype)
llepMC=loaddata.loadcontrol_MC(controltype="llep")
zllMC=loaddata.loadcontrol_MC(controltype="zll")
llepdata=loaddata.loadcontrol_data(controltype="llep")
zlldata=loaddata.loadcontrol_data(controltype="zll")
print("dataset prepared")

models_forbin={}
if signaltype=="T1bbbb":
    models_forbin=initialize.models_T1bbbb
elif signaltype=="T1qqqq":
    models_forbin=initialize.models_T1qqqq
elif signaltype=="T2bb":
    models_forbin=initialize.models_T2bb
elif signaltype=="T2qq":
    models_forbin=initialize.models_T2qq

if usehalfspliting:
    (databkg_ML_bin,databkg_test_bin,MLuse_bkg,MLusetag_bkg)=initialize.halfspliting(bkg)
else:
    (databkg_ML_bin,databkg_test_bin,MLuse_bkg,MLusetag_bkg)=initialize.spliting(bkg)
del bkg

if usehalfspliting:
    (datasig_ML_bin,datasig_test_bin,MLuse_sig,MLusetag_sig)=initialize.halfspliting(sig)
else:
    (datasig_ML_bin,datasig_test_bin,MLuse_sig,MLusetag_sig)=initialize.spliting(sig)
del sig

print("dataset sampled")
binsused=[]
#binsused_scalers=[]
#valindex=[]

#In the following we loop over the four benchmark mass-mass points
for j in range(len(models_forbin['mass'])):
#initialize the list of variables to save
    m1=models_forbin['mass'][j][0]
    m2=models_forbin['mass'][j][1]
    print("for mass1= ",m1,", mass2= ",m2)
    models_forbin['significance'].append([])
    models_forbin['traintag'].append([])
    models_forbin['trainfraction'].append([])
    models_forbin['mt2_score_corr'].append([])
    models_forbin['zlldataCR'].append([])
    models_forbin['llepdataCR'].append([])
#    models_forbin['scalers'].append([])
#    models_forbin['trainplots'].append([])
#    models_forbin['train_val_plots'].append([])
#    models_forbin['bkg_sig_plots'].append([])
#    models_forbin['CR_plots'].append([])
#    models_forbin['significance_plots'].append([])

#For each benchmark point, we loop over its most sensitive Ht, njet, nbjet bins.
    for i in models_forbin['binindex'][j]:
        tf.random.set_seed(2)
        features_train=methods.feature_choice(min(binning.selections[i][1][1],16)-1)
        features_zllCR=methods.feature_choice_zll(min(binning.selections[i][1][1],16)-1)
#        scaler = StandardScaler()
        X0=databkg_ML_bin[i][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]]
        y0=databkg_ML_bin[i][b'sigtag']
#For signal T1qqqq and T1bbbb, add 3*weight for the signals in compressed region, so the model can pay more attention to them in training.
        if signaltype=="T1qqqq" and j==0:
            X1=pd.concat([datasig_ML_bin[i][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]],
                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-300)][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]],
                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-300)][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]]],axis=0)
            y1=pd.concat([datasig_ML_bin[i][b'sigtag'],
                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-300)][b'sigtag'],
                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-300)][b'sigtag']],axis=0)
        elif signaltype=="T1bbbb" and j==0:
            X1=pd.concat([datasig_ML_bin[i][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]],
                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-300)][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]],
                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-300)][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]]],axis=0)
            y1=pd.concat([datasig_ML_bin[i][b'sigtag'],
                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-300)][b'sigtag'],
                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-300)][b'sigtag']],axis=0)
#        elif signaltype=="T2bb" and ((i in models_forbin['binindex'][1]) or (i in models_forbin['binindex'][2]) or (i in models_forbin['binindex'][3])):
#            X1=pd.concat([datasig_ML_bin[i][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]],
#                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']<=(datasig_ML_bin[i][b'GenSusyMScan1']-800)][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]],
#                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']<=(datasig_ML_bin[i][b'GenSusyMScan1']-800)][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]],
#                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-200)][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]],
#                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-200)][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]]],axis=0)
#            y1=pd.concat([datasig_ML_bin[i][b'sigtag'],
#                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']<=(datasig_ML_bin[i][b'GenSusyMScan1']-800)][b'sigtag'],
#                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']<=(datasig_ML_bin[i][b'GenSusyMScan1']-800)][b'sigtag'],
#                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-200)][b'sigtag'],
#                      datasig_ML_bin[i][datasig_ML_bin[i][b'GenSusyMScan2']>=(datasig_ML_bin[i][b'GenSusyMScan1']-200)][b'sigtag']],axis=0)
        else:
            X1=datasig_ML_bin[i][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]]
            y1=datasig_ML_bin[i][b'sigtag']

        models_forbin['traintag'][j].append([MLusetag_bkg[i],MLusetag_sig[i]])
        models_forbin['trainfraction'][j].append([MLuse_bkg[i],MLuse_sig[i]])
        if dozllmerge:
            mergebin=methods.zllmergebin(i)
            print("mergebin for bin ",i,": ",mergebin)
        if i not in binsused:
#Split the ML set into training+validation, and testing set
            X0_train, X0_test, y0_train, y0_test = train_test_split(X0, y0, test_size=0.1, random_state=42)
            X1_train, X1_test, y1_train, y1_test = train_test_split(X1, y1, test_size=0.1, random_state=90)
#            scaler.fit(pd.concat([X0[features_train[:]],X1[features_train[:]]],axis=0))
#            models_forbin['scalers'][j].append(scaler)
            binsused.append(i)
#            binsused_scalers.append(scaler)
#            valindex.append([list(X0_val.index.values),list(X1_val.index.values)])
            X0_train=X0_train.reset_index(drop=True)
            y0_train=y0_train.reset_index(drop=True)
            X0_test=X0_test.reset_index(drop=True)
            y0_test=y0_test.reset_index(drop=True)
            X1_train=X1_train.reset_index(drop=True)
            y1_train=y1_train.reset_index(drop=True)
            X1_test=X1_test.reset_index(drop=True)
            y1_test=y1_test.reset_index(drop=True)
            X_train=pd.concat([X0_train,X1_train])
            y_train=pd.concat([y0_train,y1_train])
            X_train=X_train.reset_index(drop=True)
            y_train=y_train.reset_index(drop=True)
            test0=pd.concat([X0_test,y0_test],axis=1)
            test1=pd.concat([X1_test,y1_test],axis=1)
            train0=pd.concat([X0_train,y0_train],axis=1)
            train1=pd.concat([X1_train,y1_train],axis=1)
            print("train on ",X0_train.shape[0]," background", X1_train.shape[0]," signals, test on ",X0_test.shape[0],"background",X1_test.shape[0],"signals")
#If the numbers of background and signal events are not the same, we oversample the minor one to balance the training.
            if X0_test.shape[0] > X1_test.shape[0]:
                choices=np.random.choice(np.arange(X1_test.shape[0]),X0_test.shape[0]-X1_test.shape[0])
                res_X_test=pd.concat([X0_test,X1_test,X1_test.iloc[choices,:]])
                res_y_test=pd.concat([y0_test,y1_test,y1_test.iloc[choices]])
            elif X0_test.shape[0] < X1_test.shape[0]:
                choices=np.random.choice(np.arange(X0_test.shape[0]),X1_test.shape[0]-X0_test.shape[0])
                res_X_test=pd.concat([X0_test,X1_test,X0_test.iloc[choices,:]])
                res_y_test=pd.concat([y0_test,y1_test,y0_test.iloc[choices]])
            else:
                res_X_test=pd.concat([X0_test,X1_test])
                res_y_test=pd.concat([y0_test,y1_test])
            res_X_test=res_X_test.reset_index(drop=True)
            res_y_test=res_y_test.reset_index(drop=True)
#Split the training+validation set into num_fold folds.
            kf = StratifiedKFold(n_splits=num_fold,shuffle=True)

            score_va_total=[]
            score_train_total=[]
            score_test_total=[]
#Do the k-fold training and cross validation.
            for train_index, va_index in kf.split(X_train,y_train):
                X_train_fold=X_train.iloc[train_index,:]
                y_train_fold=y_train.iloc[train_index]
                X_train_fold.reset_index(drop=True)
                y_train_fold.reset_index(drop=True)
                len_train_0=len(np.where(y_train_fold==0)[0])
                len_train_1=len(np.where(y_train_fold==1)[0])
                if len_train_0 > len_train_1:
                    choices=np.random.choice(np.where(y_train_fold==1)[0].tolist(), len_train_0-len_train_1)
                    X_train_fold=pd.concat([X_train_fold,X_train_fold.iloc[choices,:]])
                    y_train_fold=pd.concat([y_train_fold,y_train_fold.iloc[choices]])
                elif len_train_0 < len_train_1:
                    choices=np.random.choice(np.where(y_train_fold==0)[0].tolist(), len_train_1-len_train_0)
                    X_train_fold=pd.concat([X_train_fold,X_train_fold.iloc[choices,:]])
                    y_train_fold=pd.concat([y_train_fold,y_train_fold.iloc[choices]])
                X_train_fold.reset_index(drop=True)
                y_train_fold.reset_index(drop=True)

                X_val_fold  =X_train.iloc[va_index,:]
                y_val_fold  =y_train.iloc[va_index]
                X_val_fold.reset_index(drop=True)
                y_val_fold.reset_index(drop=True)
                len_val_0=len(np.where(y_val_fold==0)[0])
                len_val_1=len(np.where(y_val_fold==1)[0])
                if len_val_0 > len_val_1:
                    choices=np.random.choice(np.where(y_val_fold==1)[0].tolist(), len_val_0-len_val_1)
                    X_val_fold=pd.concat([X_val_fold,X_val_fold.iloc[choices,:]])
                    y_val_fold=pd.concat([y_val_fold,y_val_fold.iloc[choices]])
                elif len_val_0 < len_val_1:
                    choices=np.random.choice(np.where(y_val_fold==0)[0].tolist(), len_val_1-len_val_0)
                    X_val_fold=pd.concat([X_val_fold,X_val_fold.iloc[choices,:]])
                    y_val_fold=pd.concat([y_val_fold,y_val_fold.iloc[choices]])
                X_val_fold.reset_index(drop=True)
                y_val_fold.reset_index(drop=True)

                score_va=[]
                score_train=[]
                score_test=[]
#Scan the hyperparameter n_estimators, and evaluate the model performance (model "score") for training, validation and testing set
                for ii in range(20,180,20):
                    model=GradientBoostingRegressor(n_estimators=ii, learning_rate=0.3, max_features=2, max_depth=3, random_state=0)
                    model.fit(X_train_fold[features_train[:]],y_train_fold);
                    score_train.append(model.score(X_train_fold[features_train[:]],y_train_fold));
                    score_va.append(model.score(X_val_fold[features_train[:]],y_val_fold));
                    score_test.append(model.score(res_X_test[features_train[:]],res_y_test));
                score_va_total.append(score_va)
                score_train_total.append(score_train)
                score_test_total.append(score_test)
#Compute the average "score" among the folds, and also the standard deviation.
            score_va_average=[mean([score_va_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_train_average=[mean([score_train_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_test_average=[mean([score_test_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_va_std=[np.std([score_va_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_train_std=[np.std([score_train_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_test_std=[np.std([score_test_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
#Choose the n_estimator corresponding to the best validation performance.
            n_estimators=20*(score_va_average.index(max(score_va_average))+1)
            fig_nesti=methods.plot_hyperparameter(score_train_total,score_va_total,"n_estimators","parameter tuning of n_estimators--"
                                   +" bin"+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1),range(20,180,20))
            fig_nesti.savefig("hyperpara_nestimator_total_"+signaltype+"_bin"+str(i)+".png")
            fig_nesti.savefig("hyperpara_nestimator_total_"+signaltype+"_bin"+str(i)+".pdf")
            fig_nesti_average=methods.plot_hyperparameter_average(score_train_average,score_va_average,score_train_std,score_va_std,"n_estimators","parameter tuning of n_estimators--"
                                   +" bin"+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1),range(20,180,20))
            fig_nesti_average.savefig("hyperpara_nestimator_average_"+signaltype+"_bin"+str(i)+".png")
            fig_nesti_average.savefig("hyperpara_nestimator_average_"+signaltype+"_bin"+str(i)+".pdf")

            fig_nesti_average_plustest=methods.plot_hyperparameter_average_plustest(score_train_average,score_va_average,score_test_average,score_train_std,score_va_std,score_test_std,"n_estimators","parameter tuning of n_estimators--"
                                   +" bin"+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1),range(20,180,20))
            fig_nesti_average_plustest.savefig("hyperpara_nestimator_average_plustest_"+signaltype+"_bin"+str(i)+".png")
            fig_nesti_average_plustest.savefig("hyperpara_nestimator_average_plustest_"+signaltype+"_bin"+str(i)+".pdf")

            score_va_total=[]
            score_train_total=[]
            score_test_total=[]
#The same fold division and hyperparameter scan on learning rate.
            for train_index, va_index in kf.split(X_train,y_train):
                X_train_fold=X_train.iloc[train_index,:]
                y_train_fold=y_train.iloc[train_index]
                X_train_fold.reset_index(drop=True)
                y_train_fold.reset_index(drop=True)
                len_train_0=len(np.where(y_train_fold==0)[0])
                len_train_1=len(np.where(y_train_fold==1)[0])
                if len_train_0 > len_train_1:
                    choices=np.random.choice(np.where(y_train_fold==1)[0].tolist(), len_train_0-len_train_1)
                    X_train_fold=pd.concat([X_train_fold,X_train_fold.iloc[choices,:]])
                    y_train_fold=pd.concat([y_train_fold,y_train_fold.iloc[choices]])
                elif len_train_0 < len_train_1:
                    choices=np.random.choice(np.where(y_train_fold==0)[0].tolist(), len_train_1-len_train_0)
                    X_train_fold=pd.concat([X_train_fold,X_train_fold.iloc[choices,:]])
                    y_train_fold=pd.concat([y_train_fold,y_train_fold.iloc[choices]])
                X_train_fold.reset_index(drop=True)
                y_train_fold.reset_index(drop=True)

                X_val_fold  =X_train.iloc[va_index,:]
                y_val_fold  =y_train.iloc[va_index]
                X_val_fold.reset_index(drop=True)
                y_val_fold.reset_index(drop=True)
                len_val_0=len(np.where(y_val_fold==0)[0])
                len_val_1=len(np.where(y_val_fold==1)[0])
                if len_val_0 > len_val_1:
                    choices=np.random.choice(np.where(y_val_fold==1)[0].tolist(), len_val_0-len_val_1)
                    X_val_fold=pd.concat([X_val_fold,X_val_fold.iloc[choices,:]])
                    y_val_fold=pd.concat([y_val_fold,y_val_fold.iloc[choices]])
                elif len_val_0 < len_val_1:
                    choices=np.random.choice(np.where(y_val_fold==0)[0].tolist(), len_val_1-len_val_0)
                    X_val_fold=pd.concat([X_val_fold,X_val_fold.iloc[choices,:]])
                    y_val_fold=pd.concat([y_val_fold,y_val_fold.iloc[choices]])
                X_val_fold.reset_index(drop=True)
                y_val_fold.reset_index(drop=True)

                score_va=[]
                score_train=[]
                score_test=[]
                for ii in np.arange(0.1,0.9,0.1):
                    model=GradientBoostingRegressor(n_estimators=n_estimators, learning_rate=ii, max_features=2, max_depth=3, random_state=0)
                    model.fit(X_train_fold[features_train[:]],y_train_fold);
                    score_train.append(model.score(X_train_fold[features_train[:]],y_train_fold));
                    score_va.append(model.score(X_val_fold[features_train[:]],y_val_fold));
                    score_test.append(model.score(res_X_test[features_train[:]],res_y_test));
                score_va_total.append(score_va)
                score_train_total.append(score_train)
                score_test_total.append(score_test)
            score_va_average=[mean([score_va_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_train_average=[mean([score_train_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_test_average=[mean([score_test_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_va_std=[np.std([score_va_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_train_std=[np.std([score_train_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_test_std=[np.std([score_test_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            learning_rate=0.1*(score_va_average.index(max(score_va_average))+1)

            fig_lr=methods.plot_hyperparameter(score_train_total,score_va_total,"learning rate","parameter tuning of learning rate--"
                                   +" bin"+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1),np.arange(0.1,0.9,0.1))
            fig_lr.savefig("hyperpara_lr_total_"+signaltype+"_bin"+str(i)+".png")
            fig_lr.savefig("hyperpara_lr_total_"+signaltype+"_bin"+str(i)+".pdf")
            fig_lr_average=methods.plot_hyperparameter_average(score_train_average,score_va_average,score_train_std,score_va_std,"learning rate","parameter tuning of learning rate--"
                                   +" bin"+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1),np.arange(0.1,0.9,0.1))
            fig_lr_average.savefig("hyperpara_lr_average_"+signaltype+"_bin"+str(i)+".png")
            fig_lr_average.savefig("hyperpara_lr_average_"+signaltype+"_bin"+str(i)+".pdf")
            fig_lr_average_plustest=methods.plot_hyperparameter_average_plustest(score_train_average,score_va_average,score_test_average,score_train_std,score_va_std,score_test_std,"learning rate","parameter tuning of learning rate--"
                                   +" bin"+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1),np.arange(0.1,0.9,0.1))
            fig_lr_average_plustest.savefig("hyperpara_lr_average_plustest_"+signaltype+"_bin"+str(i)+".png")
            fig_lr_average_plustest.savefig("hyperpara_lr_average_plustest_"+signaltype+"_bin"+str(i)+".pdf")


            models_scan=[]
            score_va_total=[]
            score_train_total=[]
            score_test_total=[]
#The fold spliting and hyperparamter scan on max_depth
            for train_index, va_index in kf.split(X_train,y_train):
                X_train_fold=X_train.iloc[train_index,:]
                y_train_fold=y_train.iloc[train_index]
                X_train_fold.reset_index(drop=True)
                y_train_fold.reset_index(drop=True)
                len_train_0=len(np.where(y_train_fold==0)[0])
                len_train_1=len(np.where(y_train_fold==1)[0])
                if len_train_0 > len_train_1:
                    choices=np.random.choice(np.where(y_train_fold==1)[0].tolist(), len_train_0-len_train_1)
                    X_train_fold=pd.concat([X_train_fold,X_train_fold.iloc[choices,:]])
                    y_train_fold=pd.concat([y_train_fold,y_train_fold.iloc[choices]])
                elif len_train_0 < len_train_1:
                    choices=np.random.choice(np.where(y_train_fold==0)[0].tolist(), len_train_1-len_train_0)
                    X_train_fold=pd.concat([X_train_fold,X_train_fold.iloc[choices,:]])
                    y_train_fold=pd.concat([y_train_fold,y_train_fold.iloc[choices]])
                X_train_fold.reset_index(drop=True)
                y_train_fold.reset_index(drop=True)

                X_val_fold  =X_train.iloc[va_index,:]
                y_val_fold  =y_train.iloc[va_index]
                X_val_fold.reset_index(drop=True)
                y_val_fold.reset_index(drop=True)
                len_val_0=len(np.where(y_val_fold==0)[0])
                len_val_1=len(np.where(y_val_fold==1)[0])
                if len_val_0 > len_val_1:
                    choices=np.random.choice(np.where(y_val_fold==1)[0].tolist(), len_val_0-len_val_1)
                    X_val_fold=pd.concat([X_val_fold,X_val_fold.iloc[choices,:]])
                    y_val_fold=pd.concat([y_val_fold,y_val_fold.iloc[choices]])
                elif len_val_0 < len_val_1:
                    choices=np.random.choice(np.where(y_val_fold==0)[0].tolist(), len_val_1-len_val_0)
                    X_val_fold=pd.concat([X_val_fold,X_val_fold.iloc[choices,:]])
                    y_val_fold=pd.concat([y_val_fold,y_val_fold.iloc[choices]])
                X_val_fold.reset_index(drop=True)
                y_val_fold.reset_index(drop=True)

                score_va=[]
                score_train=[]
                score_test=[]
                models=[]
                for ii in range(2,10):
                    model=GradientBoostingRegressor(n_estimators=n_estimators, learning_rate=learning_rate, max_features=2, max_depth=ii, random_state=0)
                    model.fit(X_train_fold[features_train[:]],y_train_fold);
                    models.append(model)
                    score_train.append(model.score(X_train_fold[features_train[:]],y_train_fold));
                    score_va.append(model.score(X_val_fold[features_train[:]],y_val_fold));
                    score_test.append(model.score(res_X_test[features_train[:]],res_y_test));
                score_va_total.append(score_va)
                score_train_total.append(score_train)
                score_test_total.append(score_test)
                models_scan.append(models)
            score_va_average=[mean([score_va_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_train_average=[mean([score_train_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_test_average=[mean([score_test_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_va_std=[np.std([score_va_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_train_std=[np.std([score_train_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            score_test_std=[np.std([score_test_total[foldindex][ii] for foldindex in range(num_fold)]) for ii in range(8)]
            bestmodels=[models_scan[foldindex][score_va_average.index(max(score_va_average))] for foldindex in range(num_fold)]

            fig_dep=methods.plot_hyperparameter(score_train_total,score_va_total,"max_depth","parameter tuning of max_depth--"
                                   +" bin"+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1),range(2,10))
            fig_dep.savefig("hyperpara_maxdepth_total_"+signaltype+"_bin"+str(i)+".png")
            fig_dep.savefig("hyperpara_maxdepth_total_"+signaltype+"_bin"+str(i)+".pdf")
            fig_dep_average=methods.plot_hyperparameter_average(score_train_average,score_va_average,score_train_std,score_va_std,"max_depth","parameter tuning of max_depth--"
                                   +" bin"+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1),range(2,10))
            fig_dep_average.savefig("hyperpara_maxdepth_average_"+signaltype+"_bin"+str(i)+".png")
            fig_dep_average.savefig("hyperpara_maxdepth_average_"+signaltype+"_bin"+str(i)+".pdf")
            fig_dep_average_plustest=methods.plot_hyperparameter_average_plustest(score_train_average,score_va_average,score_test_average,score_train_std,score_va_std,score_test_std,"max_depth","parameter tuning of max_depth--"
                                   +" bin"+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1),range(2,10))
            fig_dep_average_plustest.savefig("hyperpara_maxdepth_average_plustest_"+signaltype+"_bin"+str(i)+".png")
            fig_dep_average_plustest.savefig("hyperpara_maxdepth_average_plustest_"+signaltype+"_bin"+str(i)+".pdf")
#Save the models with the best validation performance
            for foldindex in range(num_fold):
                dump(bestmodels[foldindex],"model_"+signaltype+"_bin"+str(i)+"_"+str(foldindex)+".joblib")
#            print("plotting history")
#            histroyplot=methods.plot_metrics(model_history,'learning curve in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
#                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
#                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
#            histroyplot.savefig("learning_curve_"+signaltype+"_bin"+str(i)+".png")
#            histroyplot.savefig("learning_curve_"+signaltype+"_bin"+str(i)+".pdf")
#            histroyplot.savefig("learning_curve_bin"+str(i)+".eps")

            print("plotting score distribution")
 #           model.load_weights(filepath)
            test0[b'MLscore'] = np.mean([bestmodels[foldindex].predict(test0[features_train[:]]) for foldindex in range(num_fold)],axis=0)
            test1[b'MLscore'] = np.mean([bestmodels[foldindex].predict(test1[features_train[:]]) for foldindex in range(num_fold)],axis=0)
            train0[b'MLscore']= np.mean([bestmodels[foldindex].predict(train0[features_train[:]]) for foldindex in range(num_fold)],axis=0)
            train1[b'MLscore']= np.mean([bestmodels[foldindex].predict(train1[features_train[:]]) for foldindex in range(num_fold)],axis=0)
            databkg_test_bin[i][b'MLscore']=np.mean([bestmodels[foldindex].predict(databkg_test_bin[i][features_train[:]]) for foldindex in range(num_fold)],axis=0)
            datasig_test_bin[i][b'MLscore']=np.mean([bestmodels[foldindex].predict(datasig_test_bin[i][features_train[:]]) for foldindex in range(num_fold)],axis=0)
            llepMC[i][b'MLscore']=np.mean([bestmodels[foldindex].predict(llepMC[i][features_train[:]]) for foldindex in range(num_fold)],axis=0)

            if dozllmerge:
                for mergebinindex in mergebin:
                    if zllMC[mergebinindex].shape[0]!=0:
                        zllMC[mergebinindex][b'MLscore']  = np.mean([bestmodels[foldindex].predict(zllMC[mergebinindex][features_zllCR[:]]) for foldindex in range(num_fold)],axis=0)
                    if zlldata[mergebinindex].shape[0]!=0:
                        zlldata[mergebinindex][b'MLscore']= np.mean([bestmodels[foldindex].predict(zlldata[mergebinindex][features_zllCR[:]]) for foldindex in range(num_fold)],axis=0)
            else:
                zllMC[i][b'MLscore']  = np.mean([bestmodels[foldindex].predict(zllMC[i][features_zllCR[:]]) for foldindex in range(num_fold)],axis=0)
                zlldata[i][b'MLscore']= np.mean([bestmodels[foldindex].predict(zlldata[i][features_zllCR[:]]) for foldindex in range(num_fold)],axis=0)
            llepdata[i][b'MLscore']=np.mean([bestmodels[foldindex].predict(llepdata[i][features_train[:]]) for foldindex in range(num_fold)],axis=0)
            figcompare = methods.plot_train_val_compare(train0,train1,test0,test1,'training and test comparison in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figcompare.savefig("train_val_compare_"+signaltype+"_bin"+str(i)+".png")
            figcompare.savefig("train_val_compare_"+signaltype+"_bin"+str(i)+".pdf")
 #           figcompare.savefig("train_val_compare_bin"+str(i)+".eps")

            figtest = methods.plot_sig_bkg_compare(datasig_test_bin[i],databkg_test_bin[i],m1,m2,'signal and background comparison in bin '
                       +str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figtest.savefig("sig_bkg_compare_"+signaltype+"_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".png")
            figtest.savefig("sig_bkg_compare_"+signaltype+"_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".pdf")
#            figtest.savefig("sig_bkg_compare_bin"+str(i)+".eps")

            fig_mt2scorecorr=methods.plot_score_mt2_corr(datasig_test_bin[i],databkg_test_bin[i],'MT2 score correlation check (test set) \nin bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            fig_mt2scorecorr.savefig("mt2scorecorr_"+signaltype+"_bin"+str(i)+".png")
            fig_mt2scorecorr.savefig("mt2scorecorr_"+signaltype+"_bin"+str(i)+".pdf")
            models_forbin['mt2_score_corr'][j].append([np.corrcoef(databkg_test_bin[i][b'mt2'],databkg_test_bin[i][b'MLscore'])[0][1],np.corrcoef(datasig_test_bin[i][b'mt2'],datasig_test_bin[i][b'MLscore'])[0][1]])
            figllep=methods.plot_CR_llep(llepMC[i],llepdata[i],'data MC comparison of lost lepton CR in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figllep.savefig("llepCR_"+signaltype+"_bin"+str(i)+".png")
            figllep.savefig("llepCR_"+signaltype+"_bin"+str(i)+".pdf")
#            figllep.savefig("llepCR_bin"+str(i)+".eps")
           
#            if zlldata[i].shape[0]*zllMC[i].shape[0]:
            if dozllmerge:
                figzll=methods.plot_CR_zll(pd.concat([zllMC[mergebinindex] for mergebinindex in mergebin],axis=0),
                       pd.concat([zlldata[mergebinindex] for mergebinindex in mergebin],axis=0),'data MC comparison of zll CR in bin '+str(i)+' zll_ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            else:
                figzll=methods.plot_CR_zll(zllMC[i],zlldata[i],'data MC comparison of zll CR in bin '+str(i)+' zll_ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figzll.savefig("zllCR_"+signaltype+"_bin"+str(i)+".png")
            figzll.savefig("zllCR_"+signaltype+"_bin"+str(i)+".pdf")
#            figzll.savefig("zllCR_bin"+str(i)+".eps")


        else:
            if dozllmerge:
                bestmodels=[load("model_"+signaltype+"_bin"+str(i)+"_"+str(foldindex)+".joblib") for foldindex in range(num_fold)]
                for mergebinindex in mergebin:
                    if zllMC[mergebinindex].shape[0]!=0:
                        zllMC[mergebinindex][b'MLscore']=np.mean([bestmodels[foldindex].predict(zllMC[mergebinindex][features_zllCR[:]]) for foldindex in range(num_fold)],axis=0)
                    if zlldata[mergebinindex].shape[0]!=0:
                        zlldata[mergebinindex][b'MLscore']=np.mean([bestmodels[foldindex].predict(zlldata[mergebinindex][features_zllCR[:]]) for foldindex in range(num_fold)],axis=0)
            figtest = methods.plot_sig_bkg_compare(datasig_test_bin[i],databkg_test_bin[i],m1,m2,'signal and background comparison(test set) in bin '
                       +str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figtest.savefig("sig_bkg_compare_"+signaltype+"_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".png")
            figtest.savefig("sig_bkg_compare_"+signaltype+"_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".pdf")
            models_forbin['mt2_score_corr'][j].append([np.corrcoef(databkg_test_bin[i][b'mt2'],databkg_test_bin[i][b'MLscore'])[0][1],np.corrcoef(datasig_test_bin[i][b'mt2'],datasig_test_bin[i][b'MLscore'])[0][1]])
#            models_forbin['scalers'][j].append(binsused_scalers[binsused.index(i)])
#            filepath="weights_T1bbbb_bin_"+str(i)+".best.hdf5"
#            model=keras.models.load_model(filepath)
#            valindex_thisbin=valindex[binsused.index(i)]
#            X_val=pd.concat([X0.loc[valindex_thisbin[0]],X1.loc[valindex_thisbin[1]]])
#            y_val=pd.concat([y0.loc[valindex_thisbin[0]],y1.loc[valindex_thisbin[1]]])
#            val=pd.concat([X_val,y_val],axis=1)
#            val[b'MLscore']=model.predict(val[features_train[:]], batch_size=20000)

#        if zlldata[i].shape[0]*zllMC[i].shape[0]:

#Scan the cut position and plot significance wrt cut.
        if dozllmerge:
            midsig=methods.computesignificance(databkg_test_bin[i],datasig_test_bin[i],pd.concat([zllMC[mergebinindex] for mergebinindex in mergebin],axis=0),llepMC[i],
                                           pd.concat([zlldata[mergebinindex] for mergebinindex in mergebin],axis=0),llepdata[i],m1,m2,1-MLuse_bkg[i],1-MLuse_sig[i],0.005,usedataexp)
        else:
            midsig=methods.computesignificance(databkg_test_bin[i],datasig_test_bin[i],zllMC[i],llepMC[i],
                                           zlldata[i],llepdata[i],m1,m2,1-MLuse_bkg[i],1-MLuse_sig[i],0.005,usedataexp)
        models_forbin['significance'][j].append(midsig)
        (zlldataCR,llepdataCR)=methods.countCRdata(zlldata[i],llepdata[i],interval=0.005)
        models_forbin['zlldataCR'][j].append(zlldataCR)
        models_forbin['llepdataCR'][j].append(llepdataCR)
        print("plotting significance")
        
        figmidsig=methods.plot_significance(midsig,'significance change of mass '+str(m1)+" "+str(m2) +" with cut--bin"
                       +str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
 
        figmidsig.savefig("significance_"+signaltype+"_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".png")
        figmidsig.savefig("significance_"+signaltype+"_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".pdf")
#        figmidsig.savefig("significance_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".eps")
#        else: 
#            midsig=[0]
#            models_forbin['significance'][j].append(midsig)
        print("finished plotting")
np.save("significance_"+signaltype+"_output.npy",models_forbin)
