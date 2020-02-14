import os
for filename in os.listdir("/scratch/wjin/featurereduced4/bkg_score/"):
    if filename.endswith("binV5.npy") and (not os.path.isfile("/scratch/wjin/featurereduced4/bkg_score/"+filename[:-4]+".root")):
        if "T1bbbb" in filename:
            signal="T1bbbb"
        elif "T1qqqq" in filename:
            signal="T1qqqq"
        elif "T2bb" in filename:
            signal="T2bb"
        elif "T2qq" in filename:
            signal="T2qq"
#        command="sbatch -p wn --mem=8000 --account=t3 -o "+"create_"+filename.replace(".npy",".txt")+" -e "+"create_"+filename.replace(".npy",".txt")+" --job-name=create --ntasks=1 batch_create.sh"+" "+"/scratch/wjin/featurereduced4/bkg_score/"+filename+" "+signal
        command="bash batch_create.sh"+" "+"/scratch/wjin/featurereduced4/bkg_score/"+filename+" "+signal
        print command
        os.system(command)

for filename in os.listdir("/scratch/wjin/featurereduced4/llcontrol_score/"):
    if filename.endswith("binV5.npy") and (not os.path.isfile("/scratch/wjin/featurereduced4/llcontrol_score/"+filename[:-4]+".root")):
        if "T1bbbb" in filename:
            signal="T1bbbb"
        elif "T1qqqq" in filename:
            signal="T1qqqq"
        elif "T2bb" in filename:
            signal="T2bb"
        elif "T2qq" in filename:
            signal="T2qq"
#        command="sbatch -p wn --mem=8000 --account=t3 -o "+"create_"+filename.replace(".npy",".txt")+" -e "+"create_"+filename.replace(".npy",".txt")+" --job-name=create --ntasks=1 batch_create.sh"+" "+"/scratch/wjin/featurereduced4/llcontrol_score/"+filename+" "+signal
        command="bash batch_create.sh"+" "+"/scratch/wjin/featurereduced4/llcontrol_score/"+filename+" "+signal
        print command
        os.system(command)

for filename in os.listdir("/scratch/wjin/featurereduced4/zinvcontrol_score/"):
    if filename.endswith("binV5.npy") and (not os.path.isfile("/scratch/wjin/featurereduced4/zinvcontrol_score/"+filename[:-4]+".root")):
        if "T1bbbb" in filename:
            signal="T1bbbb"
        elif "T1qqqq" in filename:
            signal="T1qqqq"
        elif "T2bb" in filename:
            signal="T2bb"
        elif "T2qq" in filename:
            signal="T2qq"
#        command="sbatch -p wn --mem=8000 --account=t3 -o "+"create_"+filename.replace(".npy",".txt")+" -e "+"create_"+filename.replace(".npy",".txt")+" --job-name=create --ntasks=1 batch_create.sh"+" "+"/scratch/wjin/featurereduced4/zinvcontrol_score/"+filename+" "+signal
        command="bash batch_create.sh"+" "+"/scratch/wjin/featurereduced4/zinvcontrol_score/"+filename+" "+signal
        print command
        os.system(command)

for filename in os.listdir("/scratch/wjin/featurereduced4/sig_score/"):
    if filename.endswith("binV5.npy") and (not os.path.isfile("/scratch/wjin/featurereduced4/sig_score/"+filename[:-4]+".root")):
        if "T1bbbb" in filename:
            signal="T1bbbb"
        elif "T1qqqq" in filename:
            signal="T1qqqq"
        elif "T2bb" in filename:
            signal="T2bb"
        elif "T2qq" in filename:
            signal="T2qq"
#        command="sbatch -p wn --mem=8000 --account=t3 -o "+"create_"+filename.replace(".npy",".txt")+" -e "+"create_"+filename.replace(".npy",".txt")+" --job-name=create --ntasks=1 batch_create.sh"+" "+"/scratch/wjin/featurereduced4/sig_score/"+filename+" "+signal
        command="bash batch_create.sh"+" "+"/scratch/wjin/featurereduced4/sig_score/"+filename+" "+signal
        print command
        os.system(command)

for filename in os.listdir("/scratch/wjin/featurereduced4/datasigregion_score/"):
    if filename.endswith("binV5.npy") and (not os.path.isfile("/scratch/wjin/featurereduced4/datasigregion_score/"+filename[:-4]+".root")):
        if "T1bbbb" in filename:
            signal="T1bbbb"
        elif "T1qqqq" in filename:
            signal="T1qqqq"
        elif "T2bb" in filename:
            signal="T2bb"
        elif "T2qq" in filename:
            signal="T2qq"
#        command="sbatch -p wn --mem=8000 --account=t3 -o "+"create_"+filename.replace(".npy",".txt")+" -e "+"create_"+filename.replace(".npy",".txt")+" --job-name=create --ntasks=1 batch_create.sh"+" "+"/scratch/wjin/featurereduced4/datasigregion_score/"+filename+" "+signal
        command="bash batch_create.sh"+" "+"/scratch/wjin/featurereduced4/datasigregion_score/"+filename+" "+signal
        print command
        os.system(command)
