''' 
Script to launch the datacard creation for the signals
python launchCreateDatacards.py T2qq <label>
Currently sends one job per mass mass point
'''

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


cfg = "moriond2019_35p9ifb" # config file name for 2016, where signals are stored
stepSize = 50 # 25 is the smallest interval between points in the diagonal

if model=="T1qqqq": 
  Ms = range(600, 2801, stepSize)
  ms = range(0, 1901, stepSize) 
#  Ms = range(2100, 2101, stepSize)
#  ms = range(0, 26, stepSize) 
elif model=="T2qq":
  Ms = range(200, 2601, stepSize)
  ms = range(0, 2201, stepSize) 
elif model=="T1bbbb":
  Ms = range(600, 2801, stepSize)
  ms = range(0, 1901, stepSize)
elif model=="T2bb":
  Ms = range(300, 2601, stepSize)
  ms = range(0, 2101, stepSize)

print Ms

logsDir="jobs_dcc_{}_{}_{}".format(model,cfg,label)
os.system("mkdir {}".format(logsDir))

for M in Ms:
  for m in ms:
    m1=M
    m2=M+stepSize
    m11=m
    m22=m+stepSize
    if m11 >= m2: continue

    job_name = "{}_{}_{}_{}".format(m1, m2, m11, m22)
    stdout = '`pwd`/{}/log_{}.log'.format(logsDir, job_name)
    name = 'creatingDatacards_{}_{}'.format(model, job_name) 

    command="sbatch -p wn --account=cn-test -o {} -e {} --job-name={} --time=0-00:30 --ntasks=1  createDatacards_batch_fromHome.sh {} {} {} {} {} {} {}".format(stdout, stdout, name, cfg, model, m1, m2, m11, m22, label) # add a requirement on the memory? # --mem 6000 #(6GB)

    print command
    os.system(command)
