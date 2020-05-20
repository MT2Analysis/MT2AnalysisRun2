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
np.seterr(all='ignore')
llepMC=loaddata.loadcontrol_MC(controltype="llep")
zllMC=loaddata.loadcontrol_MC(controltype="zll")
llepdata=loaddata.loadcontrol_data(controltype="llep")
zlldata=loaddata.loadcontrol_data(controltype="zll")
bkg=loaddata.loadbkg()
num_fold=9
signals=['T1bbbb','T1qqqq','T2bb','T2qq']
usedataexp=False
for signaltype in signals:
    d=np.load("significance_"+signaltype+"_output.npy",allow_pickle=True)
    dataset=d.item()
    bin_used=[]
    sig=loaddata.loadsig(massskim=True,sig=signaltype)
    for i in range(len(dataset['mass'])):
        for index,binindex in enumerate(dataset['binindex'][i]):
            if binindex not in bin_used:
                model = keras.models.load_model("weights_"+signaltype+"_bin_"+str(binindex)+".best.hdf5")
                scaler= dataset['scalers'][i][index]
                features_train=methods.feature_choice(min(binning.selections[binindex][1][1],16)-1)
                features_zllCR=methods.feature_choice_zll(min(binning.selections[binindex][1][1],16)-1)
                llepMC[binindex][b'MLscore'] = model.predict(scaler.transform(llepMC[binindex][features_train[:]]))
                llepdata[binindex][b'MLscore']=model.predict(scaler.transform(llepdata[binindex][features_train[:]]))
                mergebin=methods.zllmergebin(binindex)
                for mergebinindex in mergebin:
                    if zllMC[mergebinindex].shape[0]!=0:
                        zllMC[mergebinindex][b'MLscore'] = model.predict(scaler.transform(zllMC[mergebinindex][features_zllCR[:]]))
                    if zlldata[mergebinindex].shape[0]!=0:
                        zlldata[mergebinindex][b'MLscore'] = model.predict(scaler.transform(zlldata[mergebinindex][features_zllCR[:]]))
                figzll=methods.plot_CR_zll_pull(pd.concat([zllMC[mergebinindex] for mergebinindex in mergebin],axis=0),
                       pd.concat([zlldata[mergebinindex] for mergebinindex in mergebin],axis=0),'data MC comparison of '+r'$Z\rightarrow l^+ l^-$'+' CR in merged bin '+str(binindex) 
                       +' zll_ht '+str(min([binning.selections[mergebinindex][0][0] for mergebinindex in mergebin]))
                       +"-"+str(max([binning.selections[mergebinindex][0][1] for mergebinindex in mergebin]))
                       +" njet "+str(min([binning.selections[mergebinindex][1][0] for mergebinindex in mergebin]))
                       +"-"+str(max([binning.selections[mergebinindex][1][1] for mergebinindex in mergebin])-1)
                       +" nbjet "+str(min([binning.selections[mergebinindex][2][0] for mergebinindex in mergebin]))
                       +"-"+str(max([binning.selections[mergebinindex][2][1] for mergebinindex in mergebin])-1)+" "+r'$137.4\ fb^{-1}\ (13TeV)$')
                figllep=methods.plot_CR_llep_pull(llepMC[binindex],llepdata[binindex],'data MC comparison of lost lepton CR in bin '+str(binindex)+' ht '+str(binning.selections[binindex][0][0])
                       +"-"+str(binning.selections[binindex][0][1])+" njet "+str(binning.selections[binindex][1][0])+"-"+str(binning.selections[binindex][1][1]-1)
                       +" nbjet "+str(binning.selections[binindex][2][0])+"-"+str(binning.selections[binindex][2][1]-1)+" "+r'$137.4\ fb^{-1}\ (13TeV)$')
                figllep.savefig("llepCR_"+signaltype+"_bin"+str(binindex)+"_pull.png")
                figllep.savefig("llepCR_"+signaltype+"_bin"+str(binindex)+"_pull.pdf")
                figzll.savefig("zllCR_"+signaltype+"_bin"+str(binindex)+"_merge_pull.png")
                figzll.savefig("zllCR_"+signaltype+"_bin"+str(binindex)+"_merge_pull.pdf")
                figzll=methods.plot_CR_zll_pull(zllMC[binindex],
                       zlldata[binindex],'data MC comparison of '+r'$Z\rightarrow l^+ l^-$'+' CR in bin '+str(binindex)+' zll_ht '+str(binning.selections[binindex][0][0])
                       +"-"+str(binning.selections[binindex][0][1])+" njet "+str(binning.selections[binindex][1][0])+"-"+str(binning.selections[binindex][1][1]-1)
                       +" nbjet "+str(binning.selections[binindex][2][0])+"-"+str(binning.selections[binindex][2][1]-1)+" "+r'$137.4\ fb^{-1}\ (13TeV)$')
                figzll.savefig("zllCR_"+signaltype+"_bin"+str(binindex)+"_pull.png")
                figzll.savefig("zllCR_"+signaltype+"_bin"+str(binindex)+"_pull.pdf")
                if len(mergebin) != 1:
                    bkg_ana=bkg[binindex][bkg[binindex][b'MLtag'] >= dataset['trainfraction'][i][index][0]*20]
                    sig_ana=sig[binindex][sig[binindex][b'MLtag'] >= dataset['trainfraction'][i][index][1]*20]
                    bkg_ana[b'MLscore'] = model.predict(scaler.transform(bkg_ana[features_train[:]]))
                    sig_ana[b'MLscore'] = model.predict(scaler.transform(sig_ana[features_train[:]]))
                    bkg_trainfra=dataset['trainfraction'][i][index][0]
                    sig_trainfra=dataset['trainfraction'][i][index][1]
                    midsig=methods.computesignificance(bkg_ana,sig_ana,zllMC[binindex],llepMC[binindex],
                                           zlldata[binindex],llepdata[binindex],dataset['mass'][i][0],dataset['mass'][i][1],1-bkg_trainfra,1-sig_trainfra,0.005,usedataexp)
                    figmidsig=methods.plot_significance(midsig,'significance change of mass '+str(dataset['mass'][i][0])+" "+str(dataset['mass'][i][1]) +" with cut (zll bin not merged)--bin"
                       +str(i)+' ht '+str(binning.selections[binindex][0][0])
                       +"-"+str(binning.selections[binindex][0][1])+" njet "+str(binning.selections[binindex][1][0])+"-"+str(binning.selections[binindex][1][1]-1)
                       +" nbjet "+str(binning.selections[binindex][2][0])+"-"+str(binning.selections[binindex][2][1]-1))

                    figmidsig.savefig("significance_"+signaltype+"_singlebin_"+str(binindex)+"_mass_"+str(dataset['mass'][i][0])+"_"+str(dataset['mass'][i][1])+".png")
                    figmidsig.savefig("significance_"+signaltype+"_singlebin_"+str(binindex)+"_mass_"+str(dataset['mass'][i][0])+"_"+str(dataset['mass'][i][1])+".pdf")
                plt.close('all')
                bin_used.append(binindex)
