models_forbin={'mass':[[1400,1350],[2000,1600],[2400,1400],[2400,200]],
               'binindex':[[33,19,20,26,25,16,27,34,18,65,23,32,66,55,15,45,46,54,84],
                          [20,47,13,46,27,57,34,66],
                          [34,27,33,66,67,65],
                          [33,34,65,32,66,64,67]],
               'significance':[],
               'traintag':[],
               'trainfraction':[]#,
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
               'trainfraction':[]#,
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
               'trainfraction':[]#,
#               'trainplots':[],
#               'train_val_plots':[],
#               'bkg_sig_plots':[],
#               'CR_plots':[]
#               'significance_plots':[]
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

