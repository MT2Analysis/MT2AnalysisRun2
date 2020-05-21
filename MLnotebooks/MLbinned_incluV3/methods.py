import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from binning import selections
def significance(bkgtest,sigtest,zllCR_MC,llepCR_MC,zllCR_data,llepCR_data,m1,m2,bkgfra,sigfra,cut=0.5,weight=b'absweight',score=b'MLscore',usedataexp=False):
    b_zinv_SR=np.sum(bkgtest[(bkgtest[b'bkgtype']==3)&(bkgtest[score]>cut)][weight])/bkgfra #signal region MC bkg
    nenv_b_zinv_SR=bkgtest[(bkgtest[b'bkgtype']==3)&(bkgtest[score]>cut)].shape[0]
    b_llep_SR=np.sum(bkgtest[(bkgtest[b'bkgtype']<3)&(bkgtest[score]>cut)][weight])/bkgfra
    nenv_b_llep_SR=bkgtest[(bkgtest[b'bkgtype']<3)&(bkgtest[score]>cut)].shape[0]

    b_zll_CRMC=np.sum(zllCR_MC[zllCR_MC[score]>cut][weight])        #control region MC bkg
    nenv_b_zll_CRMC=zllCR_MC[zllCR_MC[score]>cut].shape[0]
    b_llep_CRMC=np.sum(llepCR_MC[llepCR_MC[score]>cut][weight])
    nenv_b_llep_CRMC=llepCR_MC[llepCR_MC[score]>cut].shape[0]

    if usedataexp:
        b_zll_CRdata=np.sum(zllCR_MC[zllCR_MC[score]>cut][weight]) * zllCR_data.shape[0] / np.sum(zllCR_MC[weight])
        b_llep_CRdata=np.sum(llepCR_MC[llepCR_MC[score]>cut][weight]) * llepCR_data.shape[0] / np.sum(llepCR_MC[weight])
    else:
        b_zll_CRdata=zllCR_data[zllCR_data[score]>cut].shape[0]    #control region data
        b_llep_CRdata=llepCR_data[llepCR_data[score]>cut].shape[0]

    b_zinv_estimate=0.0
    std_b_zinv_estimate=0.0
    if b_zll_CRMC!=0:
        b_zinv_estimate=b_zll_CRdata*b_zinv_SR/b_zll_CRMC
        if b_zinv_estimate!=0:
            std_b_zinv_estimate=b_zinv_estimate*np.sqrt(1.0/b_zll_CRdata+1.0/nenv_b_zinv_SR+1.0/nenv_b_zll_CRMC)
        else:
            return 0
    else:
        return 0

    b_llep_estimate=0.0
    std_b_llep_estimate=0.0
    if b_llep_CRMC!=0:
        b_llep_estimate=b_llep_CRdata*b_llep_SR/b_llep_CRMC
        if b_llep_estimate!=0:
            std_b_llep_estimate=b_llep_estimate*np.sqrt(1.0/b_llep_CRdata+1.0/nenv_b_llep_SR+1.0/nenv_b_llep_CRMC)
        else:
            return 0
    else:
        return 0

    b=b_zinv_estimate+b_llep_estimate
    sigma2_b=np.square(std_b_zinv_estimate)+np.square(std_b_llep_estimate)
    if b==0 or sigma2_b==0:
        return 0

    s=np.sum(sigtest[(sigtest[b'GenSusyMScan1']==m1)
            &(sigtest[b'GenSusyMScan2']==m2)&(sigtest[score]>cut)][weight])/sigfra
    Z=np.sqrt(2*((s+b)*np.log((s+b)*(b+sigma2_b)/(np.square(b)+(s+b)*sigma2_b)) - np.square(b)/sigma2_b*np.log(1+sigma2_b*s/b/(b+sigma2_b))))
    return Z

def computesignificance(bkgtest,sigtest,zllCR_MC,llepCR_MC,zllCR_data,llepCR_data,m1,m2,bkgfra,sigfra,interval=0.005,usedataexp=False):
    midsig=[]
    for cut in np.arange(0, 1.+interval,interval):
        midsig.append(significance(bkgtest,sigtest,zllCR_MC,llepCR_MC,zllCR_data,llepCR_data,m1,m2,bkgfra,sigfra,cut,weight=b'absweight',score=b'MLscore',usedataexp=usedataexp))
    return midsig

def plot_significance(midsig,title):
    fig=plt.figure(figsize=(16,10))
    ax = fig.add_subplot(111)
    interval=1./(len(midsig)-1)
    ax.plot(np.arange(0, 1.+interval,interval),midsig)
    ax.set_title(title,size=22)
    ax.set_xlabel('Score cut',fontsize=18)
    ax.set_ylabel('approximate median significance',fontsize=18)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    return fig

def countCRdata(zllCR_data,llepCR_data,interval=0.005):
    zlldataCR=[]
    llepdataCR=[]
    for cut in np.arange(0, 1.+interval,interval):
        zlldataCR.append(zllCR_data[zllCR_data[b'MLscore']>cut].shape[0])
        llepdataCR.append(llepCR_data[llepCR_data[b'MLscore']>cut].shape[0])
    return (zlldataCR,llepdataCR)

