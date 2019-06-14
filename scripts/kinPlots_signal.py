
import ROOT
from ROOT import kBird
from array import *
import math
import os.path
from math import sqrt

legend_saver = []

def defaultLegend(x1, y1, x2, y2):
  legend = ROOT.TLegend(x1, y1, x2, y2)
  legend.SetFillColor(0)
  legend.SetFillStyle(0)
  legend.SetLineColor(0)
  legend.SetBorderSize(0)
  legend.SetTextFont(42)
  legend.SetTextSize(0.025*1.1)

  global legend_saver
  legend_saver.append(legend)
  return legend

def getOverflowedHisto(h):
  htemp = h.Clone()
  nbins = htemp.GetNbinsX()
  last_plus_overflow = htemp.GetBinContent(nbins) + htemp.GetBinContent(nbins+1)
  last_plus_overflow_error = math.sqrt( htemp.GetBinError(nbins)*htemp.GetBinError(nbins)  + htemp.GetBinError(nbins+1)*htemp.GetBinError(nbins+1))
  htemp.SetBinContent(nbins,last_plus_overflow )
  htemp.SetBinError(nbins,last_plus_overflow_error)
  return htemp


class PlotOpt(object):
  def __init__(self, what, binning, weight, xtitle, ytitle, logX, logY):
    self.what = what
    self.binning = binning
    self.weight = weight
    self.xtitle = xtitle
    self.ytitle = ytitle
    self.logX = logX
    self.logY = logY

class Sample(object):
  def __init__(self, model='T2qq', m=1, M=500, datafNames=[] ):   # it gets called with strings

    self.m = int(m)
    self.M = int(M)
    self.model = model

    self.selection = '(GenSusyMScan1=={M} && GenSusyMScan2=={m})'.format(m=m, M=M)
    #self.selection = '(1)'
    print self.selection

    #self.gstring = '0p25' if self.g==0.25 else '1'
    self.name = '{model}_{M}_{m}'.format(model=self.model,m=self.m, M=self.M,)
    self.legName = '{model}: M={M}, m={m}'.format(model=self.model,m=self.m, M=self.M )
    self.datafNames = datafNames 

    print '*******************************************************'
    print 'Processing ', self.legName
    print '*******************************************************'

    self.histoDefs = {
      # for fixed dm mass plots (shape)
      'met'         : PlotOpt('met_pt', [40,0,2000], self.selection, 'E_{T}^{miss} [GeV]', 'Fraction of events', False, True),
      'mt2'         : PlotOpt('mt2',    [40,0,2000], self.selection, 'MT2 [GeV]',          'Fraction of events', False, True),
      'nBJet20'     : PlotOpt('nBJet20',[15,0,15],   self.selection, 'Number of b-jets',   'Fraction of events', False, True),
      'nJet30'      : PlotOpt('nJet30', [15,0,15],   self.selection, 'Number of jets',     'Fraction of events', False, True),
      'ht'          : PlotOpt('ht',     [40,0,3000], self.selection, 'H_{T} [GeV]',        'Fraction of events', False, True),
    }

    self.histos = {}
    self.datafNames = datafNames


  def fillHistos(self):

    chain = ROOT.TChain('mt2')
    for inputFile in self.datafNames:
      if not os.path.isfile(inputFile): raise RuntimeError('no file with name {}'.format(inputFile))
      chain.Add(inputFile)
      
    for name,spec in self.histoDefs.items():

      histoName='{}_{}'.format(spec.what, self.model)
      h = ROOT.TH1F(histoName, histoName, spec.binning[0], spec.binning[1], spec.binning[2])
      chain.Project(h.GetName(),  '%s' % spec.what, spec.weight, 'goff') # goff = no graphics generated

      #print 'debug 0 ', h.Integral() 
      h.SetDirectory(0)

      h.GetXaxis().SetTitle(spec.xtitle)
      h.GetXaxis().SetNdivisions(505)
      h.GetYaxis().SetTitle(spec.ytitle)
      h.GetYaxis().SetNdivisions(505)
      h.GetYaxis().SetTitleSize(0.05)
      h.GetXaxis().SetTitleSize(0.05)

      newh = getOverflowedHisto(h)
      newh.SetDirectory(0)

      self.histos[name] = newh

    return True

  def saveHistos(self, norm):
    # save all histograms for this sample in the same canvas
    wasBatch = ROOT.gROOT.IsBatch()
    ROOT.gROOT.SetBatch(True)
    c = ROOT.TCanvas(self.name, self.name, 800, 600)
    c.DivideSquare(len(self.histos.keys()))

    for i, what in enumerate(self.histos.keys()):
      h = self.histos[what]
      pad = c.cd(i + 1)
      pad.SetLogx(self.histoDefs[what].logX)
      pad.SetLogy(self.histoDefs[what].logY)
      if norm and h.Integral() != 0:
        hdrawn = h.DrawNormalized('LPE')
      else:
        hdrawn = h.Draw('PLE')

      norm_suffix='_norm' if norm else ''

    c.SaveAs('./kinPlots/' + c.GetName() + norm_suffix + '.png')
    c.SaveAs('./kinPlots/' + c.GetName() + norm_suffix + '.pdf')
    c.SaveAs('./kinPlots/' + c.GetName() + norm_suffix + '.C')
    ROOT.gROOT.SetBatch(wasBatch)



