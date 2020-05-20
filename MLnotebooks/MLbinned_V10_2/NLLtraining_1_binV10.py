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
import statistics
from math import sqrt
import binning
import loaddata
import methods
import initialize
from sklearn.preprocessing import StandardScaler
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


massskim=True
usehalfspliting=True
dozllmerge=True
usedataexp=False


signaltype=str(sys.argv[1])
if signaltype!="T1bbbb" and signaltype!="T1qqqq" and signaltype!="T2bb" and signaltype!="T2qq":
    print("wrong signal type, please input T1bbbb or T1qqqq or T2bb or T2qq")
    sys.exit(0)
#features_exclude=[b'nJet30',b'nJet40',b'nBJet20',b'nBJet30',b'nBJet40',b'ht',b'mt2',b'mht_pt',b'met_pt',b'mht_phi',b'met_phi',b'njet']

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
binsused_scalers=[]
#valindex=[]
for j in range(len(models_forbin['mass'])):
    m1=models_forbin['mass'][j][0]
    m2=models_forbin['mass'][j][1]
    print("for mass1= ",m1,", mass2= ",m2)
    models_forbin['significance'].append([])
    models_forbin['traintag'].append([])
    models_forbin['trainfraction'].append([])
    models_forbin['mt2_score_corr'].append([])
    models_forbin['scalers'].append([])
    models_forbin['zlldataCR'].append([])
    models_forbin['llepdataCR'].append([])
#    models_forbin['trainplots'].append([])
#    models_forbin['train_val_plots'].append([])
#    models_forbin['bkg_sig_plots'].append([])
#    models_forbin['CR_plots'].append([])
#    models_forbin['significance_plots'].append([])
    for i in models_forbin['binindex'][j]:
        tf.random.set_seed(2)
        features_train=methods.feature_choice(min(binning.selections[i][1][1],16)-1)
        features_zllCR=methods.feature_choice_zll(min(binning.selections[i][1][1],16)-1)
        scaler = StandardScaler()
        X0=databkg_ML_bin[i][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]]
        y0=databkg_ML_bin[i][b'sigtag']
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
            X0_train_val, X0_test, y0_train_val, y0_test = train_test_split(X0, y0, test_size=0.1, random_state=42)
            X1_train_val, X1_test, y1_train_val, y1_test = train_test_split(X1, y1, test_size=0.1, random_state=90)
            X0_train, X0_val, y0_train, y0_val = train_test_split(X0_train_val, y0_train_val, test_size=1./9., random_state=42)
            X1_train, X1_val, y1_train, y1_val = train_test_split(X1_train_val, y1_train_val, test_size=1./9., random_state=90)
            scaler.fit(pd.concat([X0[features_train[:]],X1[features_train[:]]],axis=0))
            models_forbin['scalers'][j].append(scaler)
            binsused.append(i)
            binsused_scalers.append(scaler)
