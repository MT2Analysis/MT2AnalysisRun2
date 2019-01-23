# Script to check the first failed cut for a given list of events


import os
import re
import ROOT
import sys
sys.path.append('{user}/plotting/myplotting'.format(user=os.environ['HOME']))
from spares import *


if __name__ == "__main__":

  ROOT.gROOT.SetBatch(True)
  ROOT.gROOT.ProcessLine('.L ~/CMS_style/tdrstyle2D.C')
  ROOT.gROOT.ProcessLine('setTDRStyle2D()')
  ROOT.TH1.SetDefaultSumw2()


  path = '/scratch/mratti/merged_nanoMT2/2018_V04_V01/merged/'
  inputFiles=[
    path + 'merged_Run2018A_noDupl.root',
    path + 'merged_Run2018B_noDupl.root',
    path + 'merged_Run2018C_noDupl.root',
    path + 'merged_Run2018D_noDupl.root',
  ]

  
  h_IsoTk_all_phi_eta = ROOT.TH2F("h_IsoTk_all_phi_eta", "h_IsoTk_all_phi_eta", 32, -3.2, 3.2, 30, -3., 3.)
  h_IsoTk_ele_phi_eta = ROOT.TH2F("h_IsoTk_ele_phi_eta", "h_IsoTk_ele_phi_eta", 32, -3.2, 3.2, 30, -3., 3.)
  h_IsoTk_mu_phi_eta = ROOT.TH2F("h_IsoTk_mu_phi_eta", "h_IsoTk_mu_phi_eta", 32, -3.2, 3.2, 30, -3., 3.)

  histos = [h_IsoTk_all_phi_eta, h_IsoTk_ele_phi_eta, h_IsoTk_mu_phi_eta]

  lepSels = ["(1)", "(abs(isoTrack_pdgId[0])==11)",  "(abs(isoTrack_pdgId[0])==13)", "(abs(isoTrack_pdgId[0])==11 && isoTrack_id[0]>=3)"] 

  lepType = ["pf lepton", "pf electron", "pf muon"]

  zMaxes = [150, 80, 80]

  trigger = "(HLT_PFHT1050 || HLT_PFJet500 || HLT_PFHT500_PFMET100_PFMHT100_IDTight || HLT_PFMET120_PFMHT120_IDTight || HLT_PFMETNoMu120_PFMHTNoMu120_IDTight || HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60 )"
  filters = "(Flag_goodVertices && Flag_globalSuperTightHalo2016Filter && Flag_HBHENoiseFilter && Flag_HBHENoiseIsoFilter && Flag_EcalDeadCellTriggerPrimitiveFilter && Flag_BadPFMuonFilter && Flag_BadChargedCandidateFilter && Flag_eeBadScFilter && Flag_ecalBadCalibFilter && PV_npvs>0 && nJet30FailId==0)"
  kinematics = "(ht > 250 && met_pt>250  && nJet30>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met_pt && mt2>200. && nPFLep5LowMTclean==1 && nLepHighMT==0)"
  passHEMveto = "Sum$(jet_pt > 30 && jet_eta > -4.7 && jet_eta < -1.4 && jet_phi > -1.6 && jet_phi < -0.8 ) == 0"
  #lepton = "(1)"#"(run>=319077)"

  # now need to loop over my input file and consider that particular event and all its branches ...
  chain = ROOT.TChain('Events')
  for inputFile in inputFiles:
    chain.Add(inputFile)


  for i,h in enumerate(histos):

    lepSel = lepSels[i]
    sel = "({} && {} && {} && {} && {})".format(trigger, filters, kinematics, passHEMveto, lepSel)

    chain.Project(h.GetName(), "isoTrack_eta[0]:isoTrack_phi[0]",  sel, "colz" )

  #chain.Project(h_recoLep_all_phi_eta_tightId.GetName(), "lep_eta[0]:lep_phi[0]",  sel + "&&( (lep_id[0]>2 && abs(lep_pdgId[0])==11) || (lep_id[0]>0 && abs(lep_pdgId[0])==13))", "colz" )

    c =  ROOT.TCanvas()

    label_nTot = "N evts tot = {}".format(h.GetEntries())
    label_HEM = "N evts HEM = {}".format(h.Integral(10,11,4,7)) # dummy bin range
    #for k in range(4,8):
    # for j in range(10,12):
    #   print 'j={} i={} binContent={}'.format(j,k,h.GetBinContent(j,i))
       

    h.Draw("colz")

    defaultLabels([label_nTot, label_HEM], x=0.20, y=0.91, spacing = 0.04, size = 0.06, dx = 0.10) 

    h.GetXaxis().SetTitle( "{} #phi".format(lepType[i]))
    h.GetYaxis().SetTitle( "{} #eta".format(lepType[i]))
    h.GetZaxis().SetRangeUser(0,zMaxes[i])
    #h_recoLep_all_phi_eta.GetZaxis().SetMaximum()

    c.SaveAs("{}.pdf".format(h.GetName()))
    c.SaveAs("{}.png".format(h.GetName()))
    c.SaveAs("{}.C".format(h.GetName()))

    #h_recoLep_all_phi_eta_tightId.Draw("colz")
    #c.SaveAs("h_recoLep_all_phi_eta_tightId.pdf")
  

