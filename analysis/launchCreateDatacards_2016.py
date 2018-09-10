# Script to launch the datacard creation for the signals
# python launchCreateDatacards_2016.py T2qq <label>
# Currently sends one job per mass mass point

import os
import sys
from sys import argv

if len(argv)>1:
    model = argv[1]
else:
    model = "T1tttt"

if len(argv)>2:
    label = argv[2]
else:
    label = ""


cfg = "dataETH_SnTMC_35p9ifb" #"data_2016_SnT_36p8_FixedWJets"  #data_2016_SnTMC_362ifb_18ifbUB_Signal" #"data_Run2016_7p7ifb"
stepSize = 5 if "T2cc" in model else 25

M = range(600 if "T1" in model else 100 if "T2tt"==model else 100 if "T2cc"==model else 300, 2301 if "T1" in model else 1801 if "T2qq"==model else 500,stepSize)

os.system("mkdir jobs_"+model+"_2016")

for i,m in enumerate(M):
  m1 = m
  m2 = m1+stepSize
  Y = range(0,m1+1,stepSize)
  for j,y in enumerate(Y):
    m11=y
    m22=Y[j+1] if j+1 < len(Y) else y+stepSize
    if m11 >= m2: continue

    job_name = "{}_{}_{}_{}".format(m1, m2, m11, m22)
    stdout = '`pwd`/jobs_{}_2016/log_{}.out'.format(model, job_name)
    stderr = '`pwd`/jobs_{}_2016/log_{}.err'.format(model, job_name)
    name = 'creatingDatacards_{}_{}'.format(model, job_name)# not sure what this is
    command="qsub -l h_vmem=6g -q short.q -o {} -e {} -N {} createDatacards_batch_2016.sh {} {} {} {} {} {} {}".format(stdout, stderr, name, cfg, model, m1, m2, m11, m22, label)

    print command
    os.system(command)
