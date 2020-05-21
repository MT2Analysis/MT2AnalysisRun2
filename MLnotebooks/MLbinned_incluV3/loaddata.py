import numpy as np
import pandas as pd
import binning

bkgfiles_2016=['/work/wjin/featurereduced4/bkg/preselection_2016_singletop_skim.npy',
'/work/wjin/featurereduced4/bkg/preselection_2016_tt_skim.npy',
'/work/wjin/featurereduced4/bkg/preselection_2016_wjets_skim.npy',
'/work/wjin/featurereduced4/bkg/preselection_2016_zinv_skim.npy']

bkgfiles_2017=['/work/wjin/featurereduced4/bkg/preselection_2017_singletop_skim.npy',
'/work/wjin/featurereduced4/bkg/preselection_2017_tt_skim.npy',
'/work/wjin/featurereduced4/bkg/preselection_2017_wjets_skim.npy',
'/work/wjin/featurereduced4/bkg/preselection_2017_zinv_skim.npy']

bkgfiles_2018=['/work/wjin/featurereduced4/bkg/preselection_2018_singletop_skim.npy',
'/work/wjin/featurereduced4/bkg/preselection_2018_tt_skim.npy',
'/work/wjin/featurereduced4/bkg/preselection_2018_wjets_skim.npy',
'/work/wjin/featurereduced4/bkg/preselection_2018_zinv_skim.npy']

bkgfiles_all=bkgfiles_2016+bkgfiles_2017+bkgfiles_2018

llfiles_2016=['/work/wjin/featurereduced4/llcontrol/preselection_2016_singletop_skim.npy',
'/work/wjin/featurereduced4/llcontrol/preselection_2016_tt_skim.npy',
'/work/wjin/featurereduced4/llcontrol/preselection_2016_wjets_skim.npy']

llfiles_2017=['/work/wjin/featurereduced4/llcontrol/preselection_2017_singletop_skim.npy',
'/work/wjin/featurereduced4/llcontrol/preselection_2017_tt_skim.npy',
'/work/wjin/featurereduced4/llcontrol/preselection_2017_wjets_skim.npy']

llfiles_2018=['/work/wjin/featurereduced4/llcontrol/preselection_2018_singletop_skim.npy',
'/work/wjin/featurereduced4/llcontrol/preselection_2018_tt_skim.npy',
'/work/wjin/featurereduced4/llcontrol/preselection_2018_wjets_skim.npy']

llfiles_MC_all=llfiles_2016+llfiles_2017+llfiles_2018

llfiles_2016_data=['/work/wjin/featurereduced4/llcontrol/preselection_2016_lldata_skim.npy']

llfiles_2017_data=['/work/wjin/featurereduced4/llcontrol/preselection_2017_lldata_skim.npy']

llfiles_2018_data=['/work/wjin/featurereduced4/llcontrol/preselection_2018_lldata_skim.npy']

llfiles_data_all=llfiles_2016_data+llfiles_2017_data+llfiles_2018_data

zllfiles_2016=['/work/wjin/featurereduced4/zinvcontrol/preselection_2016_dyjetsll_skim.npy']

zllfiles_2017=['/work/wjin/featurereduced4/zinvcontrol/preselection_2017_dyjetsll_skim.npy']

zllfiles_2018=['/work/wjin/featurereduced4/zinvcontrol/preselection_2018_dyjetsll_skim.npy']

zllfiles_MC_all=zllfiles_2016+zllfiles_2017+zllfiles_2018

zllfiles_2016_data=["/work/wjin/featurereduced4/zinvcontrol/preselection_2016_zlldata_skim.npy"]

zllfiles_2017_data=["/work/wjin/featurereduced4/zinvcontrol/preselection_2017_zlldata_skim.npy"]

zllfiles_2018_data=["/work/wjin/featurereduced4/zinvcontrol/preselection_2018_zlldata_skim.npy"]

zllfiles_data_all=zllfiles_2016_data+zllfiles_2017_data+zllfiles_2018_data