def plot_metrics(history,title):
    metrics =  ['loss','auc','recall','accuracy']
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
    fig=plt.figure(figsize=(12,10))
    fig.suptitle(title, fontsize=20)
    for n, metric in enumerate(metrics):
        name = metric.replace("_"," ").capitalize()
        ax=fig.add_subplot(2,2,n+1)
        ax.plot(history.epoch,  history.history[metric], color=colors[0], label='Training')
        ax.plot(history.epoch, history.history['val_'+metric],
             color=colors[0], linestyle="--", label='Validation')
        ax.set_xlabel('Epoch')
        ax.set_ylabel(name)
        ax.set_ylim(bottom=0)
        ax.legend(loc='best',prop={'size': 14})
    return fig

def plot_metrics_loss(history,title):
    fig=plt.figure(figsize=(16,10))
    ax  = fig.add_subplot(111)
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
    ax.plot(history.epoch,  history.history['loss'], color=colors[0], label='Training')
    ax.plot(history.epoch, history.history['val_'+'loss'],
             color=colors[0], linestyle="--", label='Validation')
    ax.set_xlabel('Epoch',fontsize=20)
    ax.set_ylabel('Loss',fontsize=20)
    ax.set_ylim(bottom=0)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.legend(loc='best',prop={'size': 14})
    ax.set_title(title,size=22)
    return fig

def plot_metrics_accuracy(history,title):
    fig=plt.figure(figsize=(16,10))
    ax  = fig.add_subplot(111)
    colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
    ax.plot(history.epoch,  history.history['accuracy'], color=colors[0], label='Training')
    ax.plot(history.epoch, history.history['val_'+'accuracy'],
             color=colors[0], linestyle="--", label='Validation')
    ax.set_xlabel('Epoch',fontsize=20)
    ax.set_ylabel('Accuracy',fontsize=20)
    ax.set_ylim(bottom=0)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.legend(loc='best',prop={'size': 14})
    ax.set_title(title,size=22)
    return fig

def plot_train_test_compare(train_bkg,train_sig,test_bkg,test_sig,title):
    bins=np.arange(0,1.025,0.025)
    fig = plt.figure(figsize=(20,10))
    ax = fig.add_subplot(111)
    hist_train_bkg,binEdges = np.histogram(train_bkg[b'MLscore'],bins=bins,weights=train_bkg[b'absweight'])
    hist_train_sig,_=np.histogram(train_sig[b'MLscore'],bins=bins,weights=train_sig[b'absweight'])
    hist_test_bkg,_=np.histogram(test_bkg[b'MLscore'],bins=bins,weights=test_bkg[b'absweight'])
    hist_test_sig,_=np.histogram(test_sig[b'MLscore'],bins=bins,weights=test_sig[b'absweight'])

    err2_train_bkg,_=np.histogram(train_bkg[b'MLscore'],bins=bins,weights=train_bkg[b'w2'])
    err2_train_sig,_=np.histogram(train_sig[b'MLscore'],bins=bins,weights=train_sig[b'w2'])
    err2_test_bkg,_=np.histogram(test_bkg[b'MLscore'],bins=bins,weights=test_bkg[b'w2'])
    err2_test_sig,_=np.histogram(test_sig[b'MLscore'],bins=bins,weights=test_sig[b'w2'])

    bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
    width      = binEdges[1]-binEdges[0]

    sum_train_bkg=np.sum(hist_train_bkg)
    sum_train_sig=np.sum(hist_train_sig)
    sum_test_bkg=np.sum(hist_test_bkg)
    sum_test_sig=np.sum(hist_test_sig)

    hist_train_bkg=np.divide(hist_train_bkg,sum_train_bkg)
    err_train_bkg=np.divide(np.sqrt(err2_train_bkg),sum_train_bkg)
    hist_train_sig=np.divide(hist_train_sig,sum_train_sig)
    err_train_sig=np.divide(np.sqrt(err2_train_sig),sum_train_sig)
    hist_test_bkg=np.divide(hist_test_bkg,sum_test_bkg)
    err_test_bkg=np.divide(np.sqrt(err2_test_bkg),sum_test_bkg)
    hist_test_sig=np.divide(hist_test_sig,sum_test_sig)
    err_test_sig=np.divide(np.sqrt(err2_test_sig),sum_test_sig)

    ax.bar(bincenters,hist_train_bkg,width=width,color='blue',yerr=err_train_bkg,ecolor='black',alpha=0.5,label="training background in SR",capsize=10)
    ax.bar(bincenters,hist_train_sig,width=width,color='red',yerr=err_train_sig,ecolor='black',alpha=0.5,label="training inclusive signal",capsize=10)

    ax.errorbar(bincenters,hist_test_bkg,yerr=err_test_bkg,xerr=None,ecolor='blue',c='blue',fmt='o',label='test background in SR')
    ax.errorbar(bincenters,hist_test_sig,yerr=err_test_sig,xerr=None,ecolor='red',c='red',fmt='o',label='test inclusive signal')

    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel("score",fontsize=20)
    ax.set_ylabel("Events/bin normalized to 1",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)
    return fig


