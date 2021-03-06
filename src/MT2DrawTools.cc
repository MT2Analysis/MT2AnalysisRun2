#include "../interface/MT2DrawTools.h"

#include "RooHistError.h"
#include "TLegend.h"
#include "THStack.h"
#include "TMinuit.h"

#include "../interface/MT2EstimateTree.h"

using namespace std;

MT2DrawTools::MT2DrawTools( const std::string& outDir, float lumi1, float lumi2, float lumi3 ) {

  lumi1_    = lumi1;
  lumi2_    = lumi2;
  lumi3_    = lumi3;
  lumiErr_ = 0.12;
  shapeNorm_ = false;
  outdir_ = outDir;

  data1_ = 0;
  data2_ = 0;
  data3_ = 0;
  
  mc1_ = 0;
  mc2_ = 0;
  mc3_ = 0;

  mcSF_ = 1.;

  addOverflow_ = true;

  displaySF_ = true;

  doPaperPlots_ = false;

  std::cout << "[MT2DrawTools] Initiating: " << std::endl;
  std::cout << "     lumiTot: " << lumi1_ + lumi2_ + lumi3_ << std::endl;
  std::cout << "     lumiErr: " << lumiErr_ << std::endl;
  std::cout << "     shapeNorm: " << shapeNorm_ << std::endl;
  std::cout << "     mcSF: " << mcSF_ << std::endl;
  std::cout << "     outDir: " << outdir_ << std::endl;
  std::cout << "     doPaperPlots: " << doPaperPlots_ << std::endl;

}


void MT2DrawTools::set_data( MT2Analysis<MT2EstimateTree>* data1, MT2Analysis<MT2EstimateTree>* data2, MT2Analysis<MT2EstimateTree>* data3 ) {

  data1_ = data1;
  data2_ = data2;
  data3_ = data3;

}


void MT2DrawTools::set_mc(std::vector< MT2Analysis<MT2EstimateTree>* >* mc1, std::vector< MT2Analysis<MT2EstimateTree>* >* mc2, std::vector< MT2Analysis<MT2EstimateTree>* >* mc3) {

  mc1_ = mc1;
  mc2_ = mc2;
  mc3_ = mc3;

}


void MT2DrawTools::set_outDir( const std::string& outdir ) {

  std::cout << "[MT2DrawTools] Setting outdir to: " << outdir << std::endl;
  system( Form("mkdir -p %s", outdir.c_str()) );
  outdir_ = outdir;

}


void MT2DrawTools::set_lumi(float lumi1, float lumi2, float lumi3) { 

  std::cout << "[MT2DrawTools] Setting total lumi to: " << lumi1 + lumi2 + lumi3<< std::endl;
  lumi1_ = lumi1; 
  lumi2_ = lumi2;
  lumi3_ = lumi3;

}


void MT2DrawTools::set_lumiErr( float lumiErr ) { 

  std::cout << "[MT2DrawTools] Setting lumi error to: " << lumiErr << std::endl;
  lumiErr_ = lumiErr; 

}


void MT2DrawTools::set_shapeNorm( bool shapeNorm ) { 

  if( shapeNorm )
    std::cout << "[MT2DrawTools] Using shape normalization." << std::endl;
  else
    std::cout << "[MT2DrawTools] Using lumi normalization." << std::endl;
  shapeNorm_ = shapeNorm;

}


void MT2DrawTools::set_year(int year) { 

  std::cout << "[MT2DrawTools] Setting year to: " << year<< std::endl;
  year_ = year; 

}


void MT2DrawTools::set_addOverflow( bool addOver ) { 

  if( addOver )
    std::cout << "[MT2DrawTools] Adding overflow bins." << std::endl;
  else
    std::cout << "[MT2DrawTools] Disabled adding overflow bins." << std::endl;
  addOverflow_ = addOver;

}



void MT2DrawTools::set_displaySF( bool displaySF ) { 

  if( displaySF )
    std::cout << "[MT2DrawTools] Setting display SF: ON" << std::endl;
  else
    std::cout << "[MT2DrawTools] Setting display SF: OFF" << std::endl;
  displaySF_ = displaySF;

}



void MT2DrawTools::set_mcSF( float mcsf ) {

  std::cout << "[MT2DrawTools] Setting MC SF to: " << mcsf << std::endl;
  mcSF_ = mcsf;

}


TStyle* MT2DrawTools::setStyle() {

  // set the TStyle
  TStyle* style = new TStyle("DrawBaseStyle", "");
  style->SetCanvasColor(0);
  style->SetPadColor(0);
  style->SetFrameFillColor(0);
  style->SetStatColor(0);
  style->SetOptStat(0);
  style->SetTitleFillColor(0);
  style->SetCanvasBorderMode(0);
  style->SetPadBorderMode(0);
  style->SetFrameBorderMode(0);
  style->SetPadBottomMargin(0.12);
  style->SetPadLeftMargin(0.12);
  style->cd();
  // For the canvas:
  style->SetCanvasBorderMode(0);
  style->SetCanvasColor(kWhite);
  style->SetCanvasDefH(600); //Height of canvas
  style->SetCanvasDefW(600); //Width of canvas
  style->SetCanvasDefX(0); //POsition on screen
  style->SetCanvasDefY(0);
  // For the Pad:
  style->SetPadBorderMode(0);
  style->SetPadColor(kWhite);
  style->SetPadGridX(false);
  style->SetPadGridY(false);
  style->SetGridColor(0);
  style->SetGridStyle(3);
  style->SetGridWidth(1);
  // For the frame:
  style->SetFrameBorderMode(0);
  style->SetFrameBorderSize(1);
  style->SetFrameFillColor(0);
  style->SetFrameFillStyle(0);
  style->SetFrameLineColor(1);
  style->SetFrameLineStyle(1);
  style->SetFrameLineWidth(1);
  // Margins:
  style->SetPadTopMargin(0.05);
  style->SetPadBottomMargin(0.15);//0.13);
  style->SetPadLeftMargin(0.15);//0.16);
  style->SetPadRightMargin(0.05);//0.02);
  // For the Global title:
  style->SetOptTitle(0);
  style->SetTitleFont(42);
  style->SetTitleColor(1);
  style->SetTitleTextColor(1);
  style->SetTitleFillColor(10);
  style->SetTitleFontSize(0.05);
  // For the axis titles:
  style->SetTitleColor(1, "XYZ");
  style->SetTitleFont(42, "XYZ");
  style->SetTitleSize(0.05, "XYZ");
  style->SetTitleXOffset(1.15);//0.9);
  style->SetTitleYOffset(1.5); // => 1.15 if exponents
  // For the axis labels:
  style->SetLabelColor(1, "XYZ");
  style->SetLabelFont(42, "XYZ");
  style->SetLabelOffset(0.007, "XYZ");
  style->SetLabelSize(0.045, "XYZ");
  // For the axis:
  style->SetAxisColor(1, "XYZ");
  style->SetStripDecimals(kTRUE);
  style->SetTickLength(0.03, "XYZ");
  style->SetNdivisions(510, "XYZ");
  //  style->SetPadTickX(1); // To get tick marks on the opposite side of the frame
  style->SetPadTickY(1);
  // for histograms:
  style->SetHistLineColor(1);
  // for the pallete
  Double_t stops[5] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red  [5] = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[5] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue [5] = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(5, stops, red, green, blue, 100);
  style->SetNumberContours(100);

  style->cd();
  return style;

}