SRdatafiles_2016=["/work/wjin/featurereduced4/SRdata/preselection_2016_SRdata_skim.npy"]

SRdatafiles_2017=["/work/wjin/featurereduced4/SRdata/preselection_2017_SRdata_skim.npy"]

SRdatafiles_2018=["/work/wjin/featurereduced4/SRdata/preselection_2018_SRdata_skim.npy"]

SRdatafiles_all=SRdatafiles_2016+SRdatafiles_2017+SRdatafiles_2018

sigfiles_T1bbbb_2016=['/work/wjin/featurereduced4/sig/preselection_2016_T1bbbb_94x_1_skim.npy',
                     '/work/wjin/featurereduced4/sig/preselection_2016_T1bbbb_94x_2_skim.npy',
                     '/work/wjin/featurereduced4/sig/preselection_2016_T1bbbb_94x_skim.npy']

sigfiles_T1bbbb_2017=['/work/wjin/featurereduced4/sig/preselection_2017_T1bbbb_1_skim.npy',
                     '/work/wjin/featurereduced4/sig/preselection_2017_T1bbbb_2_skim.npy',
                     '/work/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_1_skim.npy',
                     '/work/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_2_skim.npy',
                     '/work/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_3_skim.npy',
                     '/work/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_4_skim.npy',
                     '/work/wjin/featurereduced4/sig/preselection_2017_T1bbbb_ext1_skim.npy',
                     '/work/wjin/featurereduced4/sig/preselection_2017_T1bbbb_skim.npy']

sigfiles_T1qqqq_2016=['/work/wjin/featurereduced4/sig/preselection_2016_T1qqqq_94x_1_skim.npy',
                      '/work/wjin/featurereduced4/sig/preselection_2016_T1qqqq_94x_2_skim.npy',
                      '/work/wjin/featurereduced4/sig/preselection_2016_T1qqqq_94x_skim.npy']

sigfiles_T1qqqq_2017=['/work/wjin/featurereduced4/sig/preselection_2017_T1qqqq_skim.npy',
                      '/work/wjin/featurereduced4/sig/preselection_2017_T1qqqq_ext1_skim.npy',
                      '/work/wjin/featurereduced4/sig/preselection_2017_T1qqqq_ext1_1_skim.npy',
                      '/work/wjin/featurereduced4/sig/preselection_2017_T1qqqq_ext1_2_skim.npy',
                      '/work/wjin/featurereduced4/sig/preselection_2017_T1qqqq_ext1_3_skim.npy',
                      '/work/wjin/featurereduced4/sig/preselection_2017_T1qqqq_ext1_4_skim.npy',
                      '/work/wjin/featurereduced4/sig/preselection_2017_T1qqqq_1_skim.npy',
                      '/work/wjin/featurereduced4/sig/preselection_2017_T1qqqq_2_skim.npy']

sigfiles_T2bb_2016=['/work/wjin/featurereduced4/sig/preselection_2016_T2bb_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2016_T2bb_1_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2016_T2bb_mSbot1650to2600_skim.npy']

sigfiles_T2bb_2017=['/work/wjin/featurereduced4/sig/preselection_2017_T2bb_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2bb_1_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2bb_ext1_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2bb_ext1_1_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2bb_mSbot1650to2600_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2bb_mSbot1650to2600_ext1_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2bb_mSbot1650to2600_ext1_1_skim.npy']

sigfiles_T2qq_2016=['/work/wjin/featurereduced4/sig/preselection_2016_T2qq_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2016_T2qq_1_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2016_T2qq_mSq1850to2600_skim.npy']

sigfiles_T2qq_2017=['/work/wjin/featurereduced4/sig/preselection_2017_T2qq_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2qq_1_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2qq_2_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2qq_ext1_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2qq_ext1_1_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2qq_ext1_2_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2qq_ext1_3_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2qq_mSq_1850to2600_skim.npy',
                    '/work/wjin/featurereduced4/sig/preselection_2017_T2qq_mSq_1850to2600_ext1_skim.npy']