def plot_train_val_compare(train_bkg,train_sig,val_bkg,val_sig,title):
    bins=np.arange(0,1.025,0.025)
    fig = plt.figure(figsize=(20,10))
    ax = fig.add_subplot(111)
    hist_train_bkg,binEdges = np.histogram(train_bkg[b'MLscore'],bins=bins,weights=train_bkg[b'absweight'])
    hist_train_sig,_=np.histogram(train_sig[b'MLscore'],bins=bins,weights=train_sig[b'absweight'])
    hist_val_bkg,_=np.histogram(val_bkg[b'MLscore'],bins=bins,weights=val_bkg[b'absweight'])
    hist_val_sig,_=np.histogram(val_sig[b'MLscore'],bins=bins,weights=val_sig[b'absweight'])

    err2_train_bkg,_=np.histogram(train_bkg[b'MLscore'],bins=bins,weights=train_bkg[b'w2'])
    err2_train_sig,_=np.histogram(train_sig[b'MLscore'],bins=bins,weights=train_sig[b'w2'])
    err2_val_bkg,_=np.histogram(val_bkg[b'MLscore'],bins=bins,weights=val_bkg[b'w2'])
    err2_val_sig,_=np.histogram(val_sig[b'MLscore'],bins=bins,weights=val_sig[b'w2'])

    bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
    width      = binEdges[1]-binEdges[0]

    sum_train_bkg=np.sum(hist_train_bkg)
    sum_train_sig=np.sum(hist_train_sig)
    sum_val_bkg=np.sum(hist_val_bkg)
    sum_val_sig=np.sum(hist_val_sig)

    hist_train_bkg=np.divide(hist_train_bkg,sum_train_bkg)
    err_train_bkg=np.divide(np.sqrt(err2_train_bkg),sum_train_bkg)
    hist_train_sig=np.divide(hist_train_sig,sum_train_sig)
    err_train_sig=np.divide(np.sqrt(err2_train_sig),sum_train_sig)
    hist_val_bkg=np.divide(hist_val_bkg,sum_val_bkg)
    err_val_bkg=np.divide(np.sqrt(err2_val_bkg),sum_val_bkg)
    hist_val_sig=np.divide(hist_val_sig,sum_val_sig)
    err_val_sig=np.divide(np.sqrt(err2_val_sig),sum_val_sig)

    ax.bar(bincenters,hist_train_bkg,width=width,color='blue',yerr=err_train_bkg,ecolor='black',alpha=0.5,label="training background in SR",capsize=10)
    ax.bar(bincenters,hist_train_sig,width=width,color='red',yerr=err_train_sig,ecolor='black',alpha=0.5,label="training inclusive signal",capsize=10)

    ax.errorbar(bincenters,hist_val_bkg,yerr=err_val_bkg,xerr=None,ecolor='blue',c='blue',fmt='o',label='validation background in SR')
    ax.errorbar(bincenters,hist_val_sig,yerr=err_val_sig,xerr=None,ecolor='red',c='red',fmt='o',label='validation inclusive signal')

    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel("score",fontsize=20)
    ax.set_ylabel("Events/bin normalized to 1",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)
    return fig

