import re




if __name__ == "__main__":

  fileName = 'list_branches_to_add.txt'

  branches_to_add = []

  with open(fileName, 'r') as f:
    for line in f:
      if line=='\n': continue
      if '#' in line: continue

      data = re.split(' |\n', line)
      data = filter(lambda x: x!='', data)
      #print data
      branchType,branchName = data[0], data[1]

      branches_to_add.append((branchType, branchName))

  first_block_lines = []
  for t,n in branches_to_add:
    #   UInt_t          run;
    first_block_lines.append('   {:16}{};'.format(t, n))

  print ''
  print '\n'.join(first_block_lines)
  
  second_block_lines = []
  for t,n in branches_to_add:
    #   TBranch        *b_HLT_PFMET100_PFMHT100_IDTight_PFHT60;   //!
    second_block_lines.append('   {:16}*b_{};     //!'.format('TBranch', n))

  print ''
  print '\n'.join(second_block_lines)

  third_block_lines = []
  for t,n in branches_to_add:
    #   fChain->SetBranchAddress("run", &run, &b_run);
    third_block_lines.append('   fChain->SetBranchAddress("{}", &{}, &b_{}'.format(n, n, n))
  
  print ''
  print '\n'.join(third_block_lines)