std::string MT2DrawTools::getLumiText( float lumi ) {

  std::string returnText;
  if( lumi>=1.0 )
    returnText = (std::string)Form(" %.1f fb^{-1}", lumi);
  else if( lumi>0.01 )
    returnText = (std::string)Form(" %.0f pb^{-1}", 1000.*lumi);
  else 
    returnText = (std::string)Form(" %.1f pb^{-1}", 1000.*lumi);

  return returnText;

}


TPaveText* MT2DrawTools::getLabelTop( float lumi ) {

  char text[300];
  sprintf( text, "%s (13 TeV)", getLumiText(lumi).c_str() );
  //  sprintf( text, "CMS Preliminary, %s at #sqrt{s} = 13 TeV", getLumiText(lumi).c_str() );
  std::string text_str(text);
  return getLabelTop(text_str);

}

TPaveText* MT2DrawTools::getLabelTop( float lumi2015, float lumi2016 ) {

  char text[300];
  sprintf( text, "%s + %s (13 TeV)", getLumiText(lumi2015).c_str(), getLumiText(lumi2016).c_str() );
  //  sprintf( text, "CMS Preliminary, %s at #sqrt{s} = 13 TeV", getLumiText(lumi).c_str() );
  std::string text_str(text);
  return getLabelTop(text_str);

}


TPaveText* MT2DrawTools::getLabelTopSimulation( float lumi ) {

  char text[300];
  sprintf( text, "CMS Simulation, %.1f fb^{-1} at #sqrt{s} = 13 TeV", lumi );
  std::string text_str(text);
  return getLabelTopSimulation(text_str);

}

TPaveText* MT2DrawTools::getLabelTop( const std::string& text ) {

  TPaveText* label_top = new TPaveText(0.4,0.959,0.975,0.963, "brNDC");
  //  TPaveText* label_top = new TPaveText(0.4,0.953,0.975,0.975, "brNDC");
  label_top->SetBorderSize(0);
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right
  label_top->SetTextFont(42);  // label_top->SetTextFont(62);
  label_top->AddText(text.c_str());

  return label_top;

}


TPaveText* MT2DrawTools::getLabelTopSimulation( const std::string& text ) {

  TPaveText* label_top = new TPaveText(0.4,0.953,0.975,0.975, "brNDC");
  label_top->SetBorderSize(0);
  label_top->SetFillColor(kWhite);
  label_top->SetTextSize(0.038);
  label_top->SetTextAlign(31); // align right                                                                                                                                        
  label_top->SetTextFont(62);
  label_top->AddText(text.c_str());

  return label_top;

}


TPaveText* MT2DrawTools::getLabelCMS( const std::string& text ) {

  TPaveText* label_cms = new TPaveText(0.143,0.96,0.27,0.965, "brNDC");
  label_cms->SetBorderSize(0);
  label_cms->SetFillColor(kWhite);
  label_cms->SetTextSize(0.042);
  label_cms->SetTextAlign(11); // align left
  label_cms->SetTextFont(61);
  label_cms->AddText( text.c_str() );

  return label_cms;

}



void MT2DrawTools::addLabels( TCanvas* c1, float lumi, const std::string& text  ) {

  c1->cd();
  TPaveText* labelTop = MT2DrawTools::getLabelTop( lumi );
  labelTop->Draw("same");
  TPaveText* labelCMS = MT2DrawTools::getLabelCMS( text.c_str() );
  labelCMS->Draw("same");

}



TGraphAsymmErrors* MT2DrawTools::getPoissonGraph( TH1D* histo, bool drawZeros, const std::string& xerrType, float nSigma ) {

  //  histo->SetBinErrorOption(TH1::kPoisson);
  const double alpha = 1 - 0.6827;

  unsigned int nBins = histo->GetNbinsX();
  int emptyBins=0;
  for( unsigned i=1; i < nBins; ++i ) {
    if( histo->GetBinContent(i)==0 ) emptyBins += 1;
  }
  if( (float)emptyBins/(float)nBins > 0.4 ) drawZeros=false;

  TGraphAsymmErrors* graph = new TGraphAsymmErrors(0);

  for( int iBin=1; iBin<(histo->GetXaxis()->GetNbins()+1); ++iBin ) {

    int y; // these are data histograms, so y has to be integer
    double x, xerr, yerrplus, yerrminus;
    x = histo->GetBinCenter(iBin);
    if( xerrType=="0" )
      xerr = 0.;
    else if( xerrType=="binWidth" )
      xerr = histo->GetBinWidth(iBin)/2.;
    else if( xerrType=="sqrt12" )
      xerr = histo->GetBinWidth(iBin)/sqrt(12.);
    else {
      std::cout << "[MT2DrawTools::getPoissonGraph] Unkown xerrType '" << xerrType << "'. Setting to bin width." << std::endl;
      xerr = histo->GetBinWidth(iBin);
    }

    y = (int)histo->GetBinContent(iBin);

    if( y==0 && !drawZeros ) continue;     
    
    double ym =  (y==0) ? 0  : (ROOT::Math::gamma_quantile(alpha/2,y,1.));
    double yp =  ROOT::Math::gamma_quantile_c(alpha/2,y+1,1) ;
    
    //    yerrminus = histo->GetBinErrorLow(iBin);
    //    yerrplus = histo->GetBinErrorUp(iBin);
    
    //double ym, yp;
    //RooHistError::instance().getPoissonInterval(y,ym,yp,nSigma);
    
    yerrplus = yp - y;
    yerrminus = y - ym;

    if(y==0)
      std::cout << yerrplus << "\t" << yerrminus << std::endl;

    int thisPoint = graph->GetN();
    graph->SetPoint( thisPoint, x, y );
    graph->SetPointError( thisPoint, xerr, xerr, yerrminus, yerrplus );

  }

  return graph;

}



TGraphAsymmErrors* MT2DrawTools::getRatioGraph( TH1D* histo_data, TH1D* histo_mc, const std::string& xerrType){

  if( !histo_data || !histo_mc ) return 0;

  TGraphAsymmErrors* graph  = new TGraphAsymmErrors();
  
  //  TGraphAsymmErrors* graph_data = MT2DrawTools::getPoissonGraph(histo_data, false);
  TGraphAsymmErrors* graph_data = MT2DrawTools::getPoissonGraph(histo_data, true);

  //for debugging
  //TGraphAsymmErrors* graph_tmp = MT2DrawTools::getPoissonGraph(histo_mc, true);
  //cout << "Ndata: " << graph_data->GetN() << endl;
  //cout << "Nmc: " << graph_tmp->GetN() << endl;

  
  for( int i=0; i < graph_data->GetN(); ++i){
    
    Double_t x_tmp, data;
    graph_data->GetPoint( i, x_tmp, data );
      
    Double_t data_errUp = graph_data->GetErrorYhigh(i);
    Double_t data_errDn = graph_data->GetErrorYlow(i);
    
    int iBin = histo_mc->FindBin(x_tmp);
    float mc = histo_mc->GetBinContent(iBin);
    float mc_err = histo_mc->GetBinError(iBin);

    
    float ratio = data/mc;
    float ratio_errUp = sqrt( data_errUp*data_errUp/(mc*mc) + mc_err*mc_err*data*data/(mc*mc*mc*mc) );
    float ratio_errDn = sqrt( data_errDn*data_errDn/(mc*mc) + mc_err*mc_err*data*data/(mc*mc*mc*mc) );

   
    double xerr;
    
    if( xerrType=="0" )
      xerr = 0.;
    else if( xerrType=="binWidth" )
      xerr = histo_mc->GetBinWidth(iBin)/2.;
    else if( xerrType=="sqrt12" )
      xerr = histo_mc->GetBinWidth(iBin)/sqrt(12.);
    else {
      std::cout << "[MT2DrawTools::getPoissonGraph] Unkown xerrType '" << xerrType << "'. Setting to bin width." << std::endl;
      xerr = histo_mc->GetBinWidth(iBin);
    }

    graph->SetPoint(i, x_tmp, ratio );
    graph->SetPointEYhigh(i, ratio_errUp );
    graph->SetPointEYlow(i, ratio_errDn );
    graph->SetPointEXhigh(i, xerr );
    graph->SetPointEXlow(i, xerr );

  }

  graph->SetLineColor(1);
  graph->SetMarkerColor(1);
  graph->SetMarkerStyle(20);

  return graph;

}