#            valindex.append([list(X0_val.index.values),list(X1_val.index.values)])
            X0_train=X0_train.reset_index(drop=True)
            y0_train=y0_train.reset_index(drop=True)
            X0_val=X0_val.reset_index(drop=True)
            y0_val=y0_val.reset_index(drop=True)
            X0_test=X0_test.reset_index(drop=True)
            y0_test=y0_test.reset_index(drop=True)
            X1_train=X1_train.reset_index(drop=True)
            y1_train=y1_train.reset_index(drop=True)
            X1_val=X1_val.reset_index(drop=True)
            y1_val=y1_val.reset_index(drop=True)
            X1_test=X1_test.reset_index(drop=True)
            y1_test=y1_test.reset_index(drop=True)
            val0=pd.concat([X0_val,y0_val],axis=1)
            val1=pd.concat([X1_val,y1_val],axis=1)
            train0=pd.concat([X0_train,y0_train],axis=1)
            train1=pd.concat([X1_train,y1_train],axis=1)
            test0=pd.concat([X0_test,y0_test],axis=1)
            test1=pd.concat([X1_test,y1_test],axis=1)
            print("train on ",X0_train.shape[0]," background", X1_train.shape[0]," signals, validate on ",X0_val.shape[0],"background",X1_val.shape[0],"signals",", test on ",X0_test.shape[0]," background ",X1_test.shape[0]," signal")
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
                     keras.layers.BatchNormalization(axis=-1, momentum=0.2, epsilon=0.001, 
                                                     center=True, scale=True, beta_initializer='zeros', gamma_initializer='ones', moving_mean_initializer='zeros',
                                                     moving_variance_initializer='ones', beta_regularizer=None, gamma_regularizer=None, beta_constraint=None,
                                                     gamma_constraint=None),
                     keras.layers.Dense(
                     10, kernel_initializer='normal', activation='sigmoid'),
                     keras.layers.BatchNormalization(axis=-1, momentum=0.2, epsilon=0.0001, 
                                                     center=True, scale=True, beta_initializer='zeros', gamma_initializer='ones', moving_mean_initializer='zeros',
                                                     moving_variance_initializer='ones', beta_regularizer=None, gamma_regularizer=None, beta_constraint=None,
                                                     gamma_constraint=None),
                     keras.layers.Dense(
                     10, kernel_initializer='normal', activation='sigmoid'),
                      keras.layers.Dense(1, activation='sigmoid',
                         bias_initializer=output_bias),
                ])
                adam=optimizers.Adam(lr=0.0001)
                model.compile(
                     optimizer='Adam',
                    loss='binary_crossentropy',
                     metrics=metrics)
                return model

            filepath="weights_"+signaltype+"_bin_"+str(i)+".best.hdf5"
            checkpoint = ModelCheckpoint(filepath, monitor='val_accuracy', verbose=1, save_best_only=True, mode='max')
            callbacks_list = [checkpoint]
            model = make_model()
            print("start training on bin ",i)
            model_history=model.fit(x=pd.DataFrame(scaler.transform(res_X_train[features_train[:]])),
            y=res_y_train,
            batch_size=20000,
            epochs=200,
            verbose=0,
            callbacks = callbacks_list,
            validation_data=(pd.DataFrame(scaler.transform(res_X_val[features_train[:]])), res_y_val))
            print("plotting history")
            histroyplot=methods.plot_metrics(model_history,'learning curve in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            histroyplot.savefig("learning_curve_"+signaltype+"_bin"+str(i)+".png")
            histroyplot.savefig("learning_curve_"+signaltype+"_bin"+str(i)+".pdf")
            histroyplot=methods.plot_metrics_loss(model_history,'learning curve (loss change) in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            histroyplot.savefig("learning_curve_loss_"+signaltype+"_bin"+str(i)+".png")
            histroyplot.savefig("learning_curve_loss_"+signaltype+"_bin"+str(i)+".pdf")
            histroyplot=methods.plot_metrics_accuracy(model_history,'learning curve (accuracy change) in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            histroyplot.savefig("learning_curve_accuracy_"+signaltype+"_bin"+str(i)+".png")
            histroyplot.savefig("learning_curve_accuracy_"+signaltype+"_bin"+str(i)+".pdf")
#            histroyplot.savefig("learning_curve_bin"+str(i)+".eps")

            print("plotting score distribution")
            model.load_weights(filepath)
            val0[b'MLscore']=model.predict(scaler.transform(val0[features_train[:]]), batch_size=20000)
            val1[b'MLscore']=model.predict(scaler.transform(val1[features_train[:]]), batch_size=20000)
            train0[b'MLscore']=model.predict(scaler.transform(train0[features_train[:]]), batch_size=20000)
            train1[b'MLscore']=model.predict(scaler.transform(train1[features_train[:]]), batch_size=20000)
            test0[b'MLscore']=model.predict(scaler.transform(test0[features_train[:]]), batch_size=20000)
            test1[b'MLscore']=model.predict(scaler.transform(test1[features_train[:]]), batch_size=20000)
            databkg_test_bin[i][b'MLscore']=model.predict(scaler.transform(databkg_test_bin[i][features_train[:]]), batch_size=20000)
            datasig_test_bin[i][b'MLscore']=model.predict(scaler.transform(datasig_test_bin[i][features_train[:]]), batch_size=20000)
            llepMC[i][b'MLscore']=model.predict(scaler.transform(llepMC[i][features_train[:]]), batch_size=20000)

            if dozllmerge:
                for mergebinindex in mergebin:
                    if zllMC[mergebinindex].shape[0]!=0:
                        zllMC[mergebinindex][b'MLscore']=model.predict(scaler.transform(zllMC[mergebinindex][features_zllCR[:]]), batch_size=20000)
                    if zlldata[mergebinindex].shape[0]!=0:
                        zlldata[mergebinindex][b'MLscore']=model.predict(scaler.transform(zlldata[mergebinindex][features_zllCR[:]]), batch_size=20000)
            else:
                zllMC[i][b'MLscore']=model.predict(scaler.transform(zllMC[i][features_zllCR[:]]), batch_size=20000)
                zlldata[i][b'MLscore']=model.predict(scaler.transform(zlldata[i][features_zllCR[:]]), batch_size=20000)
            llepdata[i][b'MLscore']=model.predict(scaler.transform(llepdata[i][features_train[:]]), batch_size=20000)
            figcompare = methods.plot_train_val_test_compare(train0,train1,val0,val1,test0,test1,'training validation and test comparison in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
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

            fig_mt2scorecorr=methods.plot_score_mt2_corr(datasig_test_bin[i],databkg_test_bin[i],'MT2 score correlation check \nin bin '+str(i)+' ht '+str(binning.selections[i][0][0])
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
                filepath="weights_"+signaltype+"_bin_"+str(i)+".best.hdf5"
                model = keras.models.load_model(filepath)
                scaler=binsused_scalers[binsused.index(i)]
                for mergebinindex in mergebin:
                    if zllMC[mergebinindex].shape[0]!=0:
                        zllMC[mergebinindex][b'MLscore']=model.predict(scaler.transform(zllMC[mergebinindex][features_zllCR[:]]), batch_size=20000)
                    if zlldata[mergebinindex].shape[0]!=0:
                        zlldata[mergebinindex][b'MLscore']=model.predict(scaler.transform(zlldata[mergebinindex][features_zllCR[:]]), batch_size=20000)
            figtest = methods.plot_sig_bkg_compare(datasig_test_bin[i],databkg_test_bin[i],m1,m2,'signal and background comparison in bin '
                       +str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figtest.savefig("sig_bkg_compare_"+signaltype+"_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".png")
            figtest.savefig("sig_bkg_compare_"+signaltype+"_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".pdf")
            models_forbin['mt2_score_corr'][j].append([np.corrcoef(databkg_test_bin[i][b'mt2'],databkg_test_bin[i][b'MLscore'])[0][1],np.corrcoef(datasig_test_bin[i][b'mt2'],datasig_test_bin[i][b'MLscore'])[0][1]])
            models_forbin['scalers'][j].append(binsused_scalers[binsused.index(i)])
#            filepath="weights_T1bbbb_bin_"+str(i)+".best.hdf5"
#            model=keras.models.load_model(filepath)
#            valindex_thisbin=valindex[binsused.index(i)]
#            X_val=pd.concat([X0.loc[valindex_thisbin[0]],X1.loc[valindex_thisbin[1]]])
#            y_val=pd.concat([y0.loc[valindex_thisbin[0]],y1.loc[valindex_thisbin[1]]])
#            val=pd.concat([X_val,y_val],axis=1)
#            val[b'MLscore']=model.predict(val[features_train[:]], batch_size=20000)

#        if zlldata[i].shape[0]*zllMC[i].shape[0]:
        if dozllmerge:
            midsig=methods.computesignificance(databkg_test_bin[i],datasig_test_bin[i],pd.concat([zllMC[mergebinindex] for mergebinindex in mergebin],axis=0),llepMC[i],
                                           pd.concat([zlldata[mergebinindex] for mergebinindex in mergebin],axis=0),llepdata[i],m1,m2,1-MLuse_bkg[i],1-MLuse_sig[i],0.005,usedataexp)
        else:
            midsig=methods.computesignificance(databkg_test_bin[i],datasig_test_bin[i],zllMC[i],llepMC[i],
                                           zlldata[i],llepdata[i],m1,m2,1-MLuse_bkg[i],1-MLuse_sig[i],0.005,usedataexp)
        models_forbin['significance'][j].append(midsig)
        print("plotting significance")
        
        figmidsig=methods.plot_significance(midsig,'significance change of mass '+str(m1)+" "+str(m2) +" with cut--bin"
                       +str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
 
        figmidsig.savefig("significance_"+signaltype+"_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".png")
        figmidsig.savefig("significance_"+signaltype+"_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".pdf")
        (zlldataCR,llepdataCR)=methods.countCRdata(zlldata[i],llepdata[i],interval=0.005)
        models_forbin['zlldataCR'][j].append(zlldataCR)
        models_forbin['llepdataCR'][j].append(llepdataCR)
#        figmidsig.savefig("significance_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".eps")
#        else: 
#            midsig=[0]
#            models_forbin['significance'][j].append(midsig)
        print("finished plotting")
np.save("significance_"+signaltype+"_output.npy",models_forbin)