def plot_train_val_test_compare(train_bkg,train_sig,val_bkg,val_sig,test_bkg,test_sig,title):
    bins=np.arange(0,1.025,0.025)
    fig = plt.figure(figsize=(20,10))
    ax = fig.add_subplot(111)
    hist_train_bkg,binEdges = np.histogram(train_bkg[b'MLscore'],bins=bins,weights=train_bkg[b'absweight'])
    hist_train_sig,_=np.histogram(train_sig[b'MLscore'],bins=bins,weights=train_sig[b'absweight'])
    hist_val_bkg,_=np.histogram(val_bkg[b'MLscore'],bins=bins,weights=val_bkg[b'absweight'])
    hist_val_sig,_=np.histogram(val_sig[b'MLscore'],bins=bins,weights=val_sig[b'absweight'])
    hist_test_bkg,_=np.histogram(test_bkg[b'MLscore'],bins=bins,weights=test_bkg[b'absweight'])
    hist_test_sig,_=np.histogram(test_sig[b'MLscore'],bins=bins,weights=test_sig[b'absweight'])

    err2_train_bkg,_=np.histogram(train_bkg[b'MLscore'],bins=bins,weights=train_bkg[b'w2'])
    err2_train_sig,_=np.histogram(train_sig[b'MLscore'],bins=bins,weights=train_sig[b'w2'])
    err2_val_bkg,_=np.histogram(val_bkg[b'MLscore'],bins=bins,weights=val_bkg[b'w2'])
    err2_val_sig,_=np.histogram(val_sig[b'MLscore'],bins=bins,weights=val_sig[b'w2'])
    err2_test_bkg,_=np.histogram(test_bkg[b'MLscore'],bins=bins,weights=test_bkg[b'w2'])
    err2_test_sig,_=np.histogram(test_sig[b'MLscore'],bins=bins,weights=test_sig[b'w2'])

    bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
    width      = binEdges[1]-binEdges[0]

    sum_train_bkg=np.sum(hist_train_bkg)
    sum_train_sig=np.sum(hist_train_sig)
    sum_val_bkg=np.sum(hist_val_bkg)
    sum_val_sig=np.sum(hist_val_sig)
    sum_test_bkg=np.sum(hist_test_bkg)
    sum_test_sig=np.sum(hist_test_sig)

    hist_train_bkg=np.divide(hist_train_bkg,sum_train_bkg)
    err_train_bkg=np.divide(np.sqrt(err2_train_bkg),sum_train_bkg)
    hist_train_sig=np.divide(hist_train_sig,sum_train_sig)
    err_train_sig=np.divide(np.sqrt(err2_train_sig),sum_train_sig)
    hist_val_bkg=np.divide(hist_val_bkg,sum_val_bkg)
    err_val_bkg=np.divide(np.sqrt(err2_val_bkg),sum_val_bkg)
    hist_val_sig=np.divide(hist_val_sig,sum_val_sig)
    err_val_sig=np.divide(np.sqrt(err2_val_sig),sum_val_sig)
    hist_test_bkg=np.divide(hist_test_bkg,sum_test_bkg)
    err_test_bkg=np.divide(np.sqrt(err2_test_bkg),sum_test_bkg)
    hist_test_sig=np.divide(hist_test_sig,sum_test_sig)
    err_test_sig=np.divide(np.sqrt(err2_test_sig),sum_test_sig)

    ax.bar(bincenters,hist_train_bkg,width=width,color='blue',yerr=err_train_bkg,ecolor='black',alpha=0.5,label="training background in SR",capsize=10)
    ax.bar(bincenters,hist_train_sig,width=width,color='red',yerr=err_train_sig,ecolor='black',alpha=0.5,label="training inclusive signal",capsize=10)

    ax.bar(bincenters,hist_val_bkg,width=width,linewidth=0,capsize=5,ecolor='blue',yerr=err_val_bkg,fill=False)
    ax.bar(bincenters,hist_val_sig,width=width,linewidth=0,capsize=5,ecolor='red',yerr=err_val_sig,fill=False)

    ax.step(bincenters[0:len(bincenters)],hist_val_bkg,where='mid',linewidth=2,linestyle='dashed',color='blue',label='validation background in SR')
    ax.step(bincenters[0:len(bincenters)],hist_val_sig,where='mid',linewidth=2,linestyle='dashed',color='red',label='validation inclusive signal')

    ax.errorbar(bincenters,hist_test_bkg,yerr=err_test_bkg,xerr=None,ecolor='blue',c='blue',fmt='o',label='test background in SR')
    ax.errorbar(bincenters,hist_test_sig,yerr=err_test_sig,xerr=None,ecolor='red',c='red',fmt='o',label='test inclusive signal')

    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel("score",fontsize=20)
    ax.set_ylabel("Events/bin normalized to 1",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)
    return fig