TGraphAsymmErrors* MT2DrawTools::getPullPlot( TH1D* data_hist, TH1D* estimate_hist){

  if( !data_hist || !estimate_hist ) return 0;

  TGraphAsymmErrors* graph  = new TGraphAsymmErrors();
  
  TGraphAsymmErrors* graph_data = MT2DrawTools::getPoissonGraph(data_hist, true);

  for( int i=0; i < graph_data->GetN(); ++i){
    
    Double_t x_tmp, data;
    graph_data->GetPoint( i, x_tmp, data );

    Double_t data_errUp = graph_data->GetErrorYhigh(i);
    Double_t data_errDn = graph_data->GetErrorYlow(i);
     
    int iBin = estimate_hist->FindBin(x_tmp);

    float mc = estimate_hist->GetBinContent(iBin);
    float mc_err = estimate_hist->GetBinError(iBin);

    //float data_err = data_hist->GetBinError(iBin);
    float data_err = graph_data->GetErrorY(i);

    float sigma = sqrt(data_err*data_err + mc_err*mc_err);
    
    float residual =  (data-mc)/sigma;
    
    float errUp = (data_errUp + mc_err)/sigma;
    float errDn = (data_errDn + mc_err)/sigma; 

    graph->SetPoint(i, x_tmp, residual );
    graph->SetPointEYhigh(i, errUp );
    graph->SetPointEYlow(i, errDn );

    //  if(i>=0&&i<13){
    //  cout << "data: " << data << endl;
    //  cout << "data_err: " << data_err << endl;
    //  cout << "estimate: " << mc << endl;
    //  cout << "mc_err: " << mc_err << endl;
    //  cout << "sigma " << sigma << endl;
    //  cout << "residual: " << residual << endl;
    //  cout << "monojet " << i << " " << residual << endl;
    //  cout << endl;
    // }
   
  }

  graph->SetLineColor(1);
  graph->SetMarkerColor(1);
  graph->SetMarkerStyle(20);
 
  return graph;

}



// TGraphAsymmErrors* MT2DrawTools::getRatioGraph( TH1D* histo_data, TH1D* histo_mc ){

//   if( !histo_data || !histo_mc ) return 0;

//   TGraphAsymmErrors* graph  = new TGraphAsymmErrors();
  
//   TGraphAsymmErrors* graph_data = MT2DrawTools::getPoissonGraph(histo_data, false, "binWidth");
  
//   for( int i=0; i < graph_data->GetN(); ++i){
    
//     Double_t x_tmp, data;
//     graph_data->GetPoint( i, x_tmp, data );

//     Double_t data_errUp = graph_data->GetErrorYhigh(i);
//     Double_t data_errDn = graph_data->GetErrorYlow(i);
    
//     int iBin = histo_mc->FindBin(x_tmp);
//     float mc = histo_mc->GetBinContent(iBin);
//     float mc_err = histo_mc->GetBinError(iBin);


//     float ratio = data/mc;
//     float ratio_errUp = sqrt( data_errUp*data_errUp/(mc*mc) + mc_err*mc_err*data*data/(mc*mc*mc*mc) );
//     float ratio_errDn = sqrt( data_errDn*data_errDn/(mc*mc) + mc_err*mc_err*data*data/(mc*mc*mc*mc) );

//     graph->SetPoint(i, x_tmp, ratio );
//     graph->SetPointEYhigh(i, ratio_errUp );
//     graph->SetPointEYlow(i, ratio_errDn );

//     float  xerr = histo_data->GetBinWidth(iBin)/2.;
//     graph->SetPointEXhigh(i, xerr);    
//     graph->SetPointEXlow(i, xerr);    

//   }

//   graph->SetLineColor(1);
//   graph->SetMarkerColor(1);
//   graph->SetMarkerStyle(20);

//   return graph;

// }

TH1D* MT2DrawTools::getBandAtOne( TH1D* h ){

  TH1D* h_band = (TH1D*)h->Clone( Form("%s_band", h->GetName()) );
  h_band->SetMarkerSize(0);
  h_band->SetFillColor ( h->GetLineColor()-4 );
  h_band->SetFillStyle (3001);
  for ( int iBin=1; iBin <= h->GetNbinsX(); iBin++){
    h_band->SetBinContent(iBin,1);
    double error = h->GetBinContent(iBin) ? h->GetBinError(iBin)/h->GetBinContent(iBin) : 0.0;
    h_band->SetBinError(iBin, error);
  }
  
  return h_band;

}


TPad* MT2DrawTools::getCanvasMainPad( bool logY ){
  
  std::string padApp = "";
  if( logY )
    padApp = "_log";
  TPad* pad1 = new TPad(Form("pad1%s", padApp.c_str()), Form("pad1%s", padApp.c_str()), 0, 0.3-0.1, 1, 1);
  pad1->SetBottomMargin(0.15);
  if( logY )
    pad1->SetLogy();

  return pad1;

}

TPad* MT2DrawTools::getCanvasRatioPad( bool logY ){

  std::string padApp = "";
  if( logY )
    padApp = "_log";
  TPad* pad2 = new TPad(Form("pad2%s", padApp.c_str()), Form("pad2%s", padApp.c_str()), 0, 0, 1, 0.21);
  pad2->SetTopMargin(0.05);
  pad2->SetBottomMargin(0.1);

  return pad2;

}


TH2D* MT2DrawTools::getRatioAxes( float xMin, float xMax, float yMin, float yMax ){

  TH2D* h2_axes_ratio = new TH2D("axes_ratio", "", 10, xMin, xMax, 10, yMin, yMax );
  h2_axes_ratio->SetStats(0);
  h2_axes_ratio->GetXaxis()->SetLabelSize(0.00);
  h2_axes_ratio->GetXaxis()->SetTickLength(0.09);
  h2_axes_ratio->GetYaxis()->SetNdivisions(5,5,0);
  h2_axes_ratio->GetYaxis()->SetTitleSize(0.17);
  h2_axes_ratio->GetYaxis()->SetTitleOffset(0.4);
  h2_axes_ratio->GetYaxis()->SetLabelSize(0.17);
  h2_axes_ratio->GetYaxis()->SetTitle("Data / MC");

  return h2_axes_ratio;

}


double MT2DrawTools::getSFError(double integral_data, double error_data, double integral_mc, double error_mc){

  double error_datamc = integral_data/integral_mc*(sqrt( (error_data/integral_mc)*(error_data/integral_mc) + (integral_data*error_mc/(integral_data*integral_data))*(integral_data*error_mc/(integral_data*integral_data)) ));
  
  return error_datamc;

}

