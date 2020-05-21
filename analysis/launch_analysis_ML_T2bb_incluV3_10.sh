#!bin/bash      

#################################################################
#                   User's decision board                       #


#--------------------------------------------------------------
#     1. On which year do you want to run the analysis?

#Make analysis on combined years?
doCombinedYears=true


#If not, which year would you like to process?
year=2017

#--------------------------------------------------------------
#      2. Do you want to run the full analysis?

doFullAnalysis=false


#--------------------------------------------------------------
#      3. If not, select what you want to do

doMakeClean=false #do you want to clean all the .o files?

doCreationNtuples=true #do you want to create SR and CR ntuples?

doComputationEstimates=true #do you want to compute the Lost Lepton and Z invisible estimates?

doDatacard=true #do you want to create the datacards?

doSRplot=true #do you want to produce the SR plots?

doSignal=true

Signaltype="T2bb"

cfg2016="config_MLoptimize_2016_ForT2bb_incluV3_10"
#cfg2016="config_MLoptimize_2016_ForT1bbbb_btagexclu_noMLcutallevents_o_o"
#cfg2016="config_MLoptimize_2016_ForT1bbbb_btagexclu_draw"
cfg2017="config_MLoptimize_2017_ForT2bb_incluV3_10"
#cfg2017="config_MLoptimize_2017_ForT1bbbb_btagexclu_noMLcutallevents_o_o"
#cfg2017="config_MLoptimize_2017_ForT1bbbb_btagexclu_draw"
cfg2018="config_MLoptimize_2018_ForT2bb_incluV3_10"
#cfg2018="config_MLoptimize_2018_ForT1bbbb_btagexclu_noMLcutallevents_o_o"
#cfg2018="config_MLoptimize_2018_ForT1bbbb_btagexclu_draw"
#################################################################



if [ "$doFullAnalysis" = true ] ; then 
    doMakeClean=true
    doCreationNtuples=true
    doComputationEstimates=true
    doDatacard=true
    doSRplot=true
    doSignal=true
fi
 

echo " "
echo " "
echo " "
echo "-----------------------------------------------------------"
echo " "
echo "                    MT2 Analysis                           "
echo " "
echo "-----------------------------------------------------------"
echo " "
echo " "




if [ "$doFullAnalysis" = true ] && [ "$doCombinedYears" = true ] ; then
    echo "--> The full analysis will be run on the full Run2 dataset <--"
fi
if [ "$doFullAnalysis" = true ] && [ "$year" == 2016 ] && [ "$doCombinedYears" = false ] ; then
    echo "--> The full analysis will be run on the 2016 dataset <--"
fi
if [ "$doFullAnalysis" = true ] && [ "$year" == 2017 ] && [ "$doCombinedYears" = false ] ; then
    echo "--> The full analysis will be run on the 2017 dataset <--"
fi
if [ "$doFullAnalysis" = true ] && [ "$year" == 2018 ] && [ "$doCombinedYears" = false ] ; then
    echo "--> The full analysis will be run on the 2018 dataset <--"
fi
if [ "$doFullAnalysis" = false ] && [ "$doCombinedYears" = true ] ; then
    echo "--> Partial analysis will be run on the full Run2 dataset <--"
fi
if [ "$doFullAnalysis" = false ] && [ "$year" == 2016 ] && [ "$doCombinedYears" = false ] ; then
    echo "--> Partial analysis will be run on the 2016 dataset <--"
fi
if [ "$doFullAnalysis" = false ] && [ "$year" == 2017 ] && [ "$doCombinedYears" = false ] ; then
    echo "--> Partial analysis will be run on the 2017 dataset <--"
fi
if [ "$doFullAnalysis" = false ] && [ "$year" == 2018 ] && [ "$doCombinedYears" = false ] ; then
    echo "--> Partial analysis will be run on the 2018 dataset <--"
fi

echo " "


mkdir -p logs_T2bb #this directory will contain all the log files

if [ "$doFullAnalysis" = true ] || [ "$doMakeClean" = true ] ; then
    make clean
fi

echo "->Compilation of all the necessary files"

make llepControlRegion > logs_T2bb/log_compilation_llepCR_incluV3_10.txt
make zllControlRegion > logs_T2bb/log_compilation_zllCR_incluV3_10.txt   
make regionEventYields > logs_T2bb/log_compilation_yields_incluV3_10.txt   
make computeLostLepton > logs_T2bb/log_compilation_llepEstimatate_incluV3_10.txt
make computeLostLepton_combined > logs_T2bb/log_compilation_llepEstimate_combined_incluV3_10.txt 
make computeZinvFromZll > logs_T2bb/log_compilation_zinvEstimate_incluV3_10.txt 
make computeZinvFromZll_combined > logs_T2bb/log_compilation_zinvEstimate_combined_incluV3_10.txt 
make createDatacards > logs_T2bb/log_compilation_datacards_incluV3_10.txt
make createDatacards_combined > logs_T2bb/log_compilation_datacards_combined_incluV3_10.txt
make compareYield_bins_all > logs_T2bb/log_compilation_SRplots_incluV3_10.txt   
                                                                                                                                                               

