#The information of benchmark points, their corresponding sensitive bins, tags marking ML set - analysis set spliting, the splitting fractions and significance change wrt cut are saved in directories.

models_forbin={'mass':[[1400,1350],[2000,1600],[2400,1400],[2400,200]],
               'binindex':[[33,19,20,26,25,16,27,34,18,65,23,32,66,55,15,45,46,54,84],
                          [20,47,13,46,27,57,34,66],
                          [34,27,33,66,67,65],
                          [33,34,65,32,66,64,67]],
               'significance':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
#               'trainplots':[],
#               'train_val_plots':[],
#               'bkg_sig_plots':[],
#               'CR_plots':[]
#               'significance_plots':[]
}

models_debug3={'mass':[[1400,1350],[2000,1600],[2400,1400],[2400,200]],
               'binindex':[[33,19,20,26,25,16,34,18,65,23,32,55,15,45,46,54,84],
                          [20,13,46,57,34],
                          [34,33,65],
                          [33,34,65,32,64]],
               'significance':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
#               'trainplots':[],
#               'train_val_plots':[],
#               'bkg_sig_plots':[],
#               'CR_plots':[]
#               'significance_plots':[]
}

models_T1bbbb={'mass':[[1400,1350],[2000,1600],[2400,1400],[2400,200]],
               'binindex':[[33,19,20,26,25,16,34,18,65,23,32,55,15,45,46,54],
                          [20,13,46,57,34],
                          [34,33,65],
                          [33,34,65,32,64]],
               'significance':[],
               'zlldataCR':[],
               'llepdataCR':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
#               'trainplots':[],
#               'train_val_plots':[],
#               'bkg_sig_plots':[],
#               'CR_plots':[]
#               'significance_plots':[]
}

models_T1bbbb_s={'mass':[[1400,1350],[2000,1600],[2400,1400],[2400,200]],
               'binindex':[[19,26,25,16,18,23,55,15,45,54],
                          [],
                          [],
                          []],
               'significance':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
#               'trainplots':[],
#               'train_val_plots':[],
#               'bkg_sig_plots':[],
#               'CR_plots':[]
#               'significance_plots':[]
}

models_debug1={'mass':[[2400,200]],
               'binindex':[[33]],
               'significance':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
#               'trainplots':[],
#               'train_val_plots':[],
#               'bkg_sig_plots':[],
#               'CR_plots':[],
#               'significance_plots':[]
}

models_debug2={'mass':[[1400,1350],[2400,200]],
               'binindex':[[33,19],[33,34]],
               'significance':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
#               'trainplots':[],
#               'train_val_plots':[],
#               'bkg_sig_plots':[],
#               'CR_plots':[]
#               'significance_plots':[]
}


models_T1qqqq={'mass':[[1400,1300],[1800,1200],[2200,200],[2200,800]],
               'binindex':[[63,43,68,53,44,54,31,24,64,25,48,17,32],
                           [63,53,17,24,68,64,43,25,54,31,18,44,26],
                           [31,63,64,32,68,33,65,28],
                           [31,63,32,64,68,33,65,28]],
               'significance':[],
               'zlldataCR':[],
               'llepdataCR':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
}

models_T1qqqq_s={'mass':[[1400,1300],[1800,1200],[2200,200],[2200,800]],
               'binindex':[[48],
                           [],
                           [],
                           []],
               'significance':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
}

models_T2bb={'mass':[[1100,700],[1400,100],[1400,500],[800,700]],
               'binindex':[[16,19,9,15,26,33,22,20,23,18,25,32,34,8,65,45,29,2,64],
                           [29,22,32,33,28,23,25,26,16,15,31,64,21,65,18,34,19],
                           [29,22,16,32,15,33,23,25,26,28,19,18,64,31],
                           [19,45,26,16,65,20,9,33,12,55,25,18,5,46,54,34,32,13,2,60,56,44,15,11]],
               'significance':[],
               'zlldataCR':[],
               'llepdataCR':[], 
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
}

models_T2bb_s={'mass':[[1100,700],[1400,100],[1400,500],[800,700]],
               'binindex':[[],
                           [],
                           [],
                           [12,55,5,46,54,13,60,56,44,11]],
               'significance':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]
}
            