def plot_sig_bkg_compare(sig,bkg,mass1,mass2,title):
    bins=np.arange(0,1.025,0.025)
    fig = plt.figure(figsize=(20,10))
    ax = fig.add_subplot(111)

    bkg_top=bkg[(bkg[b'bkgtype']==0)|(bkg[b'bkgtype']==1)]
    bkg_wjets=bkg[bkg[b'bkgtype']==2]
    bkg_zinv=bkg[bkg[b'bkgtype']==3]

    sig_exclu=sig[(sig[b'GenSusyMScan1']==mass1)&(sig[b'GenSusyMScan2']==mass2)]

    err2_bkg,binEdges=np.histogram(bkg[b'MLscore'],bins=bins,weights=bkg[b'w2'])
    err2_sig_exclu,_=np.histogram(sig_exclu[b'MLscore'],bins=bins,weights=sig_exclu[b'w2'])
    err2_sig,_=np.histogram(sig[b'MLscore'],bins=bins,weights=sig[b'w2'])
    bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
    width      = binEdges[1]-binEdges[0]

    hist_bkg_top,_=np.histogram(bkg_top[b'MLscore'],bins=bins,weights=bkg_top[b'absweight'])
    hist_bkg_wjets,_=np.histogram(bkg_wjets[b'MLscore'],bins=bins,weights=bkg_wjets[b'absweight'])
    hist_bkg_zinv,_=np.histogram(bkg_zinv[b'MLscore'],bins=bins,weights=bkg_zinv[b'absweight'])
    sum_bkg=np.sum(hist_bkg_top)+np.sum(hist_bkg_wjets)+np.sum(hist_bkg_zinv)
    hist_bkg_top=np.divide(hist_bkg_top,sum_bkg)
    hist_bkg_wjets=np.divide(hist_bkg_wjets,sum_bkg)
    hist_bkg_zinv=np.divide(hist_bkg_zinv,sum_bkg)
    err_bkg=np.divide(np.sqrt(err2_bkg),sum_bkg)

    hist_sig_exclu,_=np.histogram(sig_exclu[b'MLscore'],bins=bins,weights=sig_exclu[b'absweight'])
    sum_sig_exclu=np.sum(hist_sig_exclu)
    hist_sig_exclu=np.divide(hist_sig_exclu,sum_sig_exclu)
    err_sig_exclu=np.divide(np.sqrt(err2_sig_exclu),sum_sig_exclu)
    hist_sig,_=np.histogram(sig[b'MLscore'],bins=bins,weights=sig[b'absweight'])
    sum_sig=np.sum(hist_sig)
    hist_sig=np.divide(hist_sig,sum_sig)
    err_sig=np.divide(np.sqrt(err2_sig),sum_sig)

    ax.bar(bincenters,hist_bkg_top,width=width,color='lightskyblue',alpha=0.7,label="top background in SR")
    ax.bar(bincenters,hist_bkg_wjets,bottom=hist_bkg_top,width=width,color='tan',alpha=0.7,label="wjets background in SR")
    ax.bar(bincenters,hist_bkg_zinv,bottom=hist_bkg_top+hist_bkg_wjets,width=width,color='thistle',yerr=err_bkg,alpha=0.7,label="zinv background in SR",capsize=5)

    ax.bar(bincenters,hist_sig,width=width,linewidth=0,capsize=5,ecolor='blue',yerr=err_sig,fill=False)
    ax.bar(bincenters,hist_sig_exclu,width=width,linewidth=0,capsize=5,ecolor='red',yerr=err_sig_exclu,fill=False)

    ax.step(bincenters[0:len(bincenters)],hist_sig,where='mid',linewidth=2,linestyle='dashed',color='blue',label='inclusive signal')
    ax.step(bincenters[0:len(bincenters)],hist_sig_exclu,where='mid',linewidth=2,linestyle='solid',color='red',label='signal SUSY parent '+str(mass1)+'GeV \n SUSY daughter '+str(mass2)+'GeV')
#    ax.hist(sig[b'MLscore'],bins=bins,alpha=1,label='inclusive signal',density=True,histtype='step',linewidth=2,color='blue',linestyle='dashed')
#    ax.hist(sig_exclu[b'MLscore'],bins=bins,alpha=1,label='signal gluino '+str(mass1)+'GeV \n neutralino '+str(mass2)+'GeV',density=True,histtype='step',linewidth=2,color='red',linestyle='solid')
    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel('score',fontsize=20)
    ax.set_ylabel("Events/bin normalized to 1",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)
    return fig

def plot_data_MC_compare(MC,data,MCtrainfra,title):
    bins=np.arange(0,1.025,0.025)
    fig = plt.figure(figsize=(20,10))
    ax = fig.add_subplot(111)

    err2_MC,binEdges=np.histogram(MC[b'MLscore'],bins=bins,weights=MC[b'w2'])
    hist_MC,_ = np.histogram(MC[b'MLscore'],bins=bins,weights=MC[b'absweight'])
    bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
    width      = binEdges[1]-binEdges[0]
    hist_MC = np.divide(hist_MC,1.0-MCtrainfra)
    err_MC  = np.sqrt(err2_MC)
    err_MC = np.divide(err_MC,1.0-MCtrainfra)
    hist_data,_=np.histogram(data[b'MLscore'],bins=bins)
    err_data=np.sqrt(hist_data)

    ax.bar(bincenters,hist_MC,width=width,color='blue',yerr=err_MC,ecolor='black',alpha=0.5,label="MC background in SR(test set)",capsize=10)
    ax.errorbar(bincenters,hist_data,yerr=err_data,xerr=None,ecolor='red',c='red',fmt='o',label='data in SR')
    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel('score',fontsize=20)
    ax.set_ylabel("Events distribution",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)
    return fig


def plot_CR_llep(MC,data,title):
    bins=np.arange(0,1.025,0.025)
    fig = plt.figure(figsize=(20,10))
    ax = fig.add_subplot(111)

    MC_top=MC[(MC[b'bkgtype']==0)|(MC[b'bkgtype']==1)]
    MC_wjets=MC[MC[b'bkgtype']==2]

    err2_MC,binEdges=np.histogram(MC[b'MLscore'],bins=bins,weights=MC[b'w2'])
    bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
    width      = binEdges[1]-binEdges[0]

    hist_MC_top,_=np.histogram(MC_top[b'MLscore'],bins=bins,weights=MC_top[b'absweight'])
    hist_MC_wjets,_=np.histogram(MC_wjets[b'MLscore'],bins=bins,weights=MC_wjets[b'absweight'])

#    sum_MC=np.sum(hist_MC_top)+np.sum(hist_MC_wjets)
#    hist_MC_top=np.divide(hist_MC_top,sum_MC*width)
#    hist_MC_wjets=np.divide(hist_MC_wjets,sum_MC*width)
#    err_MC=np.divide(np.sqrt(err2_MC),sum_MC*width)
    err_MC=np.sqrt(err2_MC)

    hist_data,_=np.histogram(data[b'MLscore'],bins=bins)