echo "-----------------------------------------------------------------------"
echo "                        IMPORTANT STATEMENT                            "
echo "  Make sure that the /EventYields_*/datacard_templates rep are empty   "
echo "-----------------------------------------------------------------------"

if [ "$doSignal" = true ] ; then
    echo "-> do signal selection"

    if [ "$year" = 2016 ] || [ "$doCombinedYears" = true ] ; then
         ./regionEventYields $cfg2016 signal  $Signaltype >& logs_T2bb/log_signal_2016_incluV3_10.txt &
    fi

    if [ "$year" = 2017 ] || [ "$doCombinedYears" = true ] ; then
         ./regionEventYields $cfg2017 signal  $Signaltype >& logs_T2bb/log_signal_2017_incluV3_10.txt &
    fi

    if [ "$year" = 2018 ] || [ "$doCombinedYears" = true ] ; then
         ./regionEventYields $cfg2018 signal  $Signaltype >& logs_T2bb/log_signal_2018_incluV3_10.txt &
    fi
    echo "Done with the signal analysis"

fi


if [ "$doCreationNtuples" = true ] ; then 
    echo "->Start of the single lepton CR ntuples creation process"
    if [ "$year" = 2016 ] || [ "$doCombinedYears" = true ] ; then
	./llepControlRegion $cfg2016 data >& logs_T2bb/log_llepCR_data16_incluV3_10.txt &
	./llepControlRegion $cfg2016 mc >& logs_T2bb/log_llepCR_mc16_incluV3_10.txt &
    fi
    if [ "$year" = 2017 ] || [ "$doCombinedYears" = true ] ; then
	./llepControlRegion $cfg2017 data >& logs_T2bb/log_llepCR_data17_incluV3_10.txt &
	./llepControlRegion $cfg2017 mc >& logs_T2bb/log_llepCR_mc17_incluV3_10.txt &
    fi
    if [ "$year" = 2018 ] || [ "$doCombinedYears" = true ] ; then
	./llepControlRegion $cfg2018 data >& logs_T2bb/log_llepCR_data18_incluV3_10.txt &
	./llepControlRegion $cfg2018 mc >& logs_T2bb/log_llepCR_mc18_incluV3_10.txt &
    fi
    
    echo "Launched creation of the single lepton CR ntuples"
    echo "->Start of the di-lepton CR ntuples creation process"
    
    if [ "$year" = 2016 ] || [ "$doCombinedYears" = true ] ; then
	./zllControlRegion $cfg2016 data >& logs_T2bb/log_zllCR_data16_incluV3_10.txt &
	./zllControlRegion $cfg2016 mc >& logs_T2bb/log_zllCR_mc16_incluV3_10.txt &
    fi
    if [ "$year" = 2017 ] || [ "$doCombinedYears" = true ] ; then
	./zllControlRegion $cfg2017 data >& logs_T2bb/log_zllCR_data17_incluV3_10.txt &
	./zllControlRegion $cfg2017 mc >& logs_T2bb/log_zllCR_mc17_incluV3_10.txt &
    fi
    if [ "$year" = 2018 ] || [ "$doCombinedYears" = true ] ; then
	./zllControlRegion $cfg2018 data >& logs_T2bb/log_zllCR_data18_incluV3_10.txt &
	./zllControlRegion $cfg2018 mc >& logs_T2bb/log_zllCR_mc18_incluV3_10.txt &
    fi
    
    echo "Launched the creation of the di-lepton CR ntuples"
    
    echo "->Creation of the SR ntuples"
    
    if [ "$year" = 2016 ] && [ "$doCombinedYears" = false ] ; then
	./regionEventYields $cfg2016 data >& logs_T2bb/log_yields_data16_incluV3_10.txt & 
	echo "work in progress ... takes some time (~1h)"
	./regionEventYields $cfg2016 mc >& logs_T2bb/log_yields_mc16_incluV3_10.txt 
    fi
    if [ "$year" = 2017 ] && [ "$doCombinedYears" = false ] ; then
	./regionEventYields $cfg2017 data >& logs_T2bb/log_yields_data17_incluV3_10.txt &
	echo "work in progress ... takes some time (~1h)"
	./regionEventYields $cfg2017 mc >& logs_T2bb/log_yields_mc17_incluV3_10.txt
    fi
    if [ "$year" = 2018 ] && [ "$doCombinedYears" = false ] ; then
	./regionEventYields $cfg2018 data >& logs_T2bb/log_yields_data18_incluV3_10.txt &
	echo "work in progress ... takes some time (~1h)"
	./regionEventYields $cfg2018 mc >& logs_T2bb/log_yields_mc18_incluV3_10.txt 
    fi
    
    if [ "$doCombinedYears" = true ] ; then
	./regionEventYields $cfg2016 data >& logs_T2bb/log_yields_data16_incluV3_10.txt & 
	./regionEventYields $cfg2016 mc >& logs_T2bb/log_yields_mc16_incluV3_10.txt &
	./regionEventYields $cfg2017 data >& logs_T2bb/log_yields_data17_incluV3_10.txt &
	./regionEventYields $cfg2018 data >& logs_T2bb/log_yields_data18_incluV3_10.txt &
	./regionEventYields $cfg2018 mc >& logs_T2bb/log_yields_mc18_incluV3_10.txt &
	echo "work in progress ... takes some time (~1h)"
	./regionEventYields $cfg2017 mc >& logs_T2bb/log_yields_mc17_incluV3_10.txt
    fi
    
    echo "Done with the creation of the SR ntuples"