sigfiles_T1bbbb_all=sigfiles_T1bbbb_2016+sigfiles_T1bbbb_2017

sigfiles_T1qqqq_all=sigfiles_T1qqqq_2016+sigfiles_T1qqqq_2017

sigfiles_T2bb_all=sigfiles_T2bb_2016+sigfiles_T2bb_2017

sigfiles_T2qq_all=sigfiles_T2qq_2016+sigfiles_T2qq_2017

def loadbkg(year=0,isbin=True, isinclubin=False):
    pd.options.mode.chained_assignment = None
    bkgfiles=[]
    if year==2016:
        bkgfiles=bkgfiles_2016
    elif year==2017:
        bkgfiles=bkgfiles_2017
    elif year==2018:
        bkgfiles=bkgfiles_2018
    else:
        bkgfiles=bkgfiles_all


    databkg=[]
    for i in range(len(bkgfiles)):
        databkg.append(pd.DataFrame(np.load(bkgfiles[i],allow_pickle=True).item()))
        if "singletop" in bkgfiles[i]:
            databkg[i][b'bkgtype']=0
        elif "tt" in bkgfiles[i]:
            databkg[i][b'bkgtype']=1
        elif "wjets" in bkgfiles[i]:
            databkg[i][b'bkgtype']=2
        elif "zinv" in bkgfiles[i]:
            databkg[i][b'bkgtype']=3

    databkg = pd.concat(databkg,axis=0)
    databkg = databkg.replace(np.nan, 0)
    databkg[b'evt_scale1fb'] = databkg[b'evt_scale1fb'].abs()
    databkg[b'absweight'] = databkg[b'absweight'].abs()
    databkg[b'w2']=databkg[b'absweight']*databkg[b'absweight']
    databkg[b'sigtag']=0
    print("Background set prepared")
    if isbin and not isinclubin:
        print("Now binning")
        databkg_bin=[]
        for i in range(len(binning.selections)):
            databkg_bin.append(databkg[(databkg[b'ht']>=binning.selections[i][0][0])
                                      &(databkg[b'ht']<binning.selections[i][0][1])
                                      &(databkg[b'nJet30']>=binning.selections[i][1][0])
                                      &(databkg[b'nJet30']<binning.selections[i][1][1])
                                      &(databkg[b'nBJet20']>=binning.selections[i][2][0])
                                      &(databkg[b'nBJet20']<binning.selections[i][2][1])])
        return databkg_bin

    elif isbin and isinclubin:
        print("Now binning inclusive")
        databkg_bin=[]
        for i in range(len(binning.selections_inclu)):
            databkg_bin.append(databkg[(databkg[b'ht']>=binning.selections_inclu[i][0][0])
                                      &(databkg[b'ht']<binning.selections_inclu[i][0][1])
                                      &(databkg[b'nJet30']>=binning.selections_inclu[i][1][0])
                                      &(databkg[b'nJet30']<binning.selections_inclu[i][1][1])
                                      &(databkg[b'nBJet20']>=binning.selections_inclu[i][2][0])
                                      &(databkg[b'nBJet20']<binning.selections_inclu[i][2][1])])
        return databkg_bin
    else:
        return databkg