#    sum_data=np.sum(hist_data)
#    err_data=np.divide(np.sqrt(hist_data),sum_data*width)
#    hist_data=np.divide(hist_data,sum_data*width)
    err_data=np.sqrt(hist_data)

    ax.bar(bincenters,hist_MC_top,width=width,color='lightskyblue',alpha=0.7,label="top(MC) in llep CR")
    ax.bar(bincenters,hist_MC_wjets,bottom=hist_MC_top,width=width,color='tan',yerr=err_MC,alpha=0.7,label="wjets(MC) in llep CR",capsize=10)

    ax.errorbar(bincenters,hist_data,yerr=err_data,xerr=None,ecolor='red',c='red',fmt='o',label='data in llep CR')

    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel('score',fontsize=20)
#    ax.set_ylabel("normalized distribution",fontsize=20)
    ax.set_ylabel("Events distribution",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)
    return fig

def plot_CR_llep_pull(MC,data,title):
    bins=np.arange(0,1.025,0.025)
    fig = plt.figure(figsize=(20,16))
    grid = plt.GridSpec(4,1, hspace=0.35, wspace=0.35)
    ax = fig.add_subplot(grid[0:3,:])
    pull = fig.add_subplot(grid[3,:],sharex=ax)

    MC_top=MC[(MC[b'bkgtype']==0)|(MC[b'bkgtype']==1)]
    MC_wjets=MC[MC[b'bkgtype']==2]

    err2_MC,binEdges=np.histogram(MC[b'MLscore'],bins=bins,weights=MC[b'w2'])
    bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
    width      = binEdges[1]-binEdges[0]

    hist_MC_top,_=np.histogram(MC_top[b'MLscore'],bins=bins,weights=MC_top[b'absweight'])
    hist_MC_wjets,_=np.histogram(MC_wjets[b'MLscore'],bins=bins,weights=MC_wjets[b'absweight'])

    err_MC=np.sqrt(err2_MC)

    hist_data,_=np.histogram(data[b'MLscore'],bins=bins)
    err_data=np.sqrt(hist_data)

    ax.bar(bincenters,hist_MC_top,width=width,color='lightskyblue',alpha=0.7,label="top(MC) in llep CR")
    ax.bar(bincenters,hist_MC_wjets,bottom=hist_MC_top,width=width,color='tan',alpha=0.7,label="wjets(MC) in llep CR")
    ax.bar(bincenters,2*err_MC,bottom=hist_MC_top+hist_MC_wjets-err_MC,width=width,color='grey',alpha=0,label="MC uncertainty",hatch="//")

    ax.errorbar(bincenters,hist_data,yerr=err_data,xerr=None,ecolor='red',c='red',fmt='o',label='data in llep CR')

    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel('score',fontsize=20)
    ax.set_ylabel("Events distribution",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)

    hist_MC=np.add(hist_MC_top,hist_MC_wjets)
    err_MCratio=np.divide(err_MC,hist_MC)
    dataMCratio=np.divide(hist_data,hist_MC)
    err_dataMCratio=np.divide(err_data,hist_MC)
    dataMCratio_total=np.sum(hist_data)/np.sum(hist_MC)
    err_dataMCratio_total=dataMCratio_total*np.sqrt(1/np.sum(hist_data)+np.sum(err2_MC)/np.square(np.sum(hist_MC)))
    pull.axhline(y=1, color='black', linestyle='-')
    pull.axhline(y=dataMCratio_total, color='r', linestyle='-')
    pull.bar(bincenters,2*err_MCratio,bottom=1-err_MCratio,width=width,color='grey',alpha=0,hatch="//")
    pull.errorbar(bincenters,dataMCratio,yerr=err_dataMCratio,xerr=None,ecolor='red',c='red',fmt='o')
    pull.tick_params(axis='both', which='major', labelsize=16)
    pull.tick_params(axis='both', which='minor', labelsize=16)
    pull.set_ylabel("data/MC",fontsize=20)
    pull.set_title("data/MC = "+'{:.2f}'.format(dataMCratio_total)+r'$\pm$'+'{:.2f}'.format(err_dataMCratio_total),size=20,loc='left')
    return fig

def plot_CR_zll(MC,data,title):
    bins=np.arange(0,1.025,0.025)
    fig = plt.figure(figsize=(20,10))
    ax = fig.add_subplot(111)

    err2_MC,binEdges=np.histogram(MC[b'MLscore'],bins=bins,weights=MC[b'w2'])
    bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
    width      = binEdges[1]-binEdges[0]

    hist_MC,_=np.histogram(MC[b'MLscore'],bins=bins,weights=MC[b'absweight'])
#    sum_MC=np.sum(hist_MC)
#    hist_MC=np.divide(hist_MC,sum_MC*width)
#    err_MC=np.divide(np.sqrt(err2_MC),sum_MC*width)
    err_MC=np.sqrt(err2_MC)


    hist_data,_=np.histogram(data[b'MLscore'],bins=bins)