TPaveText* MT2DrawTools::getRatioText( double integral_data, double integral_mc, double error_datamc ){
 
TPaveText* ratioText = new TPaveText( 0.133, -0.051, 0.4, 0.1 , "brNDC" );
  ratioText->SetTextSize(0.031);
  ratioText->SetTextFont(62);
  ratioText->SetTextColor(2);
  ratioText->SetFillColor(0);
  ratioText->SetTextAlign(11);
  //ratioText->AddText( Form("Data/MC = %.2f +/- %.2f", integral_data/integral_mc, error_datamc) );
  ratioText->AddText( Form("MC scaled by %.2f", integral_data/integral_mc) );

  return ratioText;

}
  
TLine* MT2DrawTools::getSFLine(double integral_data, double integral_mc, float xMin, float xMax){

  double scaleFactor = integral_data/integral_mc;
  TLine* lineSF = new TLine(xMin, scaleFactor, xMax, scaleFactor);
  lineSF->SetLineColor(kRed);

  return lineSF;

}

TGraphErrors* MT2DrawTools::getSFBand(double integral_data, double error_data, double integral_mc, double error_mc, float xMin, float xMax){
  
  double error_datamc = MT2DrawTools::getSFError(integral_data, error_data, integral_mc, error_mc);

  double x[2]={(double)xMin, (double)xMax};
  double xerr[2]={0., 0.};
  double yerr[2]={error_datamc, error_datamc};
  double y[2]={integral_data/integral_mc, integral_data/integral_mc};

  TGraphErrors* SFband = new TGraphErrors(2, x, y, xerr, yerr);
  SFband->SetLineColor(0);
  SFband->SetFillColor(kRed);
  SFband->SetFillStyle(3244);
  
  return SFband;
  
}


TF1* MT2DrawTools::getSFFit(TGraphAsymmErrors* g_ratio, float xMin, float xMax){

  if( !g_ratio ) return 0;

  TF1* f=new TF1("f", "[0]", xMin, xMax);
  f->SetLineColor(kRed);
  f->SetParameter(0, 1.0);
  g_ratio->Fit(f, "0");
  
  return f;

}

void MT2DrawTools::getSFFitParameters(TF1* f, double &sf, double &sfErr, double &chi2, int &ndof){

  chi2  = f->GetChisquare();
  ndof     = f->GetNDF();
  sf    = f->GetParameter(0);
  sfErr = f->GetParError(0);

}

TGraphErrors* MT2DrawTools::getSFFitBand(TF1* f, float xMin, float xMax){
  
  double chi2, sf, sfErr;
  int ndof;
  MT2DrawTools::getSFFitParameters(f, sf, sfErr, chi2, ndof);

  double x[2]    ={(double)xMin, (double)xMax};
  double y[2]    ={sf, sf};

  double xerr[2] ={0., 0.};
  double yerr[2] ={sfErr, sfErr};

  TGraphErrors* SFband = new TGraphErrors(2, x, y, xerr, yerr);
  SFband->SetLineColor(0);
  SFband->SetFillColor(kRed);
  SFband->SetFillStyle(3244);
  
  return SFband;
  
}

TPaveText* MT2DrawTools::getFitText( TF1* f ){
 

  TPaveText* ratioText = new TPaveText( 0.135, -0.051, 0.4, 0.1 , "brNDC" );
  //ratioText->SetTextSize(0.025);
  ratioText->SetTextSize(0.031);
  ratioText->SetTextFont(62);
  ratioText->SetTextColor(2);
  ratioText->SetFillColor(0);
  ratioText->SetTextAlign(11);
  
  double chi2, sf, sfErr;
  int ndof;
  MT2DrawTools::getSFFitParameters(f, sf, sfErr, chi2, ndof);
  //ratioText->AddText( Form("Data/MC = %.2f #pm %.2f (#chi^{2}/ndof = %.2f / %d)", sf, sfErr, chi2, ndof) );
  ratioText->AddText( Form("Fitted Data/MC = %.2f #pm %.2f", sf, sfErr) );

  return ratioText;

}


TGraphErrors* MT2DrawTools::getSystBand(float xMin, float xMax, double SystErr){
  
  double x[2]={(double)xMin, (double)xMax};
  double xerr[2]={0., 0.};
  double yerr[2]={SystErr, SystErr};
  double y[2]={1.0, 1.0};

  TGraphErrors* SystBand = new TGraphErrors(2, x, y, xerr, yerr);
  SystBand->SetLineColor(0);
  SystBand->SetFillColor(kGray+2);
  SystBand->SetFillStyle(3244);
  
  return SystBand;
  
}


TH1D* MT2DrawTools::getMCBandHisto( TH1D* histo_mc, double SystErr ){

  TH1D* histoBand = (TH1D*) histo_mc->Clone("histo_band");
  for( int b=1; b <= histoBand->GetNbinsX(); ++b ){

    float thisStatErr = histoBand->GetBinError(b);
    float thisStats = histoBand->GetBinContent(b);
    float thisSystErr = thisStats*SystErr;
    float thisErr = sqrt(thisStatErr*thisStatErr+thisSystErr*thisSystErr);
    histoBand->SetBinError(b, thisErr);

  }

  histoBand->SetLineColor(0);
  histoBand->SetFillColor(kGray+2);
  histoBand->SetFillStyle(3244);

  return histoBand;

}


void MT2DrawTools::addOverflowSingleHisto( TH1D* yield ) {

  yield->SetBinContent(yield->GetNbinsX(),
		       yield->GetBinContent(yield->GetNbinsX()  )+
		       yield->GetBinContent(yield->GetNbinsX()+1)  );
  yield->SetBinError(  yield->GetNbinsX(),
		       sqrt(yield->GetBinError(yield->GetNbinsX() )*
			    yield->GetBinError(yield->GetNbinsX() )+
			    yield->GetBinError(yield->GetNbinsX()+1)*
			    yield->GetBinError(yield->GetNbinsX()+1)  ));

  yield->SetBinContent(yield->GetNbinsX()+1, 0.);
  yield->SetBinError  (yield->GetNbinsX()+1, 0.);

}



void MT2DrawTools::addOverflowSingleHisto( TH3D* yield3d ) {
  
  for (int y=1; y<=yield3d->GetNbinsY()+1; ++y)
    for (int z=1; z<=yield3d->GetNbinsZ()+1; ++z){

      yield3d->SetBinContent(yield3d->GetNbinsX(), y, z,
			   yield3d->GetBinContent(yield3d->GetNbinsX(), y, z  )+
			   yield3d->GetBinContent(yield3d->GetNbinsX()+1, y, z)  );
      yield3d->SetBinError(  yield3d->GetNbinsX(), y, z,
			   sqrt(yield3d->GetBinError(yield3d->GetNbinsX(), y, z  )*
				yield3d->GetBinError(yield3d->GetNbinsX(), y, z  )+
				yield3d->GetBinError(yield3d->GetNbinsX()+1, y, z)*
				yield3d->GetBinError(yield3d->GetNbinsX()+1, y, z)  ));
      
      yield3d->SetBinContent(yield3d->GetNbinsX()+1, y, z, 0.);
      yield3d->SetBinError  (yield3d->GetNbinsX()+1, y, z, 0.);
    }
  
}




