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


cfg = "moriond2019_35p9ifb" #"data_2016_SnT_36p8_FixedWJets"  #data_2016_SnTMC_362ifb_18ifbUB_Signal" #"data_Run2016_7p7ifb"
stepSize = 5 if "T2cc" in model else 50 if "T2qq" in model else 25  # 25 is previous step used, it is the right one to use close to the diagonal

#M = range(600 if "T1" in model else 100 if "T2tt"==model else 100 if "T2cc"==model else 300, 2301 if "T1" in model else 1801 if "T2qq"==model else 500,stepSize) 
# previous ranges abobe
#M = range(700 if "T1" in model else 200 if "T2tt"==model else 200 if "T2cc"==model else 400, 2401 if "T1" in model else 1901 if "T2qq"==model else 600,stepSize)
#print M
# NOTE: range is only for T1qqqq -> need to be adjusted
if model=="T1qqqq": 
  Ms = range(700, 2301, stepSize)
  ms = range(500, 1601, stepSize)
elif model=="T2qq":
  Ms = range(300, 1801, stepSize)
  ms = range(100, 1401, stepSize)

print Ms

logsDir="jobs_{}_{}_{}".format(model,cfg,label)
os.system("mkdir {}".format(logsDir))

for M in Ms:
  for m in ms:
    m1=M
    m2=M+stepSize
    m11=m
    m22=m+stepSize
    if m11 >= m2: continue

    job_name = "{}_{}_{}_{}".format(m1, m2, m11, m22)
    stdout = '`pwd`/{}/log_{}.out'.format(logsDir, job_name)
    stderr = '`pwd`/{}/log_{}.err'.format(logsDir, job_name)
    name = 'creatingDatacards_{}_{}'.format(model, job_name) 
    command="qsub -l h_vmem=6g -q short.q -o {} -e {} -N {} createDatacards_batch.sh {} {} {} {} {} {} {}".format(stdout, stderr, name, cfg, model, m1, m2, m11, m22, label) # it is important to require higher possible memory

    print command
    os.system(command)
