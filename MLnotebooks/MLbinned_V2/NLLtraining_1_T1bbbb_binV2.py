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
import binning
import loaddata
import methods
import initialize
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

#features_exclude=[b'nJet30',b'nJet40',b'nBJet20',b'nBJet30',b'nBJet40',b'ht',b'mt2',b'mht_pt',b'met_pt',b'mht_phi',b'met_phi',b'njet']

bkg=loaddata.loadbkg()
sig=loaddata.loadsig(massskim=massskim)
llepMC=loaddata.loadcontrol_MC(controltype="llep")
zllMC=loaddata.loadcontrol_MC(controltype="zll")
llepdata=loaddata.loadcontrol_data(controltype="llep")
zlldata=loaddata.loadcontrol_data(controltype="zll")
print("dataset prepared")

models_forbin=initialize.models_forbin

(databkg_ML_bin,databkg_test_bin,MLuse_bkg,MLusetag_bkg)=initialize.spliting(bkg)
del bkg
(datasig_ML_bin,datasig_test_bin,MLuse_sig,MLusetag_sig)=initialize.spliting(sig)
del sig

print("dataset sampled")
binsused=[]
#valindex=[]
for j in range(len(models_forbin['mass'])):
    m1=models_forbin['mass'][j][0]
    m2=models_forbin['mass'][j][1]
    print("for mass1= ",m1,", mass2= ",m2)
    models_forbin['significance'].append([])
    models_forbin['traintag'].append([])
    models_forbin['trainfraction'].append([])
#    models_forbin['trainplots'].append([])
#    models_forbin['train_val_plots'].append([])
#    models_forbin['bkg_sig_plots'].append([])
#    models_forbin['CR_plots'].append([])
#    models_forbin['significance_plots'].append([])
    for i in models_forbin['binindex'][j]:
        features_train=methods.feature_choice(min(binning.selections[i][1][1],16)-1)
        X0=databkg_ML_bin[i][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]]
        y0=databkg_ML_bin[i][b'sigtag']
        X1=datasig_ML_bin[i][(features_train+[b'evt_scale1fb',b'GenSusyMScan1',b'GenSusyMScan2',b'absweight',b'bkgtype',b'w2'])[:]]
        y1=datasig_ML_bin[i][b'sigtag']
        models_forbin['traintag'][j].append([MLusetag_bkg[i],MLusetag_sig[i]])
        models_forbin['trainfraction'][j].append([MLuse_bkg[i],MLuse_sig[i]])
        if i not in binsused:
            X0_train, X0_val, y0_train, y0_val = train_test_split(X0, y0, test_size=0.1, random_state=42)
            X1_train, X1_val, y1_train, y1_val = train_test_split(X1, y1, test_size=0.1, random_state=90)
            binsused.append(i)
