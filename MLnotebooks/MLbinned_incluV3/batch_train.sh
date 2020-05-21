sbatch  -p gpu --time=2-03:30  --account=gpu_gres --gres=gpu:1  -o training_T1bbbb.log -e training_T1bbbb.log  --job-name=NLLtrain batch_NLL.sh T1bbbb 
sbatch  -p gpu --time=2-03:30  --account=gpu_gres --gres=gpu:1  -o training_T1qqqq.log -e training_T1qqqq.log  --job-name=NLLtrain batch_NLL.sh T1qqqq
sbatch  -p gpu --time=2-03:30  --account=gpu_gres --gres=gpu:1  -o training_T2bb.log -e training_T2bb.log  --job-name=NLLtrain batch_NLL.sh T2bb
sbatch  -p gpu --time=2-03:30  --account=gpu_gres --gres=gpu:1  -o training_T2qq.log -e training_T2qq.log  --job-name=NLLtrain batch_NLL.sh T2qq
