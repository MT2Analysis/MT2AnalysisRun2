# Script to submit the region creation scripts to batch
import os

def getOptions():

  from argparse import ArgumentParser

  parser = ArgumentParser(description='Launch regions', add_help=True)
  parser.add_argument('-r','--region', type=str, dest='region', help='which region ?', default='all', choices=['llep', 'zll', 'sr', 'all'])
  parser.add_argument('-w', '--what', type=str, dest='what', help='data or mc', default='data', choices=['data', 'mc', 'all'])
  parser.add_argument('-y','--year', type=int, dest='year', help='year of data taking / MC taking :)', default=-1, choices=[2016,2017,2018,-1])
  parser.add_argument('-i', '--incl', type=str, dest='incl', help='Inclusive regions ?', default='all', choices=['incl', 'excl', 'all'])

  return parser.parse_args()

if __name__ == "__main__":

  import subprocess
  logsDir = 'logs'
  # create logs dir
  command = 'mkdir -p {l}'.format(l=logsDir)
  if not os.path.isdir(logsDir):
    subprocess.check_output(command, shell=True)
  #else: raise RuntimeError('logsDir {l} already present please check'.format(l=logsDir))




  options = getOptions()

  if options.region == 'llep': scripts_to_submit = ['llepControlRegion']
  elif options.region == 'zll': scripts_to_submit = ['zllControlRegion']
  elif options.region == 'sr': scripts_to_submit = ['regionEventYields']
  elif options.region == 'all': scripts_to_submit = ['llepControlRegion', 'zllControlRegion','regionEventYields']

  if options.what == 'all':
    whats_to_submit=['data', 'mc', '']
  else:
    whats_to_submit = [options.what]

  if options.year!=-1:
    years_to_submit = [options.year]
  else: 
    years_to_submit = [2016,2017,2018]

  if options.incl=='all':
    incls_to_submit = ['_incl', '']
  elif options.incl=='incl':
    incls_to_submit = ['_incl']
  elif options.incl=='excl':
    incls_to_submit = ['']

  configs_to_submit = []
  
  for year in years_to_submit:
    if year==2016:
      config_lumi = '35p9'
      config_year = ''
    elif year==2017:
      config_lumi = '41p5'
      config_year = '_2017'
    elif year==2018:
      config_lumi = '59p9'
      config_year = '_2018' 

    for config_incl in incls_to_submit:
      
      config_to_submit = 'config_{}ifb{}{}'.format(config_lumi,config_incl,config_year )
      configs_to_submit.append(config_to_submit)
  
      if os.path.isfile('cfgs/{}.txt'.format(config_to_submit))==False:
        raise RuntimeError('config {} not a valid one'.format(config_to_submit))

  
  for script in scripts_to_submit:
    for config in configs_to_submit:
      for what in whats_to_submit:
      
        command = './{} {} {}'.format(script, config, what)
        print command
        template = [
           'source $VO_CMS_SW_DIR/cmsset_default.sh',
           'shopt -s expand_aliases',
           'cmsenv',
           command 
        ]

        template = '\n'.join(template)

        command_nickname = command.replace(' ', '_').replace('./', '')
        print command_nickname

        sub_merge_file = 'submit_{}.sh'.format(command_nickname)
        with open( '{}/{}'.format(logsDir,sub_merge_file), 'w') as f:
          f.write(template)

        qsub_command = 'qsub -o {l} -e {l} -N {n} -q short.q -cwd {l}/{s}'.format(l=logsDir, s=sub_merge_file, n=command_nickname) # short queue should be enough
        # submit
        print qsub_command
        os.system(qsub_command)
        

