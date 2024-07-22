#!/usr/bin/env python
'''revisited 20170618

todo:
- this script might be redundant..check for others
- need to clean it up more, and
- run it

run PolSARPro decompositions on T3 matrix'''
import os
import sys
from misc import read_config, error, normpath, run

'''this variable represents the (local) root path..
The basis of the symbols relevant to this operation'''

mypath = normpath("./")  # /Volumes/ash/C4/
# outpath = /Users/ash/Desktop/keyriver/

lines = [ ]  # os.popen('find ./ -name "' + mynamesearchstring+'"').readlines()
lines.append(normpath('./'))

#TYPE: data_convert_MLK_T3 DirInput DirOutput Ncol OffsetLig OffsetCol FinalNlig FinalNcol Symmetrisation OutputDataFormat Nlook_col Nlook_lig

#h_a_alpha_decomposition_C4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol alpbetepsdelgamnhu lambda alpha entropy anisotropy CombHA CombH1mA Comb1mHA Comb1mH1mA

#h_a_alpha_eigenvalue_set_C4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol eigen1234 proba1234 anisotropy anisotropy12 anisotropy34 asymetry polarisation_fraction erd rvi pedestal shannon lueneburg

#h_a_alpha_eigenvector_set_C4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol alpha1234 beta1234 epsilon1234 delta1234 gamma1234 nhu1234 alpbetepsdelgamnhu

sep = " "

for line in lines:
  path = normpath(line.strip())
	# print(path) 
	opath = path
	# print("\t"+opath);
	if not exists(opath):
      run("mkdir -p " + opath)
	
  # read image dimensions from config.txt file
  nrow, ncol = rowcol( path+"/config.txt")
	
  # cmd = "./data_convert_MLK_T3.exe "+ path+sep+opath+sep+ncol+" 0 0 " +nrow+sep+ncol+ " 0 T3 5 5"
  nrow, ncol = read_config( path+"/config.txt");
	
  print(path + " nrow "+ nrow + " ncol "+ ncol)
	#a = os.system(cmd) 
  run("h_a_alpha_eigenvector_set_T3.exe " + path+sep+opath+sep+"1 0 0 "+nrow+sep+ncol+" 1 1 1 1 1 ") 

	run("h_a_alpha_eigenvalue_set_T3.exe "+ path + sep + opath + sep+ "1 0 0 "+nrow+sep+ncol+" 1 1 1 1 1 1 1 1 1 1 1 ")

  run("h_a_alpha_decomposition_T3.exe "+ path + sep + opath + sep + " 1 0 0 " + nrow + sep + ncol + " 1 1 1 1 1 1 1 1 1 ")