def loadsig(year=0,isbin=True, isinclubin=False,massskim=True, sig="T1bbbb"):
    mass1con=0
    mass2con=0
    sigfiles=[]
    pd.options.mode.chained_assignment = None
    if sig=="T1bbbb":
        if massskim:
            mass1con=1500
            mass2con=800
        if year==2016:
            sigfiles=sigfiles_T1bbbb_2016
        elif year==2017:
            sigfiles=sigfiles_T1bbbb_2017
        else:
            sigfiles=sigfiles_T1bbbb_all
    elif sig=="T1qqqq":
        if massskim:
            mass1con=1500
            mass2con=800
        if year==2016:
            sigfiles=sigfiles_T1qqqq_2016
        elif year==2017:
            sigfiles=sigfiles_T1qqqq_2017
        else:
            sigfiles=sigfiles_T1qqqq_all

    elif sig=="T2bb":
        if massskim:
            mass1con=900
            mass2con=400
        if year==2016:
            sigfiles=sigfiles_T2bb_2016
        elif year==2017:
            sigfiles=sigfiles_T2bb_2017
        else:
            sigfiles=sigfiles_T2bb_all

    elif sig=="T2qq":
        if massskim:
            mass1con=1500
            mass2con=600
        if year==2016:
            sigfiles=sigfiles_T2qq_2016
        elif year==2017:
            sigfiles=sigfiles_T2qq_2017
        else:
            sigfiles=sigfiles_T2qq_all
    else:
        print("other signals are not supported now")
        return -1

    datasig = pd.concat([pd.DataFrame(np.load(i,allow_pickle=True).item()) for i in sigfiles],axis=0)
    datasig = datasig.replace(np.nan, 0)

    if massskim:
        datasig = datasig[(datasig[b'GenSusyMScan1']>=mass1con)|(datasig[b'GenSusyMScan2']>=mass2con)]

    datasig[b'bkgtype']=-1
    datasig[b'absweight'] = datasig[b'absweight'].abs()
    datasig[b'w2']=datasig[b'absweight']*datasig[b'absweight']
    datasig[b'sigtag']=1
    print("signal set prepared")
    if isbin and not isinclubin:
        print("Now binning")
        datasig_bin=[]
        for i in range(len(binning.selections)):
            datasig_bin.append(datasig[(datasig[b'ht']>=binning.selections[i][0][0])
                                      &(datasig[b'ht']<binning.selections[i][0][1])
                                      &(datasig[b'nJet30']>=binning.selections[i][1][0])
                                      &(datasig[b'nJet30']<binning.selections[i][1][1])
                                      &(datasig[b'nBJet20']>=binning.selections[i][2][0])
                                      &(datasig[b'nBJet20']<binning.selections[i][2][1])])
        return datasig_bin
    elif isbin and isinclubin:
        print("Now binning")
        datasig_bin=[]
        for i in range(len(binning.selections_inclu)):
            datasig_bin.append(datasig[(datasig[b'ht']>=binning.selections_inclu[i][0][0])
                                      &(datasig[b'ht']<binning.selections_inclu[i][0][1])
                                      &(datasig[b'nJet30']>=binning.selections_inclu[i][1][0])
                                      &(datasig[b'nJet30']<binning.selections_inclu[i][1][1])
                                      &(datasig[b'nBJet20']>=binning.selections_inclu[i][2][0])
                                      &(datasig[b'nBJet20']<binning.selections_inclu[i][2][1])])
        return datasig_bin

    else:
        return datasig


def loadSRdata(year=0,isbin=True, isinclubin=False):
#load SRdata for MC data comparison
    pd.options.mode.chained_assignment = None
    SRdatafiles=[]
    if year==2016:
        SRdatafiles = SRdatafiles_2016
    elif year==2017:
        SRdatafiles = SRdatafiles_2017
    elif year==2018:
        SRdatafiles = SRdatafiles_2018
    else:
        SRdatafiles = SRdatafiles_all

    SRdata=[]
    for i in range(len(SRdatafiles)):
        SRdata.append(pd.DataFrame(np.load(SRdatafiles[i],allow_pickle=True).item()))

    SRdata = pd.concat(SRdata,axis=0)
    SRdata = SRdata.replace(np.nan, 0)
    print("data ", SRdata.shape[0])
    print("SRdata set prepared")
    if isbin and not isinclubin:
        print("Now binning")
        SRdata_bin=[]
        for i in range(len(binning.selections)):
            SRdata_bin.append(SRdata[(SRdata[b'ht']>=binning.selections[i][0][0])
                                      &(SRdata[b'ht']<binning.selections[i][0][1])
                                      &(SRdata[b'nJet30']>=binning.selections[i][1][0])
                                      &(SRdata[b'nJet30']<binning.selections[i][1][1])
                                      &(SRdata[b'nBJet20']>=binning.selections[i][2][0])
                                      &(SRdata[b'nBJet20']<binning.selections[i][2][1])])
        return SRdata_bin
    elif isbin and isinclubin:
        print("Now binning")
        SRdata_bin=[]
        for i in range(len(binning.selections_inclu)):
            SRdata_bin.append(SRdata[(SRdata[b'ht']>=binning.selections_inclu[i][0][0])
                                      &(SRdata[b'ht']<binning.selections_inclu[i][0][1])
                                      &(SRdata[b'nJet30']>=binning.selections_inclu[i][1][0])
                                      &(SRdata[b'nJet30']<binning.selections_inclu[i][1][1])
                                      &(SRdata[b'nBJet20']>=binning.selections_inclu[i][2][0])
                                      &(SRdata[b'nBJet20']<binning.selections_inclu[i][2][1])])
        return SRdata_bin
    else:
        return SRdata