#            valindex.append([list(X0_val.index.values),list(X1_val.index.values)])
            X0_train=X0_train.reset_index(drop=True)
            y0_train=y0_train.reset_index(drop=True)
            X0_val=X0_val.reset_index(drop=True)
            y0_val=y0_val.reset_index(drop=True)
            X1_train=X1_train.reset_index(drop=True)
            y1_train=y1_train.reset_index(drop=True)
            X1_val=X1_val.reset_index(drop=True)
            y1_val=y1_val.reset_index(drop=True)
            val0=pd.concat([X0_val,y0_val],axis=1)
            val1=pd.concat([X1_val,y1_val],axis=1)
            train0=pd.concat([X0_train,y0_train],axis=1)
            train1=pd.concat([X1_train,y1_train],axis=1)
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
            histroyplot=methods.plot_metrics(model_history,'learning curve in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            histroyplot.savefig("learning_curve_bin"+str(i)+".png")
            histroyplot.savefig("learning_curve_bin"+str(i)+".pdf")
#            histroyplot.savefig("learning_curve_bin"+str(i)+".eps")

            print("plotting score distribution")
            model.load_weights(filepath)
            val0[b'MLscore']=model.predict(val0[features_train[:]], batch_size=20000)
            val1[b'MLscore']=model.predict(val1[features_train[:]], batch_size=20000)
            train0[b'MLscore']=model.predict(train0[features_train[:]], batch_size=20000)
            train1[b'MLscore']=model.predict(train1[features_train[:]], batch_size=20000)
            databkg_test_bin[i][b'MLscore']=model.predict(databkg_test_bin[i][features_train[:]], batch_size=20000)
            datasig_test_bin[i][b'MLscore']=model.predict(datasig_test_bin[i][features_train[:]], batch_size=20000)
            llepMC[i][b'MLscore']=model.predict(llepMC[i][features_train[:]], batch_size=20000)
            zllMC[i][b'MLscore']=model.predict(zllMC[i][features_train[:]], batch_size=20000)
            llepdata[i][b'MLscore']=model.predict(llepdata[i][features_train[:]], batch_size=20000)
            zlldata[i][b'MLscore']=model.predict(zlldata[i][features_train[:]], batch_size=20000)

            figcompare = methods.plot_train_val_compare(train0,train1,val0,val1,'train and validation comparison in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figcompare.savefig("train_val_compare_bin"+str(i)+".png")
            figcompare.savefig("train_val_compare_bin"+str(i)+".pdf")
 #           figcompare.savefig("train_val_compare_bin"+str(i)+".eps")

            figtest = methods.plot_sig_bkg_compare(datasig_test_bin[i],databkg_test_bin[i],m1,m2,'signal and background comparison(test set) in bin '
                       +str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figtest.savefig("sig_bkg_compare_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".png")
            figtest.savefig("sig_bkg_compare_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".pdf")
#            figtest.savefig("sig_bkg_compare_bin"+str(i)+".eps")

            figllep=methods.plot_CR_llep(llepMC[i],llepdata[i],'data MC comparison of lost lepton CR in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figllep.savefig("llepCR_bin"+str(i)+".png")
            figllep.savefig("llepCR_bin"+str(i)+".pdf")
#            figllep.savefig("llepCR_bin"+str(i)+".eps")

            figzll=methods.plot_CR_zll(zllMC[i],zlldata[i],'data MC comparison of zll CR in bin '+str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figzll.savefig("zllCR_bin"+str(i)+".png")
            figzll.savefig("zllCR_bin"+str(i)+".pdf")
#            figzll.savefig("zllCR_bin"+str(i)+".eps")


        else:
            figtest = methods.plot_sig_bkg_compare(datasig_test_bin[i],databkg_test_bin[i],m1,m2,'signal and background comparison(test set) in bin '
                       +str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
            figtest.savefig("sig_bkg_compare_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".png")
            figtest.savefig("sig_bkg_compare_bin"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".pdf")
#            filepath="weights_T1bbbb_bin_"+str(i)+".best.hdf5"
#            model=keras.models.load_model(filepath)
#            valindex_thisbin=valindex[binsused.index(i)]
#            X_val=pd.concat([X0.loc[valindex_thisbin[0]],X1.loc[valindex_thisbin[1]]])
#            y_val=pd.concat([y0.loc[valindex_thisbin[0]],y1.loc[valindex_thisbin[1]]])
#            val=pd.concat([X_val,y_val],axis=1)
#            val[b'MLscore']=model.predict(val[features_train[:]], batch_size=20000)


        midsig=methods.computesignificance(databkg_test_bin[i],datasig_test_bin[i],zllMC[i],llepMC[i],
                                           zlldata[i],llepdata[i],m1,m2,1-MLuse_bkg[i],1-MLuse_sig[i],0.005)
        models_forbin['significance'][j].append(midsig)
        print("plotting significance")
        figmidsig=methods.plot_significance(midsig,'significance change of mass '+str(m1)+" "+str(m2) +" with cut--bin"
                       +str(i)+' ht '+str(binning.selections[i][0][0])
                       +"-"+str(binning.selections[i][0][1])+" njet "+str(binning.selections[i][1][0])+"-"+str(binning.selections[i][1][1]-1)
                       +" nbjet "+str(binning.selections[i][2][0])+"-"+str(binning.selections[i][2][1]-1))
 
        figmidsig.savefig("significance_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".png")
        figmidsig.savefig("significance_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".pdf")
#        figmidsig.savefig("significance_bin_"+str(i)+"_mass_"+str(m1)+"_"+str(m2)+".eps")
        print("finished plotting")
np.save("significance_output.npy",models_forbin)