TH1D* MT2DrawTools::getBand( TF1* f, const std::string& name ) {

 const int ndim_resp_q = f->GetNpar();
 TMatrixD emat_resp_q(ndim_resp_q, ndim_resp_q);
 gMinuit->mnemat(&emat_resp_q[0][0], ndim_resp_q);

 return getBand(f, emat_resp_q, name);

}



// Create uncertainty band (histogram) for a given function and error matrix
// in the range of the function.
TH1D* MT2DrawTools::getBand(TF1 *f, TMatrixD const& m, std::string name, bool getRelativeBand, int npx) {

 Bool_t islog = true;
 //double xmin = f->GetXmin()*0.9;
 //double xmax = f->GetXmax()*1.1; //fixes problem in drawing with c option
 double xmin = f->GetXmin();
 double xmax = f->GetXmax()*1.1; //fixes problem in drawing with c option
 int npar = f->GetNpar();
 //TString formula = f->GetExpFormula();

 // Create binning (linear or log)
 Double_t xvec[npx];
 xvec[0] = xmin;
 double dx = (islog ? pow(xmax/xmin, 1./npx) : (xmax-xmin)/npx);
 for (int i = 0; i != npx; ++i) {
   xvec[i+1] = (islog ? xvec[i]*dx : xvec[i]+dx);
 }


 //
 // Compute partial derivatives numerically
 // can be used with any fit function
 //
 Double_t sigmaf[npx];
 TH1D* h1_band = new TH1D(name.c_str(), "", npx, xvec);

 for( int ipx=0; ipx<npx; ++ipx ) {

   sigmaf[ipx] = 0.;
   Double_t partDeriv[npar];

   //compute partial derivatives of f wrt its parameters:
   for( int ipar=0; ipar<npar; ++ipar ) {

     Float_t pi = f->GetParameter(ipar);
     Float_t dpi = sqrt(m[ipar][ipar])*0.01; //small compared to the par sigma
     f->SetParameter(ipar, pi+dpi);
     Float_t fplus = f->Eval(xvec[ipx]); 
     f->SetParameter(ipar, pi-dpi);
     Float_t fminus = f->Eval(xvec[ipx]); 
     f->SetParameter(ipar, pi); //put it back as it was

     partDeriv[ipar] = (fplus-fminus)/(2.*dpi);

   } //for params

   //compute sigma(f) at x:
   for( int ipar=0; ipar<npar; ++ipar ) {
     for( int jpar=0; jpar<npar; ++jpar ) {
       sigmaf[ipx] += partDeriv[ipar]*partDeriv[jpar]*m[ipar][jpar];
     }
   }
   sigmaf[ipx] = sqrt(sigmaf[ipx]); //absolute band

   h1_band->SetBinContent( ipx, f->Eval(xvec[ipx]) );
   if( getRelativeBand )
     h1_band->SetBinError( ipx, sigmaf[ipx]/f->Eval(xvec[ipx]) );
   else
     h1_band->SetBinError( ipx, sigmaf[ipx] );

 } //for points

 h1_band->SetMarkerStyle(20);
 h1_band->SetMarkerSize(0);
 h1_band->SetFillColor(18);
 h1_band->SetFillStyle(3001);


 //TGraph* h1_statError = new TGraph(npx, xvec, sigmaf);
//TH2D* h2_axesStat = new TH2D("axesStat", "", 10, 20., 1400., 10, 0., 10.);
//h2_axesStat->GetXaxis()->SetNoExponent();
//h2_axesStat->GetXaxis()->SetMoreLogLabels();
//TCanvas* cStat = new TCanvas("cStat", "cStat", 600, 600);
//cStat->cd();
//cStat->SetLogx();
//h2_axesStat->Draw();
//h1_band->Draw("psame");
//std::string canvasName = "stat/" + name + ".eps";
//cStat->SaveAs(canvasName.c_str());

//delete h2_axesStat;
//delete cStat;

 return h1_band;

} //getband