def loadcontrol_MC(year=0,isbin=True,isinclubin=False,controltype=""):
#load llcontrol: controltype="llep",load zllcontrol: controltype="zll"
#in this version inclusive CR not supported
    pd.options.mode.chained_assignment = None
    llfiles=[]
    zllfiles=[]
    if year==2016:
        llfiles = llfiles_2016
        zllfiles = zllfiles_2016
    elif year==2017:
        llfiles = llfiles_2017
        zllfiles = zllfiles_2017
    elif year==2018:
        llfiles = llfiles_2018
        zllfiles = zllfiles_2018
    else:
        llfiles = llfiles_MC_all
        zllfiles = zllfiles_MC_all

    controlfiles = []
    if controltype=="llep":
        controlfiles = llfiles
    elif controltype=="zll":
        controlfiles = zllfiles
    else:
        print("please specify a CR type: zll or llep")
        return -1
#        controlfiles = llfiles+zllfiles

    datacontrol=[]
    for i in range(len(controlfiles)):
        datacontrol.append(pd.DataFrame(np.load(controlfiles[i],allow_pickle=True).item()))
        if "singletop" in controlfiles[i]:
            datacontrol[i][b'bkgtype']=0
        elif "tt" in controlfiles[i]:
            datacontrol[i][b'bkgtype']=1
        elif "wjets" in controlfiles[i]:
            datacontrol[i][b'bkgtype']=2
        elif "dyjetsll" in controlfiles[i]:
            datacontrol[i][b'bkgtype']=4

        if "llcontrol" in controlfiles[i]:
            datacontrol[i][b'CRtype']=0
        elif "zinvcontrol" in controlfiles[i]:
            datacontrol[i][b'CRtype']=1

    datacontrol = pd.concat(datacontrol,axis=0)
    datacontrol = datacontrol.replace(np.nan, 0)
    if controltype=="zll":
#        print("before zll selection ",datacontrol.shape[0]," number lep_pdgId0==lep_pdgId1 ",datacontrol[(datacontrol[b'lep_pdgId0']==datacontrol[b'lep_pdgId1'])].shape[0]," number lep_pdgId0==-lep_pdgId1 ",datacontrol[(datacontrol[b'lep_pdgId0']==-datacontrol[b'lep_pdgId1'])].shape[0])
        datacontrol=datacontrol[(datacontrol[b'zll_pt']>200.)&(datacontrol[b'zll_mass']<111.19)&(datacontrol[b'zll_mass']>71.19)&((datacontrol[b'lep_pdgId0']+datacontrol[b'lep_pdgId1'])==0)]
#        print("after zll selection ",datacontrol.shape[0])
    datacontrol[b'evt_scale1fb'] = datacontrol[b'evt_scale1fb'].abs()    
