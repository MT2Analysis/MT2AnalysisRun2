''' 
Script to submit to batch the limit analysis for all benchmark points.
Steps will be:
 - datacard creation for signals
 - datacards combination
 - limit calculation
Will save all output, including intermediate output, to the analysis directory

Example of usage:
python launchLimitAnalysis.py --cfg moriond2019_35p9ifb --ver V00 --model T1qqqq --time 23
'''

import os
import sys


class SigPoint(object):
  def __init__(self, model='T1qqqq', M='1200', m='50'):
    self.model = model
    self.M = M
    self.m = m
    self.name = model+'_'+M+'_'+m



def getOptions():
  from argparse import ArgumentParser
  parser = ArgumentParser(description='helper for limit analysis', add_help=True)

  parser.add_argument('--cfg', type=str, dest='cfg', help='version of this production, i.e. the config file name', default='moriond2019_35p9ifb')
  parser.add_argument('--ver', type=str, dest='ver', help='an additional handle to versionize', default='V01')
  parser.add_argument('--time', type=str, dest='time', help='allowed time for full job in hours', default='05')
  parser.add_argument('--model', type=str, dest='model', help='which model to run', default='T1qqqq', choices=['T1qqqq', 'T1bbbb', 'T2qq', 'T2bb'])

  return parser.parse_args()


def getSignalPoints(model):

  sigPoints = []

  if model=='T1qqqq':  
    sigPoints.append(SigPoint(model='T1qqqq', M='1400', m='1300'))
    sigPoints.append(SigPoint(model='T1qqqq', M='1800', m='1200'))
    sigPoints.append(SigPoint(model='T1qqqq', M='2200', m='200'))
    sigPoints.append(SigPoint(model='T1qqqq', M='2200', m='800'))
    sigPoints.append(SigPoint(model='T1qqqq', M='1600', m='1500'))
    sigPoints.append(SigPoint(model='T1qqqq', M='2000', m='1400'))
    sigPoints.append(SigPoint(model='T1qqqq', M='2400', m='200'))
    sigPoints.append(SigPoint(model='T1qqqq', M='2400', m='800'))
    # etc.
  elif model=='T1bbbb':
    sigPoints.append(SigPoint(model='T1bbbb', M='1400', m='1350'))
    sigPoints.append(SigPoint(model='T1bbbb', M='2000', m='1600'))
    sigPoints.append(SigPoint(model='T1bbbb', M='2400', m='200'))
    sigPoints.append(SigPoint(model='T1bbbb', M='2400', m='1400'))
    sigPoints.append(SigPoint(model='T1bbbb', M='1600', m='1550'))
    sigPoints.append(SigPoint(model='T1bbbb', M='2200', m='1800'))
    sigPoints.append(SigPoint(model='T1bbbb', M='2600', m='200'))
    sigPoints.append(SigPoint(model='T1bbbb', M='2600', m='1400'))
  elif model=='T2bb':
    sigPoints.append(SigPoint(model='T2bb', M='800', m='700'))
    sigPoints.append(SigPoint(model='T2bb', M='1100', m='700'))
    sigPoints.append(SigPoint(model='T2bb', M='1400', m='100'))
    sigPoints.append(SigPoint(model='T2bb', M='1400', m='500'))
    sigPoints.append(SigPoint(model='T2bb', M='1000', m='900'))
    sigPoints.append(SigPoint(model='T2bb', M='1300', m='900'))
    sigPoints.append(SigPoint(model='T2bb', M='1600', m='200'))
    sigPoints.append(SigPoint(model='T2bb', M='1600', m='500'))
  elif model=='T2qq':
    sigPoints.append(SigPoint(model='T2qq', M='1100', m='900'))
    sigPoints.append(SigPoint(model='T2qq', M='1500', m='900'))
    sigPoints.append(SigPoint(model='T2qq', M='1900', m='200'))
    sigPoints.append(SigPoint(model='T2qq', M='1900', m='600'))
    sigPoints.append(SigPoint(model='T2qq', M='1300', m='1100'))
    sigPoints.append(SigPoint(model='T2qq', M='1700', m='1100'))
    sigPoints.append(SigPoint(model='T2qq', M='2100', m='200'))
    sigPoints.append(SigPoint(model='T2qq', M='2100', m='600'))
  else:
    pass
    #etc.

  return sigPoints

if __name__ == "__main__":

  opt = getOptions()
  
  # get the signal points
  sigPoints = getSignalPoints(opt.model)  

  # prepare for logs
  logsDir="jobs_LimAna_{}_{}_{}".format(opt.cfg,opt.ver,opt.model)
  os.system("mkdir {}".format(logsDir))

  # prepare for each job
  for sp in sigPoints:

    stdout = '`pwd`/{}/log_{}.log'.format(logsDir, sp.name)

    command="sbatch -p wn --mem=10000 --account=t3 -o {o} -e {o} --job-name={n} --time=0-{t}:00 --ntasks=1  LimitAnalysis.sh {cfg} {ver} {mod} {M} {Mup} {m} {mup}".format(
                  o=stdout, n=sp.name, t=opt.time, cfg=opt.cfg, ver=opt.ver, mod=opt.model, M=sp.M, Mup=str(int(sp.M)+5), m=sp.m, mup=str(int(sp.m)+5)) 

    print command
#    os.system(command)
    







