import os

directory="/work/wjin/featurereduced4/rootfiles/2016/"
targetdir="/work/wjin/featurereduced4/bkg_score/"
signaltype=["T1bbbb","T1qqqq","T2bb","T2qq"]
treename="mt2"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2016_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30  --account=gpu_gres --gres=gpu:1  -o "+"logs/evabkg_2016_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evabkg_2016_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)


directory="/work/wjin/featurereduced4/rootfiles_llcontrol/2016/"
targetdir="/work/wjin/featurereduced4/llcontrol_score/"
signaltype=["T1bbbb","T1qqqq","T2bb","T2qq"]
treename="mt2"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2016_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evall_2016_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evall_2016_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_zinvcontrol/2016/"
targetdir="/work/wjin/featurereduced4/zinvcontrol_score/"
signaltype=["T1bbbb","T1qqqq","T2bb","T2qq"]
treename="mt2"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2016_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evazll_2016_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evazll_2016_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles/2016/signal/"
targetdir="/work/wjin/featurereduced4/sig_score/"
signaltype=["T1bbbb","T1qqqq","T2bb","T2qq"]
treename="mt2"

for filename in os.listdir(directory):
    for signal in signaltype:
        if filename.endswith(".root") and (signal in filename) :
            inputfile = directory+filename
            outputfile= targetdir+"score1_2016_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-7:30  --account=gpu_gres --gres=gpu:1  -o "+"logs/evasig_2016_"+filename.replace(".root",".txt")+" -e "+"logs/evasig_2016_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)


directory="/work/wjin/featurereduced4/rootfiles/2016/data/"
targetdir="/work/wjin/featurereduced4/datasigregion_score/"
treename="Events"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2016_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evadatasr_2016_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evadatasr_2016_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_llcontrol/2016/data/"
targetdir="/work/wjin/featurereduced4/llcontrol_score/"
treename="Events"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2016_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evadatall_2016_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evadatall_2016_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_zinvcontrol/2016/data/"
targetdir="/work/wjin/featurereduced4/zinvcontrol_score/"
treename="Events"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2016_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evadatazll_2016_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evadatazll_2016_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)


directory="/work/wjin/featurereduced4/rootfiles/2017/"
targetdir="/work/wjin/featurereduced4/bkg_score/"
treename="mt2"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2017_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evabkg_2017_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evabkg_2017_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_llcontrol/2017/"
targetdir="/work/wjin/featurereduced4/llcontrol_score/"
treename="mt2"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2017_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evall_2017_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evall_2017_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_zinvcontrol/2017/"
targetdir="/work/wjin/featurereduced4/zinvcontrol_score/"
treename="mt2"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2017_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evazll_2017_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evazll_2017_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)


directory="/work/wjin/featurereduced4/rootfiles/2017/signal/"
targetdir="/work/wjin/featurereduced4/sig_score/"
treename="mt2"

for filename in os.listdir(directory):
    for signal in signaltype:
        if filename.endswith(".root") and (signal in filename):
            inputfile = directory+filename
            outputfile= targetdir+"score1_2017_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-07:30  --account=gpu_gres --gres=gpu:1  -o "+"logs/evasig_2017_"+filename.replace(".root",".txt")+" -e "+"logs/evasig_2017_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles/2017/data/"
targetdir="/work/wjin/featurereduced4/datasigregion_score/"
treename="Events"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2017_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evadata_2017_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evadata_2017_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_llcontrol/2017/data/"
targetdir="/work/wjin/featurereduced4/llcontrol_score/"
treename="Events"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2017_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evadatall_2017_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evadatall_2017_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_zinvcontrol/2017/data/"
targetdir="/work/wjin/featurereduced4/zinvcontrol_score/"
treename="Events"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2017_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evadatazll_2017_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evadatazll_2017"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles/2018/"
targetdir="/work/wjin/featurereduced4/bkg_score/"
treename="mt2"
for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2018_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evabkg_2018_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evabkg_2018_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_llcontrol/2018/"
targetdir="/work/wjin/featurereduced4/llcontrol_score/"
treename="mt2"
for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2018_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu  --time=0-1:30  --account=gpu_gres --gres=gpu:1  -o "+"logs/evall_2018_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evall_2018_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_zinvcontrol/2018/"
targetdir="/work/wjin/featurereduced4/zinvcontrol_score/"
treename="mt2"
for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2018_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evazll_2018_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evazll_2018_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)



directory="/work/wjin/featurereduced4/rootfiles/2018/data/"
targetdir="/work/wjin/featurereduced4/datasigregion_score/"
treename="Events"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2018_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evadata_2018_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evadata_2018_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_llcontrol/2018/data/"
targetdir="/work/wjin/featurereduced4/llcontrol_score/"
treename="Events"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2018_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evadatall_2018_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evadatall_2018_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)

directory="/work/wjin/featurereduced4/rootfiles_zinvcontrol/2018/data/"
targetdir="/work/wjin/featurereduced4/zinvcontrol_score/"
treename="Events"

for filename in os.listdir(directory):
    if filename.endswith(".root"):
        for signal in signaltype:
            inputfile = directory+filename
            outputfile= targetdir+"score1_for"+signal+"_2018_"+filename.replace(".root","_binV10.npy")
            if os.path.isfile(outputfile):
                continue
            command="sbatch  -p gpu --time=0-1:30 --account=gpu_gres --gres=gpu:1  -o "+"logs/evadatazll_2018_"+signal+"_"+filename.replace(".root",".txt")+" -e "+"logs/evadatazll_2018_"+signal+"_"+filename.replace(".root",".txt")+" --job-name=scoreeval batch_try.sh"+" "+inputfile+" "+outputfile+" "+treename+" "+signal
            print(command)
            os.system(command)