#    datacontrol[b'absweight'] = datacontrol[b'absweight'].abs()
    datacontrol[b'w2']=datacontrol[b'absweight']*datacontrol[b'absweight']
    datacontrol[b'sigtag']=0
    print("control set MC prepared")
    if isbin and not isinclubin:
        print("Now binning")
        datacontrol_bin=[]
        for i in range(len(binning.selections)):
            if controltype=="llep":
                datacontrol_bin.append(datacontrol[(datacontrol[b'ht']>=binning.selections[i][0][0])
                                      &(datacontrol[b'ht']<binning.selections[i][0][1])
                                      &(datacontrol[b'nJet30']>=binning.selections[i][1][0])
                                      &(datacontrol[b'nJet30']<binning.selections[i][1][1])
                                      &(datacontrol[b'nBJet20']>=binning.selections[i][2][0])
                                      &(datacontrol[b'nBJet20']<binning.selections[i][2][1])])
            elif controltype=="zll":
                datacontrol_bin.append(datacontrol[(datacontrol[b'zll_ht']>=binning.selections[i][0][0])
                                      &(datacontrol[b'zll_ht']<binning.selections[i][0][1])
                                      &(datacontrol[b'nJet30']>=binning.selections[i][1][0])
                                      &(datacontrol[b'nJet30']<binning.selections[i][1][1])
                                      &(datacontrol[b'nBJet20']>=binning.selections[i][2][0])
                                      &(datacontrol[b'nBJet20']<binning.selections[i][2][1])])
        return datacontrol_bin
    elif isbin and isinclubin:
        print("Now binning")
        datacontrol_bin=[]
        for i in range(len(binning.selections_inclu)):
            if controltype=="llep":
                datacontrol_bin.append(datacontrol[(datacontrol[b'ht']>=binning.selections_inclu[i][0][0])
                                      &(datacontrol[b'ht']<binning.selections_inclu[i][0][1])
                                      &(datacontrol[b'nJet30']>=binning.selections_inclu[i][1][0])
                                      &(datacontrol[b'nJet30']<binning.selections_inclu[i][1][1])
                                      &(datacontrol[b'nBJet20']>=binning.selections_inclu[i][2][0])
                                      &(datacontrol[b'nBJet20']<binning.selections_inclu[i][2][1])])
            elif controltype=="zll":
                datacontrol_bin.append(datacontrol[(datacontrol[b'zll_ht']>=binning.selections_inclu[i][0][0])
                                      &(datacontrol[b'zll_ht']<binning.selections_inclu[i][0][1])
                                      &(datacontrol[b'nJet30']>=binning.selections_inclu[i][1][0])
                                      &(datacontrol[b'nJet30']<binning.selections_inclu[i][1][1])
                                      &(datacontrol[b'nBJet20']>=binning.selections_inclu[i][2][0])
                                      &(datacontrol[b'nBJet20']<binning.selections_inclu[i][2][1])])
        return datacontrol_bin
    else:
        return datacontrol


def loadcontrol_data(year=0,isbin=True,isinclubin=False,controltype=""):
#load llcontrol: controltype="llep",load zllcontrol: controltype="zll"
#in this version the inclusive CR is not supported
    pd.options.mode.chained_assignment = None
    llfiles=[]
    zllfiles=[]
    if year==2016:
        llfiles = llfiles_2016_data
        zllfiles = zllfiles_2016_data
    elif year==2017:
        llfiles = llfiles_2017_data
        zllfiles = zllfiles_2017_data
    elif year==2018:
        llfiles = llfiles_2018_data
        zllfiles = zllfiles_2018_data
    else:
        llfiles = llfiles_data_all
        zllfiles = zllfiles_data_all

    controlfiles = []
    if controltype=="llep":
        controlfiles = llfiles
    elif controltype=="zll":
        controlfiles = zllfiles
    else:
        print("please specify a CR type: zll or llep")
        return -1