#    sum_data=np.sum(hist_data)
#    err_data=np.divide(np.sqrt(hist_data),sum_data*width)
#    hist_data=np.divide(hist_data,sum_data*width)
    err_data=np.sqrt(hist_data)

    ax.bar(bincenters,hist_MC,width=width,color='thistle',yerr=err_MC,alpha=0.7,label="dyjetsll(MC) in zll CR",capsize=10)
    ax.errorbar(bincenters,hist_data,yerr=err_data,xerr=None,ecolor='red',c='red',fmt='o',label='data in zll CR')

    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel('score',fontsize=20)
#    ax.set_ylabel("normalized distribution",fontsize=20)
    ax.set_ylabel("Events distribution",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)
    return fig

def plot_CR_zll_pull(MC,data,title):
    bins=np.arange(0,1.025,0.025)
    fig = plt.figure(figsize=(20,16))
    grid = plt.GridSpec(4,1, hspace=0.35, wspace=0.35)
    ax = fig.add_subplot(grid[0:3,:])
    pull = fig.add_subplot(grid[3,:],sharex=ax)

    err2_MC,binEdges=np.histogram(MC[b'MLscore'],bins=bins,weights=MC[b'w2'])
    bincenters = 0.5*(binEdges[1:]+binEdges[:-1])
    width      = binEdges[1]-binEdges[0]

    hist_MC,_=np.histogram(MC[b'MLscore'],bins=bins,weights=MC[b'absweight'])
#    sum_MC=np.sum(hist_MC)
#    hist_MC=np.divide(hist_MC,sum_MC*width)
#    err_MC=np.divide(np.sqrt(err2_MC),sum_MC*width)
    err_MC=np.sqrt(err2_MC)


    hist_data,_=np.histogram(data[b'MLscore'],bins=bins)
#    sum_data=np.sum(hist_data)
#    err_data=np.divide(np.sqrt(hist_data),sum_data*width)
#    hist_data=np.divide(hist_data,sum_data*width)
    err_data=np.sqrt(hist_data)

    ax.bar(bincenters,hist_MC,width=width,color='thistle',alpha=0.7,label="dyjetsll(MC) in zll CR")
    ax.errorbar(bincenters,hist_data,yerr=err_data,xerr=None,ecolor='red',c='red',fmt='o',label='data in zll CR')
    ax.bar(bincenters,2*err_MC,bottom=hist_MC-err_MC,width=width,color='grey',alpha=0,label="MC uncertainty",hatch="//")

    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel('score',fontsize=20)
#    ax.set_ylabel("normalized distribution",fontsize=20)
    ax.set_ylabel("Events distribution",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)

    err_MCratio=np.divide(err_MC,hist_MC)
    dataMCratio=np.divide(hist_data,hist_MC)
    err_dataMCratio=np.divide(err_data,hist_MC)
    dataMCratio_total=np.sum(hist_data)/np.sum(hist_MC)
    err_dataMCratio_total=dataMCratio_total*np.sqrt(1/np.sum(hist_data)+np.sum(err2_MC)/np.square(np.sum(hist_MC)))
    pull.axhline(y=1, color='black', linestyle='-')
    pull.axhline(y=dataMCratio_total, color='r', linestyle='-')
    pull.bar(bincenters,2*err_MCratio,bottom=1-err_MCratio,width=width,color='grey',alpha=0,hatch="//")
    pull.errorbar(bincenters,dataMCratio,yerr=err_dataMCratio,xerr=None,ecolor='red',c='red',fmt='o')
    pull.tick_params(axis='both', which='major', labelsize=16)
    pull.tick_params(axis='both', which='minor', labelsize=16)
    pull.set_ylabel("data/MC",fontsize=20)
    pull.set_title("data/MC = "+'{:.2f}'.format(dataMCratio_total)+r'$\pm$'+'{:.2f}'.format(err_dataMCratio_total),size=20,loc='left')
    return fig