# make a sample list to ease plotting
class SampleList(object):
  def __init__(self, name, samples):
    self.samples = samples  #set(samples)
    self.name = self.samples[0].model + '_' + name
    self.colors = [   ROOT.kOrange+1, ROOT.kRed, ROOT.kMagenta+2, ROOT.kViolet+8, ROOT.kAzure-8, ROOT.kAzure+6 ,
                      ROOT.kGreen+1, ROOT.kSpring+4, ROOT.kYellow -5, ROOT.kYellow -3, ROOT.kYellow, ROOT.kOrange
                  ]
    self.styles = [  20,20,20,24,24,24 ]
    if len(self.samples) > len(self.colors):
      raise RuntimeError('need more colors (%d vs %d)' % (len(self.colors), len(self.samples)))
    self.checkConsistency()
  def add(self, sample):
    self.samples.insert(sample)
    if len(self.samples) > len(self.colors):
      raise RuntimeError('need more colors (%d vs %d)' % (len(self.colors), len(self.samples)))
    self.checkConsistency()
  def remove(self, sample):
    self.samples.remove(sample)
  def checkConsistency(self):
    # makes sure samples have same binning and histograms
    for sample1 in self.samples:
      for sample2 in self.samples:
        if len(sample1.histoDefs) != len(sample2.histoDefs):
          raise RuntimeError('number of PlotOpts differs for %s and %s' % (sample1.name, sample2.name))
        if len(sample1.histos) != len(sample2.histos):
          raise RuntimeError('number of histos differs for %s and %s' % (sample1.name, sample2.name))
  def saveAll(self, norm=False, sameCanvas=False):
    # superimpose histograms for all samples, on the same canvas or on separate canvases
    wasBatch = ROOT.gROOT.IsBatch()
    ROOT.gROOT.SetBatch(True)
    if sameCanvas:
      c = ROOT.TCanvas(self.name, self.name, 6400, 4800)
      tosave = { c.GetName() : c }
    else:
      c = None
      tosave = {}

    legends = []
    graph_saver = []

    hMax = {}
    for j, what in enumerate(self.samples[0].histos.keys()):
      hMax[what] = []
      leg = defaultLegend(0.60,0.70,0.90,0.95)
      leg.SetTextSize(0.025*1.4)
      legends.append(leg)
    # do the actual plotting
    for i, sample in enumerate(self.samples):
      #print 'debug progress saveAll', i, sample.name
      if i == 0:
        opt = 'LPE'
        if sameCanvas: c.DivideSquare(len(sample.histos.keys()))
      else: opt = 'LPEsame'
      for j, what in enumerate(sample.histos.keys()):
        #print 'debug progress saveAll, ' ,j, what
        h = sample.histos[what]
        graph_saver.append(h)
        histo_name = sample.name

        if sameCanvas: 
          pad = c.cd(j + 1)
          graph_saver.append(pad)
        else:
          cname = '%s_%s' % (self.name, what)
          if cname not in tosave.keys():
            tosave[cname] = ROOT.TCanvas(cname, cname, 800, 600)
          pad = tosave[cname].cd()


        if i == 0:
          pad.SetLogx(sample.histoDefs[what].logX)
          pad.SetLogy(sample.histoDefs[what].logY)
        if norm and h.Integral():
          hdrawn = h.DrawNormalized(opt)
          hMax[what].append(hdrawn)
          hdrawn.SetLineColor(self.colors[i])
          hdrawn.SetMarkerColor(self.colors[i])
          hdrawn.SetMarkerStyle(self.styles[i])
          hdrawn.SetMarkerSize(3)
          hdrawn.SetLineWidth(2)
          legends[j].AddEntry(hdrawn,histo_name, 'LP')


        else:
          h.Draw(opt)
          hMax[what].append(h)
          h.SetLineColor(self.colors[i])
          h.SetMarkerColor(self.colors[i])
          h.SetMarkerStyle(self.styles[i])
          h.SetMarkerSize(4)
          h.SetLineWidth(2)
          legends[j].AddEntry(h, histo_name, 'LP')


        if i == len(self.samples) - 1:
          legends[j].Draw('same')
        #  leg = pad.BuildLegend(0.50,0.75,0.95,0.94)
          #leg.Draw()


    # compute the max of norm histo and set the range accordingly
    for j, what in enumerate(self.samples[0].histos.keys()):
      #print 'debug progress saveAll - second loop, ' ,j, what
      if len(hMax[what])>0:
        max = 0
        for ih in hMax[what]:
          if(max < ih.GetMaximum()):
            max=ih.GetMaximum()
        if (sample.histoDefs[what].logY==True):
          #hMax[name][0].GetYaxis().SetRangeUser(0.01, max * 7)
          hMax[what][0].SetMaximum(max*7)
          #print hMax[what][0].GetMaximum()
        else:
          #hMax[name][0].GetYaxis().SetRangeUser(0.01, max * 1.5)
          hMax[what][0].SetMaximum(max * 1.5)

    norm_suffix='_norm' if norm else ''

    #print 'debug tosave items', tosave.items()
    for cname, canv in tosave.items():
      #print 'debug progress saveAll save canva', cname
      canv.SaveAs('./kinPlots/' + cname + norm_suffix +'.png')
      canv.SaveAs('./kinPlots/' + cname + norm_suffix +'.pdf')
      canv.SaveAs('./kinPlots/' + cname + norm_suffix +'.C')
      #print 'debug after save canva'

    del graph_saver

    #print 'before setting batch'
    ROOT.gROOT.SetBatch(wasBatch)
       

    #print 'debug progress saveAll ----> end here'