#        controlfiles = llfiles+zllfiles

    datacontrol=[]
    for i in range(len(controlfiles)):
        datacontrol.append(pd.DataFrame(np.load(controlfiles[i],allow_pickle=True).item()))
        if "llcontrol" in controlfiles[i]:
            datacontrol[i][b'CRtype']=0
        elif "zinvcontrol" in controlfiles[i]:
            datacontrol[i][b'CRtype']=1

    datacontrol = pd.concat(datacontrol,axis=0)
    datacontrol = datacontrol.replace(np.nan, 0)
    print("data ",controltype, datacontrol.shape[0])
    if controltype=="zll":
#        print("before zll selection ",datacontrol.shape[0]," number lep_pdgId0==lep_pdgId1 ",datacontrol[(datacontrol[b'lep_pdgId1']==datacontrol[b'lep_pdgId0'])].shape[0]," number lep_pdgId0==-lep_pdgId1 ",datacontrol[(datacontrol[b'lep_pdgId0']==-datacontrol[b'lep_pdgId1'])].shape[0])
        datacontrol=datacontrol[(datacontrol[b'zll_pt']>200.)&(datacontrol[b'zll_mass']<111.19)&(datacontrol[b'zll_mass']>71.19)&((datacontrol[b'lep_pdgId0']+datacontrol[b'lep_pdgId1'])==0)]
#        print("after zll selection ",datacontrol.shape[0])
    print("control set data prepared")
    if isbin and not isinclubin:
        print("Now binning")
        datacontrol_bin=[]
        for i in range(len(binning.selections)):
            if controltype=="llep":
                datacontrol_bin.append(datacontrol[(datacontrol[b'ht']>=binning.selections[i][0][0])
                                      &(datacontrol[b'ht']<binning.selections[i][0][1])
                                      &(datacontrol[b'nJet30']>=binning.selections[i][1][0])
                                      &(datacontrol[b'nJet30']<binning.selections[i][1][1])
                                      &(datacontrol[b'nBJet20']>=binning.selections[i][2][0])
                                      &(datacontrol[b'nBJet20']<binning.selections[i][2][1])])
            elif controltype=="zll":
                datacontrol_bin.append(datacontrol[(datacontrol[b'zll_ht']>=binning.selections[i][0][0])
                                      &(datacontrol[b'zll_ht']<binning.selections[i][0][1])
                                      &(datacontrol[b'nJet30']>=binning.selections[i][1][0])
                                      &(datacontrol[b'nJet30']<binning.selections[i][1][1])
                                      &(datacontrol[b'nBJet20']>=binning.selections[i][2][0])
                                      &(datacontrol[b'nBJet20']<binning.selections[i][2][1])])
        return datacontrol_bin

    elif isbin and isinclubin:
        print("Now binning")
        datacontrol_bin=[]
        for i in range(len(binning.selections_inclu)):
            if controltype=="llep":
                datacontrol_bin.append(datacontrol[(datacontrol[b'ht']>=binning.selections_inclu[i][0][0])
                                      &(datacontrol[b'ht']<binning.selections_inclu[i][0][1])
                                      &(datacontrol[b'nJet30']>=binning.selections_inclu[i][1][0])
                                      &(datacontrol[b'nJet30']<binning.selections_inclu[i][1][1])
                                      &(datacontrol[b'nBJet20']>=binning.selections_inclu[i][2][0])
                                      &(datacontrol[b'nBJet20']<binning.selections_inclu[i][2][1])])
            elif controltype=="zll":
                datacontrol_bin.append(datacontrol[(datacontrol[b'zll_ht']>=binning.selections_inclu[i][0][0])
                                      &(datacontrol[b'zll_ht']<binning.selections_inclu[i][0][1])
                                      &(datacontrol[b'nJet30']>=binning.selections_inclu[i][1][0])
                                      &(datacontrol[b'nJet30']<binning.selections_inclu[i][1][1])
                                      &(datacontrol[b'nBJet20']>=binning.selections_inclu[i][2][0])
                                      &(datacontrol[b'nBJet20']<binning.selections_inclu[i][2][1])])
        return datacontrol_bin
    else:
        return datacontrol