def plot_score_mt2_corr(sig,bkg,title):
    fig = plt.figure(figsize=(10,10))
    ax = fig.add_subplot(111)
    dataset=(sig,bkg)
    colors=('blue','red')
    groups=('inclusive signal','background')
    for data,color,group in zip(dataset,colors,groups):
        if data.shape[0]>100000:
            ax.scatter(data[b'mt2'], data[b'MLscore'], alpha=0.8, c=color, edgecolors='none', s=0.5, label=group)
        elif data.shape[0]>10000:
            ax.scatter(data[b'mt2'], data[b'MLscore'], alpha=0.8, c=color, edgecolors='none', s=1, label=group)
        elif data.shape[0]>1000:
            ax.scatter(data[b'mt2'], data[b'MLscore'], alpha=0.8, c=color, edgecolors='none', s=3, label=group)
        else:
            ax.scatter(data[b'mt2'], data[b'MLscore'], alpha=0.8, c=color, edgecolors='none', s=10, label=group)
    ax.legend(loc='best',prop={'size': 16})
    ax.set_xlabel('MT2',fontsize=20)
    ax.set_ylabel("MLscore",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=16)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)
    return fig

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
             [b'jet10_eta',b'jet10_phi',b'jet10_pt',b'jet10_btagDeepCSV'],
             [b'jet11_eta',b'jet11_phi',b'jet11_pt',b'jet11_btagDeepCSV'],
             [b'jet12_eta',b'jet12_phi',b'jet12_pt',b'jet12_btagDeepCSV'],
             [b'jet13_eta',b'jet13_phi',b'jet13_pt',b'jet13_btagDeepCSV'],
             [b'jet14_eta',b'jet14_phi',b'jet14_pt',b'jet14_btagDeepCSV'],
             [b'jet15_eta',b'jet15_phi',b'jet15_pt',b'jet15_btagDeepCSV']]
    features_train=[b'deltaPhiMin',b'diffMetMht',b'mht_pt',b'met_pt',b'mht_phi',b'met_phi']
    for i in range(njet):
        features_train=features_train+features_pre[i]
    return features_train


def feature_choice_zll(njet):
    features_pre=[[b'jet1_eta',b'jet1_phi',b'jet1_pt',b'jet1_btagDeepCSV'],
             [b'jet2_eta',b'jet2_phi',b'jet2_pt',b'jet2_btagDeepCSV'],
             [b'jet3_eta',b'jet3_phi',b'jet3_pt',b'jet3_btagDeepCSV'],
             [b'jet4_eta',b'jet4_phi',b'jet4_pt',b'jet4_btagDeepCSV'],
             [b'jet5_eta',b'jet5_phi',b'jet5_pt',b'jet5_btagDeepCSV'],
             [b'jet6_eta',b'jet6_phi',b'jet6_pt',b'jet6_btagDeepCSV'],
             [b'jet7_eta',b'jet7_phi',b'jet7_pt',b'jet7_btagDeepCSV'],
             [b'jet8_eta',b'jet8_phi',b'jet8_pt',b'jet8_btagDeepCSV'],
             [b'jet9_eta',b'jet9_phi',b'jet9_pt',b'jet9_btagDeepCSV'],
             [b'jet10_eta',b'jet10_phi',b'jet10_pt',b'jet10_btagDeepCSV'],
             [b'jet11_eta',b'jet11_phi',b'jet11_pt',b'jet11_btagDeepCSV'],
             [b'jet12_eta',b'jet12_phi',b'jet12_pt',b'jet12_btagDeepCSV'],
             [b'jet13_eta',b'jet13_phi',b'jet13_pt',b'jet13_btagDeepCSV'],
             [b'jet14_eta',b'jet14_phi',b'jet14_pt',b'jet14_btagDeepCSV'],
             [b'jet15_eta',b'jet15_phi',b'jet15_pt',b'jet15_btagDeepCSV']]
    features_train=[b'zll_deltaPhiMin',b'zll_diffMetMht',b'zll_mht_pt',b'zll_met_pt',b'zll_mht_phi',b'zll_met_phi']
    for i in range(njet):
        features_train=features_train+features_pre[i]
    return features_train


def zllmergebin(binindex):
    mergebin1=[]
    mergebin2=[]
    mergebin3=[]
    mergebin4=[]
    mergebin5=[]
    mergebin6=[]
    mergebin7=[]
    mergebin8=[]
    for i in range(len(selections)):
        if selections[i][0][1]==450 and selections[i][1][0]>=7 and selections[i][2][0]!=0:
            mergebin1.append(i)
        elif selections[i][0][1]==575 and selections[i][1][0]>=7 and selections[i][2][0]!=0:
            mergebin2.append(i)
        elif selections[i][0][0]==575 and selections[i][1][0]>=7 and selections[i][2][0]==0:
            mergebin3.append(i)
        elif selections[i][0][0]==1200 and selections[i][1][0]>=7 and selections[i][2][0]==0:
            mergebin4.append(i)
        elif selections[i][0][0]==1500 and selections[i][1][0]>=7 and selections[i][2][0]==0:
            mergebin5.append(i)
        elif selections[i][0][0]==575 and selections[i][1][0]>=7 and selections[i][2][0]!=0:
            mergebin6.append(i)
        elif selections[i][0][0]==1200 and selections[i][1][0]>=7 and selections[i][2][0]!=0:
            mergebin7.append(i)
        elif selections[i][0][0]==1500 and selections[i][1][0]>=7 and selections[i][2][0]!=0:
            mergebin8.append(i)
    if binindex in mergebin1:
        return mergebin1
    elif binindex in mergebin2:
        return mergebin2
    elif binindex in mergebin3:
        return mergebin3
    elif binindex in mergebin4:
        return mergebin4
    elif binindex in mergebin5:
        return mergebin5
    elif binindex in mergebin6:
        return mergebin6
    elif binindex in mergebin7:
        return mergebin7
    elif binindex in mergebin8:
        return mergebin8
    else:
        return [binindex]