def makeMassesPlot(path,files,points,model,namePlot):

  samplesToPlot=[]
  for M,m in points:

    sample=Sample(model=model, m=m, M=M, datafNames=map(lambda x : path+x, files)) 
    sample.fillHistos()
    sample.saveHistos(norm=True)

    samplesToPlot.append(sample)

  listToPlot=SampleList(name=namePlot, samples=samplesToPlot)
  listToPlot.saveAll(norm=True,sameCanvas=True)


if __name__ == "__main__":
    ROOT.gROOT.SetBatch(True)
    ROOT.TH1.SetDefaultSumw2()
    ROOT.gROOT.ProcessLine('.L /work/mratti/CMS_style/tdrstyleGraph.C')
    ROOT.gROOT.ProcessLine('setTDRStyle()')

    fileNames = {
      'T2bb': ['T2bb_1.root', 'T2bb.root'],
      'T1bbbb' : ['T1bbbb_94x_1.root', 'T1bbbb_94x_2.root'], 
      'T2qq' : ['T2qq.root', 'T2qq_1.root'], 
      'T1qqqq' : ['T1qqqq_94x_2.root', 'T1qqqq_94x_1.root'], 
    }
    
    pathName = '/scratch/mmasciov/NEWSnTtrees/2016/signal/'

    fixed_m_points = [
          [400, 1], 
          [700, 1], 
          [1000,1], 
          [1300,1], 
    ]

    fixed_M_points = [
          [800, 1], 
          [800, 400], 
          [800, 700], 
          [800, 750], 
          [800, 775], 
    ]

    for model in fileNames.keys():
      makeMassesPlot(path=pathName,files=fileNames[model],points=fixed_m_points,model=model,namePlot='fixed_m' )
      makeMassesPlot(path=pathName,files=fileNames[model],points=fixed_M_points,model=model,namePlot='fixed_M' )


      