fi

wait

if [ "$doComputationEstimates" = true ] ; then
    echo "->Starting the estimate computation"

    if [ "$year" = 2016 ] && [ "$doCombinedYears" = false ] ; then
	./computeLostLepton $cfg2016 > logs_T2bb/log_llepEstimate_16_incluV3_10.txt 
    fi
    if [ "$year" = 2017 ] && [ "$doCombinedYears" = false ] ; then
	./computeLostLepton $cfg2017 > logs_T2bb/log_llepEstimate_17_incluV3_10.txt 
    fi
    if [ "$year" = 2018 ] && [ "$doCombinedYears" = false ] ; then
	./computeLostLepton $cfg2018 > logs_T2bb/log_llepEstimate_18_incluV3_10.txt 
    fi
    if [ "$doCombinedYears" = true ] ; then
	echo "work in progress - please wait"
	./computeLostLepton_combined $cfg2016 $cfg2017 $cfg2018 > logs_T2bb/log_llepEstimate_combined_incluV3_10.txt 
    fi

    echo "Done with the Lost Lepton estimate"

    echo "->Computation of the Z invisible estimate"
    if [ "$year" = 2016 ] && [ "$doCombinedYears" = false ] ; then
	./computeZinvFromZll $cfg2016 > logs_T2bb/log_zinvEstimate_16_incluV3_10.txt 
    fi
    if [ "$year" = 2017 ] && [ "$doCombinedYears" = false ] ; then
	./computeZinvFromZll $cfg2017 > logs_T2bb/log_zinvEstimate_17_incluV3_10.txt 
    fi
    if [ "$year" = 2018 ] && [ "$doCombinedYears" = false ] ; then
	./computeZinvFromZll $cfg2018 > logs_T2bb/log_zinvEstimate_18_incluV3_10.txt 
    fi
    if [ "$doCombinedYears" = true ] ; then
	echo "work in progress - please wait"
	./computeZinvFromZll_combined $cfg2016 $cfg2017 $cfg2018 > logs_T2bb/log_zinvEstimate_combined_incluV3_10.txt
    fi
    
    echo "Done with the Z invisible estimate"

    echo "--> DONE WITH COMPUTATION OF THE ESTIMATES"
fi

if [ "$doDatacard" = true ] ; then
    echo "->Creation of the datacards"

    if [ "$year" = 2016 ] && [ "$doCombinedYears" = false ] ; then
	./createDatacards $cfg2016 T2bb 10 10 10 10 > logs_T2bb/log_datacard_16_incluV3_10.txt 
    fi
    if [ "$year" = 2017 ] && [ "$doCombinedYears" = false ] ; then
	./createDatacards $cfg2017 T2bb 10 10 10 10 > logs_T2bb/log_datacard_17_incluV3_10.txt 
    fi
    if [ "$year" = 2018 ] && [ "$doCombinedYears" = false ] ; then
	./createDatacards $cfg2018 T2bb 10 10 10 10 > logs_T2bb/log_datacard_18_incluV3_10.txt 
    fi   
    if [ "$doCombinedYears" = true ] ; then
	./createDatacards_combined $cfg2016 $cfg2017 $cfg2018 T2bb 10 10 10 10 > logs_T2bb/log_datacard_combined_incluV3_10.txt 
    fi

    echo "-->Done with the creation of the datacards"
fi

if [ "$doSRplot" = true ] ; then
    echo "->Making SR plots"
    
    if [ "$year" = 2016 ] && [ "$doCombinedYears" = false ] ; then
	./compareYield_bins_all $cfg2016 > logs_T2bb/log_SRplot_16_incluV3_10.txt 
    fi
    if [ "$year" = 2017 ] && [ "$doCombinedYears" = false ] ; then
	./compareYield_bins_all $cfg2017 > logs_T2bb/log_SRplot_17_incluV3_10.txt
    fi
    if [ "$year" = 2018 ] && [ "$doCombinedYears" = false ] ; then 
	./compareYield_bins_all $cfg2018 > logs_T2bb/log_SRplot_18_incluV3_10.txt 
    fi
    if [ "$doCombinedYears" = true ] ; then
	./compareYield_bins_all $cfg2016 $cfg2017 $cfg2018 > logs_T2bb/log_SRplot_combined_incluV3_10.txt
    fi

    echo "Done with the SR plots"
fi


echo "--> DONE"