std::vector<TCanvas*> MT2DrawTools::drawRegionYields_fromTree( const std::string& saveName, const std::string& varName, const std::string& selection, int nBins, float xMin, float xMax, std::string axisName, const std::string& units, const std::string& kinCuts, const std::string& topoCuts ) {
//void MT2DrawTools::drawRegionYields_fromTree( MT2Analysis<MT2EstimateTree>* data, std::vector<MT2Analysis<MT2EstimateTree>* >  bgYields, const std::string& saveName, const std::string& varName, const std::string& selection, int nBins, float xMin, float xMax, std::string axisName, const std::string& units, const std::string& kinCuts ) {
  std::cout << "[MT2DrawTools::drawRegionYields_fromTree] Starting with var=" << varName << ", selection=" << selection << std::endl;

  TString sel_tstr(selection);
  if( sel_tstr.Contains("weight") ) {
    std::cout << "[MT2DrawTools::drawRegionYields_fromTree] WARNING!! Selection contains 'weight'!! Are you sure you know what you're doing??" << std::endl;
  }

  std::vector<TCanvas*> returnVector;

  system( Form("mkdir -p %s", outdir_.c_str()) );

  
  float binWidth = (xMax-xMin)/nBins;
  if( axisName=="" ) axisName = varName;


  TH1::AddDirectory(kTRUE); // stupid ROOT memory allocation needs this

  std::set<MT2Region> MT2Regions = mc1_->at(0)->getRegions();
  
  for( std::set<MT2Region>::iterator iMT2 = MT2Regions.begin(); iMT2!=MT2Regions.end(); ++iMT2 ) {

    TCanvas *old_c1 = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(Form("c1_%s", iMT2->getName().c_str()));
    if( old_c1 != 0 ) delete old_c1;
  
    MT2Region thisRegion( (*iMT2) );
   
    TTree* tree_data1 = (data1_) ? data1_->get(thisRegion)->tree : 0;
    TTree* tree_data2 = (data2_) ? data2_->get(thisRegion)->tree : 0;
    TTree* tree_data3 = (data3_) ? data3_->get(thisRegion)->tree : 0;

    std::vector<TTree*> tree_data = {tree_data1, tree_data2, tree_data3};

   

    TH1D* h1_data1 = 0;
    TH1D* h1_data2 = 0;
    TH1D* h1_data3 = 0;

    std::vector<TH1D*> h1_data_sep = {h1_data1, h1_data2, h1_data3};
   
    for(unsigned int i(0); i<h1_data_sep.size(); ++i){ //we loop on the years 
      TString index = to_string(i+1);
      if( tree_data[i] ) {
	h1_data_sep[i] = new TH1D("h1_data_sep" + index, "", nBins, xMin, xMax );
	tree_data[i]->Project( "h1_data_sep" + index, varName.c_str(), selection.c_str() );
	//tree_data->Project( "h1_data", varName.c_str(), Form("%f*(%s)", data_->getWeight(), selection.c_str()) );
	if( addOverflow_ )
	  MT2DrawTools::addOverflowSingleHisto(h1_data_sep[i]);
      }
    }

  
    TH1D* h1_data = 0;
    //if there is only one year
    if(!tree_data[2] || !tree_data[3]){
      h1_data = (TH1D*) h1_data_sep[0]->Clone();
    }
    else{ //we combine the three years
      TH1D* h1_data_intermediate = (TH1D*) h1_data_sep[0]->Clone();
      h1_data_intermediate->Add(h1_data_sep[1]);
      h1_data = (TH1D*) h1_data_intermediate->Clone(); //this histogram will contain all the years
      h1_data->Add(h1_data_sep[2]);
    }

    TGraphAsymmErrors* gr_data = 0;
    gr_data = MT2DrawTools::getPoissonGraph(h1_data, false, "binWidth");
    gr_data->SetMarkerStyle(20);
    gr_data->SetMarkerSize(1.2);

    
    std::vector<TH1D*> histos_mc1;
    std::vector<TH1D*> histos_mc2;
    std::vector<TH1D*> histos_mc3;

    //std::vector<vector<TH1D*>> histos_mc = {histos_mc1, histos_mc2, histos_mc3};
    std::vector<TH1D*> histos_mc_sep;

    for( unsigned i=0; i<mc1_->size(); ++i ) {//loop on the different contributions of the background 
      TTree* tree_mc1 = (mc1_) ? (mc1_->at(i)->get(thisRegion)->tree) : 0;
      TTree* tree_mc2 = (mc2_) ? (mc2_->at(i)->get(thisRegion)->tree) : 0;
      TTree* tree_mc3 = (mc3_) ? (mc3_->at(i)->get(thisRegion)->tree) : 0;

      std::vector<TTree*> tree_mc = {tree_mc1, tree_mc2, tree_mc3};

      std::string thisName1 = (tree_mc1) ? "h1_1_" + mc1_->at(i)->getName() + "_" + thisRegion.getName() : 0;
      std::string thisName2 = (tree_mc2) ? "h1_2_" + mc2_->at(i)->getName() + "_" + thisRegion.getName() : ""; 
      std::string thisName3 = (tree_mc2) ? "h1_3_" + mc3_->at(i)->getName() + "_" + thisRegion.getName() : ""; 

     
      std::vector<std::string> thisName = {thisName1, thisName2, thisName3};
      std::vector<float> lumi = {lumi1_, lumi2_, lumi3_};

      TH1D* h1_mc1 = 0;
      TH1D* h1_mc2 = 0;
      TH1D* h1_mc3 = 0;

      std::vector<TH1D*> h1_mc = {h1_mc1, h1_mc2, h1_mc3};

      for(unsigned int j(0); j<tree_mc.size(); ++j){ //we loop on the years

	TObject* obj = gROOT->FindObject(thisName[j].c_str());
	if( obj ) delete obj;

	h1_mc[j] = new TH1D( thisName[j].c_str(), "", nBins, xMin, xMax );
	h1_mc[j]->Sumw2();

	if( selection!="" ){
	  if(tree_mc[j]){
	    tree_mc[j]->Project( thisName[j].c_str(), varName.c_str(), Form("%f*weight*(%s)", lumi[j], selection.c_str()) );
	    //tree_mc->Project( thisName.c_str(), varName.c_str(), Form("%f*(%s)", lumi_*mc_->at(i)->getWeight(), selection.c_str()) );
	  }
	}
	else{
	  if(tree_mc[j]){
	    tree_mc[j]->Project( thisName[j].c_str(), varName.c_str(), Form("%f*weight", lumi[j]) );
	    //tree_mc->Project( thisName.c_str(), varName.c_str(), Form("%f", lumi_*mc_->at(i)->getWeight()) );
	  }
	}
	if( addOverflow_ )
	  MT2DrawTools::addOverflowSingleHisto(h1_mc[j]);
	
	histos_mc_sep.push_back(h1_mc[j]);
      
	//if( varName == "nBJets"){
	// for(int iBin=1; iBin< h1_mc->GetNbinsX(); iBin++){
	//   std::cout << "iBin " << i << " = " << h1_mc->GetBinContent(iBin) << std::endl;
	//}}

      }
     
    }
  
   
    std::vector<TH1D*> histos_mc(2);

    if(!mc2_ && !mc3_){ //only one year
      histos_mc[0] = histos_mc_sep[0];
      histos_mc[1] = histos_mc_sep[3];
    }
    else{ //we combine the years per MC category
    
    TH1D* h1_int1 = (TH1D*) histos_mc_sep[0]->Clone();
    h1_int1->Add(histos_mc_sep[1]);
    TH1D* h1_int2 = (TH1D*) h1_int1->Clone();
    h1_int2->Add(histos_mc_sep[2]);

    histos_mc[0] = h1_int2;

    TH1D* h1_int3 = (TH1D*) histos_mc_sep[3]->Clone();
    h1_int3->Add(histos_mc_sep[4]);
    TH1D* h1_int4 = (TH1D*) h1_int3->Clone();
    h1_int4->Add(histos_mc_sep[5]);

    histos_mc[1] = h1_int4;  
    }

    //we combine MC categories
    TH1D* mc_sum;
 
    for( unsigned i=0; i<histos_mc.size(); ++i ) { 
      if( i==0 ) {
	mc_sum = new TH1D( *histos_mc[i] );
	mc_sum->SetName("mc_sum");
      } else {
	mc_sum->Add( histos_mc[i] );
      }
    }
         
    

    float scaleFactor = mcSF_;
    
    double integralData = -9999, integralMC = -9999, errorData = -9999, errorMC = -9999;

    if( data1_ ) {

      float sf =-9999;

      if( addOverflow_ ){
	//adding the overflow bin to the SF calculation
	integralData = h1_data->IntegralAndError(1, nBins+1, errorData);
        integralMC = mc_sum->IntegralAndError(1, nBins+1, errorMC);

      }else{ 
	//not adding the overflow bin
	integralData = h1_data->IntegralAndError(1, nBins, errorData);
        integralMC = mc_sum->IntegralAndError(1, nBins, errorMC);
      }

      std::cout << "Integrals: data=" << integralData << "\t mc=" << integralMC << std::endl;
      sf  = integralData/integralMC;

      std::cout << "SF: " << sf << std::endl;
      if( shapeNorm_ ) scaleFactor *= sf;
    }

    

    TObject* oldStack = gROOT->FindObject("bgStack");
    if( oldStack ) delete oldStack;
    
    TH1D* histo_mc;
    THStack* bgStack = new THStack("bgStack", "");
    
    
    for( unsigned i=0; i<histos_mc.size(); ++i ) { 
      int index = mc1_->size() - i - 1;
      histos_mc[index]->SetFillColor( mc1_->at(index)->getColor() );
      histos_mc[index]->SetLineColor( kBlack );
      //if( shapeNorm_ && data_ )
      histos_mc[index]->Scale( scaleFactor );

      if(i==0) histo_mc = (TH1D*) histos_mc[index]->Clone("histo_mc");
      else histo_mc->Add(histos_mc[index]);
      bgStack->Add(histos_mc[index]);
    }

    TH1D* mcBand = MT2DrawTools::getMCBandHisto( histo_mc, lumiErr_ );
    
    TGraphAsymmErrors* g_ratio = 0;
    if( data1_ ) g_ratio = MT2DrawTools::getRatioGraph(h1_data, mcBand);
    
    TLine* lineCentral = new TLine(xMin, 1.0, xMax, 1.0);
    lineCentral->SetLineColor(1);
    //TGraphErrors* systBand = MT2DrawTools::getSystBand(xMin, xMax, lumiErr_);
    
    TF1* fSF = (data1_) ? MT2DrawTools::getSFFit(g_ratio, xMin, xMax) : 0;
    TGraphErrors* SFFitBand = (fSF) ? MT2DrawTools::getSFFitBand(fSF, xMin, xMax) : 0;
    
    double errorDataMC = MT2DrawTools::getSFError(integralData, errorData, integralMC, errorMC );

    TCanvas* c1 = new TCanvas(Form("c1_%s", iMT2->getName().c_str()), "", 600, 600);
    c1->cd();
    
    TCanvas* c1_log = new TCanvas(Form("c1_log_%s", iMT2->getName().c_str()), "", 600, 600);

    float yMaxScale = 1.1;
    float yMax1 = (data1_) ? h1_data->GetMaximum()*yMaxScale : 0.;
    float yMax2 = (data1_) ? yMaxScale*(h1_data->GetMaximum() + sqrt(h1_data->GetMaximum())) : 0.;
    float yMax3 = yMaxScale*(bgStack->GetMaximum());
    float yMax = (yMax1>yMax2) ? yMax1 : yMax2;
    if( yMax3 > yMax ) yMax = yMax3;
    if( histo_mc->GetNbinsX()<2 ) yMax *=3.;
    yMax*=1.25;
    
    std::string xAxisTitle;
    if( units!="" ) 
      xAxisTitle = (std::string)(Form("%s [%s]", axisName.c_str(), units.c_str()) );
    else
      xAxisTitle = (std::string)(Form("%s", axisName.c_str()) );

    std::string yAxisTitle;
    if(binWidth>0.99){
      if( units!="" ) 
	yAxisTitle = (std::string)(Form("Events / (%.0f %s)", binWidth, units.c_str()));
      else
	yAxisTitle = (std::string)(Form("Events / (%.0f)", binWidth));
    }
    else if(binWidth>0.099){
      if( units!="" ) 
	yAxisTitle = (std::string)(Form("Events / (%.2f %s)", binWidth, units.c_str()));
      else
	yAxisTitle = (std::string)(Form("Events / (%.2f)", binWidth));
    }
    else{
      if( units!="" ) 
	yAxisTitle = (std::string)(Form("Events / (%.4f %s)", binWidth, units.c_str()));
      else
	yAxisTitle = (std::string)(Form("Events / (%.4f)", binWidth));
    }

    TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., yMax );
    h2_axes->SetXTitle(xAxisTitle.c_str());
    h2_axes->SetYTitle(yAxisTitle.c_str());

    c1->cd();
  
    TPad* pad1 = 0;
    if( this->twoPads() ) {
      pad1 = MT2DrawTools::getCanvasMainPad();
      pad1->Draw();
      pad1->cd();
    }


    h2_axes->Draw();

    float yMin_log = (data1_ && h1_data->GetMinimum()>2.) ? 1. : 0.1;

    TH2D* h2_axes_log = new TH2D("axes_log", "", 10, xMin, xMax, 10, yMin_log, yMax*10.0 );
    h2_axes_log->SetXTitle(xAxisTitle.c_str());
    h2_axes_log->SetYTitle(yAxisTitle.c_str());

    c1_log->cd();

    TPad* pad1_log = 0;
    if( this->twoPads() ) {
      pad1_log = MT2DrawTools::getCanvasMainPad( true );
      pad1_log->Draw();
      pad1_log->cd();
    } else {
      c1_log->SetLogy();
    }

    h2_axes_log->Draw();
   


    std::vector<std::string> niceNames = thisRegion.getNiceNames();
 //   for( unsigned i=0; i<niceNames.size(); ++i ) {
 //
 //     float yMax = 0.9-(float)i*0.05;
 //     float yMin = yMax - 0.05;
 //     TPaveText* regionText = new TPaveText( 0.18, yMin, 0.55, yMax, "brNDC" );
 //     regionText->SetTextSize(0.035);
 //     regionText->SetTextFont(42);
 //     regionText->SetFillColor(0);
 //     regionText->SetTextAlign(11);
 //     regionText->AddText( niceNames[i].c_str() );
 //
 //     //c1->cd();
 //     //regionText->Draw("same");
 // 
 //     //c1_log->cd();
 //     //regionText->Draw("same");
 // 
 //   }


    
    for( unsigned i=0; i<niceNames.size(); ++i ) { 
      
      float yMax = 0.9-(float)i*0.05;
      float yMin = yMax - 0.05;
      TPaveText* regionText = new TPaveText( 0.18, yMin, 0.55, yMax, "brNDC" );
      regionText->SetTextSize(0.030);
      regionText->SetTextFont(42);
      regionText->SetFillColor(0);
      regionText->SetTextAlign(11);

    
      if( i==0 ) {

        if(kinCuts!="") {
          regionText->AddText( kinCuts.c_str() );
        } else {
          regionText->AddText( niceNames[i].c_str() );
        }

      } else if( i==1 ) {
    
        if(topoCuts!="") {
          regionText->AddText( topoCuts.c_str() );
        } else {
          regionText->AddText( niceNames[i].c_str() );
        }

      }
    
      if( this->twoPads() )
        pad1->cd();
      else
        c1->cd();
      regionText->Draw("same");
      
      if( this->twoPads() )
        pad1_log->cd();
      else
        c1_log->cd();
      regionText->Draw("same");
      
    }


    TPaveText* normText = new TPaveText( 0.47, 0.7, 0.62, 0.82, "brNDC" );
    normText->SetFillColor(0);
    normText->SetTextSize(0.035);
    if( scaleFactor!=1. ) {
      if( displaySF_ )
        normText->AddText( Form("#splitline{MC scaled}{by %.2f}", scaleFactor) );
      else
	normText->AddText( "#splitline{Shape}{Norm.}" );
      if( this->twoPads() ) 
        pad1->cd();
      else
        c1->cd();
      //   normText->Draw("same");
      if( this->twoPads() ) 
        pad1_log->cd();
      else
        c1_log->cd();
      //normText->Draw("same");
    }


    int addLines = (data1_) ? 2 : 0;
    TLegend* legend = new TLegend( 0.67, 0.9-(mc1_->size()+addLines)*0.06, 0.93, 0.9 );
    legend->SetTextSize(0.038);
    legend->SetTextFont(42);
    legend->SetFillColor(0);
    //float data_int = 0.;

    if( data1_ ) {
      TString year = to_string(year_);
      TString nameOfDataLegend = "Data " + year;
      if(!data2_){
	legend->AddEntry( gr_data, nameOfDataLegend, "P" );
      }
       //std::cout<< h1_data->GetName() <<std::endl;
      //std::cout << "integral: " << h1_data->Integral(3, -1) << std::endl;
      //std::cout << "data(30-60): "<< h1_data->GetBinContent(3) + h1_data->GetBinContent(4) << std::endl;
      //data_int = h1_data->GetBinContent(3) + h1_data->GetBinContent(4);
    }
    //float mc_int = 0.;
    for( unsigned i=0; i<histos_mc.size(); ++i ) {  
      legend->AddEntry( histos_mc[i], mc1_->at(i)->getFullName().c_str(), "F" );
      //std::cout<< histos_mc[i]->GetName() << std::endl;
      //std::cout << "integral: " << histos_mc[i]->Integral(3, -1) << std::endl;
      //std::cout << "30-60: "<< histos_mc[i]->GetBinContent(3) + histos_mc[i]->GetBinContent(4) << std::endl;
      //mc_int += histos_mc[i]->GetBinContent(3) + histos_mc[i]->GetBinContent(4);
    }
    //float qcdFrac = (histos_mc[0]->GetBinContent(3) + histos_mc[0]->GetBinContent(4) )/ mc_int;
    //std::cout << "qcd frac: " << qcdFrac << std::endl;
    //std::cout << "qcd estimate: " << data_int*qcdFrac << std::endl;
    //std::cout << "qcd true (0-30): " <<  1.27*(histos_mc[0]->GetBinContent(1) + histos_mc[0]->GetBinContent(2)) << std::endl;
    //std::cout << "qcd true (30-60): " << 1.27*(histos_mc[0]->GetBinContent(3) + histos_mc[0]->GetBinContent(3)) << std::endl;
    if( data1_ ) 
      legend->AddEntry( mcBand, "MC Uncert.", "F" );

   
    
    
    TPaveText* fitText = (fSF) ? MT2DrawTools::getFitText( fSF ) : 0;
    
    TPaveText* ratioText = MT2DrawTools::getRatioText( integralData, integralMC, errorDataMC );
    //    TLine* lineSF = MT2DrawTools::getSFLine(integral_data, integral_mc, xMin, xMax);
    //    TGraphErrors* SFband = MT2DrawTools::getSFBand(integral_data, error_data, integral_mc, error_mc, xMin, xMax);
    
    float yMinR=0.0;
    float yMaxR=2.0;
    
    TH2D* h2_axes_ratio = MT2DrawTools::getRatioAxes( xMin, xMax, yMinR, yMaxR );

    std::string CMStext = doPaperPlots_ ? "CMS" : "CMS Preliminary";

    c1->cd();
    if( this->twoPads() )
      pad1->cd();
    legend->Draw("same");
    bgStack->Draw("histo same");
    if( data1_ ) {
      mcBand->Draw("E2 same");
      gr_data->Draw("p same");
    }
    if( !shapeNorm_ && fitText )
      fitText->Draw("same");
    if (shapeNorm_ && ratioText)
      ratioText->Draw("same");

    (data1_) ? MT2DrawTools::addLabels( (TCanvas*)pad1, lumi1_+lumi2_+lumi3_, CMStext.c_str() ) : MT2DrawTools::addLabels( (TCanvas*)c1, lumi1_+lumi2_+lumi3_, "CMS Simulation"); 
    //(data_) ? MT2DrawTools::addLabels( (TCanvas*)pad1, lumi_, CMStext.c_str() ) : MT2DrawTools::addLabels( (TCanvas*)pad1, lumi_, "CMS Simulation"); 

    gPad->RedrawAxis();

    c1_log->cd();
    if( this->twoPads() )
      pad1_log->cd();
    legend->Draw("same");
    bgStack->Draw("histo same");
    if( data1_ ) {
      mcBand->Draw("E2 same");
      gr_data->Draw("p same");
    }
    if( !shapeNorm_ && fitText )
      fitText->Draw("same");
    if (shapeNorm_ && ratioText)
      ratioText->Draw("same");
    
    (data1_) ? MT2DrawTools::addLabels( (TCanvas*)pad1_log, lumi1_+lumi2_+lumi3_, CMStext.c_str() ) : MT2DrawTools::addLabels( (TCanvas*)c1_log, lumi1_+lumi2_+lumi3_, "CMS Simulation"); 
    //(data_) ? MT2DrawTools::addLabels( (TCanvas*)pad1_log, lumi_, CMStext.c_str() ) : MT2DrawTools::addLabels( (TCanvas*)pad1_log, lumi_, "CMS Simulation"); 

    gPad->RedrawAxis();
    
    c1->cd();

    if( twoPads() ) {
      TPad* pad2 = MT2DrawTools::getCanvasRatioPad();
      pad2->Draw();
      pad2->cd();

      h2_axes_ratio->Draw("");
      lineCentral->Draw("same");
      if( !shapeNorm_ ){
        //systBand->Draw("3,same");
        lineCentral->Draw("same");

        if( data1_ ) {
          SFFitBand->Draw("3,same");
          fSF->Draw("same");
        }
//        SFband->Draw("3,same");
//        lineSF->Draw("same");
      }

      if( g_ratio ) g_ratio->Draw("PE,same");    
      gPad->RedrawAxis();


      c1_log->cd();
      TPad* pad2_log = MT2DrawTools::getCanvasRatioPad( true );
      pad2_log->Draw();
      pad2_log->cd();

      h2_axes_ratio->Draw("");
      lineCentral->Draw("same");
      if( !shapeNorm_ ){
        //systBand->Draw("3,same");
        lineCentral->Draw("same");

        if( data1_ ) {
          SFFitBand->Draw("3,same");
          fSF->Draw("same");
        }       
//        SFband->Draw("3,same");
//        lineSF->Draw("same");
      }
      
      if( g_ratio ) g_ratio->Draw("PE,same");
      gPad->RedrawAxis();

    } // if twoPads


    std::string regionSaveName = (MT2Regions.size()!=1) ? "_" + thisRegion.getName() : "";

    c1->SaveAs( Form("%s/%s%s.eps", outdir_.c_str(), saveName.c_str(), regionSaveName.c_str()) );
    c1->SaveAs( Form("%s/%s%s.png", outdir_.c_str(), saveName.c_str(), regionSaveName.c_str()) );
    c1->SaveAs( Form("%s/%s%s.pdf", outdir_.c_str(), saveName.c_str(), regionSaveName.c_str()) );

    c1_log->SaveAs( Form("%s/%s%s_log.eps", outdir_.c_str(), saveName.c_str(), regionSaveName.c_str()) );
    c1_log->SaveAs( Form("%s/%s%s_log.png", outdir_.c_str(), saveName.c_str(), regionSaveName.c_str()) );
    c1_log->SaveAs( Form("%s/%s%s_log.pdf", outdir_.c_str(), saveName.c_str(), regionSaveName.c_str()) );

    returnVector.push_back(c1);

    delete h2_axes;
    delete h2_axes_log;
    delete h2_axes_ratio;
    
    delete h1_data;
    delete h1_data_sep[0];
    delete h1_data_sep[1];
    delete h1_data_sep[2];

    mc_sum = 0;
    histos_mc_sep[0] = 0;
    histos_mc_sep[1] = 0;
    histos_mc_sep[2] = 0;
    
   

   
    //for( unsigned i=0; i<histos_mc.size(); ++i )
    //  delete histos_mc[i];

    delete c1_log;

  }// for MT2 regions

  return returnVector;

}