'''
models_T2qq={'mass':[[1000,600],[1300,400],[1400,200],[800,600]],
               'binindex':[[14,17,24,31,63,21,53,18,43,25,15,28,22,32,7,54,64,44,8,19,33,68],
                           [28,21,14,31,24,17,63,22,25,32,29,15,53,18,64,54],
                           [28,31,21,24,14,32,63,29,22,17,25,64],
                           [43,17,53,24,63,31,25,44,18,54,64,10,7,48,14,32,68,0,15,11,8,55,39,3,21,19]],
               'significance':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[]
}
'''

models_T2qq={'mass':[[1900,200],[1900,600],[1500,900],[1100,900]],
             'binindex':[[28,31,32,29,63,21],
                         [28,31,21,29,32,63,24],
                         [14,21,28,31,17,24,63,15,25,22,18,53,32,64,29,43,54],
                         [24,53,31,32,43,17,63,25,64,54,18,48,29,11,10,44,14,7,49,45,19,15,65,21,0,26,8,39]],
               'significance':[],
               'zlldataCR':[],
               'llepdataCR':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]

}

models_T2qq_s={'mass':[[1900,200],[1900,600],[1500,900],[1100,900]],
             'binindex':[[],
                         [],
                         [],
                         [48,11,10,44,7,49,45,19,65,0,26,8,39]],
               'significance':[],
               'traintag':[],
               'trainfraction':[],
               'mt2_score_corr':[],
               'scalers':[]

}


def spliting(dataset_bin):
    MLuse=[]
    MLuse_tag=[]
    dataset_ML_bin=[]
    dataset_test_bin=[]
    for i in range(len(dataset_bin)):
        if dataset_bin[i].shape[0]<20000:
            dataset_ML_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']<10])
            dataset_test_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']>=10])
            dataset_ML_bin[i]=dataset_ML_bin[i].reset_index(drop=True)
            dataset_test_bin[i]=dataset_test_bin[i].reset_index(drop=True)
            MLuse.append(0.5)
            MLuse_tag.append([0,1,2,3,4,5,6,7,8,9])
        elif dataset_bin[i].shape[0]<40000:
            dataset_ML_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']<5])
            dataset_test_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']>=5])
            dataset_ML_bin[i]=dataset_ML_bin[i].reset_index(drop=True)
            dataset_test_bin[i]=dataset_test_bin[i].reset_index(drop=True)
            MLuse.append(0.25)
            MLuse_tag.append([0,1,2,3,4])
        elif dataset_bin[i].shape[0]<80000:
            dataset_ML_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']<3])
            dataset_test_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']>=3])
            dataset_ML_bin[i]=dataset_ML_bin[i].reset_index(drop=True)
            dataset_test_bin[i]=dataset_test_bin[i].reset_index(drop=True)
            MLuse.append(0.15)
            MLuse_tag.append([0,1,2])
        else:
            dataset_ML_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']<2])
            dataset_test_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']>=2])
            dataset_ML_bin[i]=dataset_ML_bin[i].reset_index(drop=True)
            dataset_test_bin[i]=dataset_test_bin[i].reset_index(drop=True)
            MLuse.append(0.1)
            MLuse_tag.append([0,1])

    return (dataset_ML_bin,dataset_test_bin,MLuse,MLuse_tag)


def halfspliting(dataset_bin):
    MLuse=[]
    MLuse_tag=[]
    dataset_ML_bin=[]
    dataset_test_bin=[]
    for i in range(len(dataset_bin)):
        dataset_ML_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']<10])
        dataset_test_bin.append(dataset_bin[i][dataset_bin[i][b'MLtag']>=10])
        dataset_ML_bin[i]=dataset_ML_bin[i].reset_index(drop=True)
        dataset_test_bin[i]=dataset_test_bin[i].reset_index(drop=True)
        MLuse.append(0.5)
        MLuse_tag.append([0,1,2,3,4,5,6,7,8,9])
    return (dataset_ML_bin,dataset_test_bin,MLuse,MLuse_tag)

