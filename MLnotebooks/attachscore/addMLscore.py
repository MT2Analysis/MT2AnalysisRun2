import sys

inputfile=str(sys.argv[1])
outputfile=str(sys.argv[2])
model=str(sys.argv[3])

import uproot
import numpy as np
from keras.models import load_model
from branches_V01 import features_train_1
from branches_V01 import features_train_2
#from ROOT import TFile, TTree
filename_evaluate=inputfile
filename_friend=outputfile

with uproot.open(filename_evaluate) as f:
  events = f["mt2"]
#  events=f['Events']
  ars={}
  # column stack with the **same** order of columns as they were given to the NN
  if "btagDeepCSV_excluded" in model:
    features=features_train_2
  else:
    features=features_train_1
  for i,ib in enumerate(features):
    print('Reading branch=', ib)
    ars[ib] = events.array(ib)
    ars[ib] = ars[ib].reshape((ars[ib].shape[0],1))
    if i==0: X = ars[ib]
    else: X = np.hstack((X, ars[ib]))

print('X=', X)
print('shape=', X.shape)

print('\nLoading model')
loaded_model = load_model(model)
prediction = loaded_model.predict(X)

prediction = prediction.reshape(prediction.shape[0])
#print('reshape prediction is=',prediction )
print('shape=',prediction.shape )
np.save(filename_friend,prediction)
print(filename_friend," saved")
#f1=TFile(filename_friend,"recreate")
#tree1=TTree("mt2_friend","mt2_friend")
#score=array('d',[0])
#tree1.Branch("score_V01",score,"score_V01/D")
#for i in range(prediction.shape[0]):
#    score[0]=prediction[i]
#    tree1.Fill()
#f1.Write()
#f1.Close()


#with uproot.recreate(filename_friend) as f:
#  f["mt2_friend"] = uproot.newtree({"score_V01": "float64"})
#  f["mt2_friend"].extend({"score_V01": prediction})