float MT2DrawTools::getDataMCSF( TCanvas* c1 ) {


  TList* list = getCorrectList( c1 );
  

  TGraphAsymmErrors* gr_data = (TGraphAsymmErrors*)list->FindObject("Graph");

  TH1D* mcBand = (TH1D*)list->FindObject("histo_band");
  float mcInt = mcBand->Integral();

  float dataInt = MT2DrawTools::graphIntegral( gr_data );

  return dataInt/mcInt;

}




float MT2DrawTools::graphIntegral( TGraphAsymmErrors* graph, float xMin, float xMax ) {

  float integral = 0.;

  for( int iPoint=0; iPoint<graph->GetN(); ++iPoint ) {

    Double_t x, y;
    graph->GetPoint( iPoint, x, y );

    if( x >= xMin && x <= xMax ) 
      integral += y;

  }

  return integral;

}

    




TList* MT2DrawTools::getCorrectList( TCanvas* c1 ) {

  TList* list = c1->GetListOfPrimitives();

  if( list->GetSize()==2 ) { // means that it's a two-pad canvas

    TPad* pad1 = (TPad*)list->FindObject("pad1");
    list = pad1->GetListOfPrimitives();

  }

  return list;

}

  


bool MT2DrawTools::twoPads() const {

  return data1_ && mc1_;

}
