# Script to check the first failed cut for a given list of events


import os
import re
import ROOT

def getEventListFromTxt(list):
  events = []
  if not os.path.isfile(list):
    raise RuntimeError('list {} is not a valid path'.format(list))

  file=open(list,  'r')
  for line in file:
    if '(' not in line: continue
    if ':' in line: continue # only select the lines where ETH misses the events and SnT does not
    els = re.split('\(|\)|\,|\s', line)
    els = filter(lambda x: x!='', els)

    run=int(els[0])
    event=int(els[1])

    events.append([run, event])
    #print events

  return events

def isInterestingEvent(e, eventList):
  for (evt,run) in eventList:
    #print (evt,run), (e.run, e.event)
    if (evt,run)==(e.run,e.event):
      return True # return true as soon as you find an interesting event

  return False


def stamp(str1, str2):
   print '{} : {}'.format(str1, str2)

def findFailedCut(e,sel='ZllCR', year='2017'):

 evtRun='({:6d},{:12d})'.format(e.run, e.event)

 if year=='2017':
   # filters missing
   # triggers missing

   # kinematic selections common to SF and OF
   if e.nLep!=2: stamp(evtRun, 'nLep={}'.format(e.nLep))
   if e.nLep>0 and e.lep_pt[0]<100: stamp(evtRun, 'lep_pt[0]={}'.format(e.lep_pt[0]) )
   if e.nLep>1 and e.lep_pt[1]<30:  stamp(evtRun, 'lep_pt[1]={}'.format(e.lep_pt[1]) )
   if e.nJet30<=1: stamp(evtRun, 'nJet30={}'.format(e.nJet30))
   if e.nJet30FailId!=0: stamp(evtRun, 'nJet30FailId={}'.format(e.nJet30FailId))
   if e.zll_deltaPhiMin < 0.3: stamp(evtRun, 'zll_deltaPhiMin={}'.format(e.zll_deltaPhiMin))
   if e.zll_diffMetMht > 0.5 * e.zll_met_pt: stamp(evtRun, 'zll_diffMetMht={}, 0.5*zll_met_pt={}'.format(e.zll_diffMetMht, 0.5*e.zll_met_pt))
   if not ( (e.nJet30>1 and e.zll_ht<1000. and e.zll_met_pt>250.) or (e.nJet30>1 and e.zll_ht>=1000. and e.zll_met_pt>30.) or (e.nJet30==1 and e.zll_met_pt>250.)):
     stamp(evtRun, 'nJet30={} zll_ht={} zll_met_pt={}'.format(e.nJet30,e.zll_ht,e.zll_met_pt))
   if e.nLep>1 and (e.lep_pdgId[0]*e.lep_pdgId[1])>0: stamp(evtRun, 'lep_pdgId[0]={}, lep_pdgId[1]={}'.format(e.lep_pdgId[0], e.lep_pdgId[1]))

   if 'OF' in sel:
     if e.nLep>1 and e.lep_pdgId[0] == -e.lep_pdgId[1]: stamp(evtRun, 'lep_pdgId[0]={}, lep_pdgId[1]={}'.format(e.lep_pdgId[0], e.lep_pdgId[1]))

   elif 'SF' in sel:
     if e.nLep>1 and e.lep_pdgId[0] != -e.lep_pdgId[1]: stamp(evtRun, 'lep_pdgId[0]={}, lep_pdgId[1]={}'.format(e.lep_pdgId[0], e.lep_pdgId[1]))

   if 'nominalZ' in sel:
    if (e.zll_mass-91.19)>=20: stamp(evtRun, 'zll_mass={}'.format(e.zll_mass))
    if (e.zll_pt <= 200.):  stamp(evtRun, 'zll_pt={}'.format(e.zll_pt))

   elif 'invertedZ' in sel:
    if (e.zll_mass-91.19)<20: stamp(evtRun, 'zll_mass={}'.format(e.zll_mass))
    if (e.zll_pt > 200.):  stamp(evtRun, 'zll_pt={}'.format(e.zll_pt))


if __name__ == "__main__":

  list='evtLists/list_Bennett_2018_12_04.txt'

  path = '/scratch/mratti/merged_nanoMT2/2017_V01_V00_V0/merged/'
  inputFiles=[
    path + 'merged_Run2017B_noDupl.root',
    path + 'merged_Run2017C_noDupl.root',
    path + 'merged_Run2017D_noDupl.root',
    path + 'merged_Run2017E_noDupl.root',
    path + 'merged_Run2017F_noDupl.root',
  ]

  eventList=getEventListFromTxt(list=list)

  # now need to loop over my input file and consider that particular event and all its branches ...
  chain = ROOT.TChain('Events')
  for inputFile in inputFiles:
    chain.Add(inputFile)

  for e in chain:
    # check if event is one of those interesting
    if isInterestingEvent(e,eventList):
      print 'Found event to be checked, run={} event={}'.format(e.run,e.event)

      # now check why this event fails the selection
      findFailedCut(e=e,sel='ZllCR_OF_invertedZ', year='2017')
