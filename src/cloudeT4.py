#!/usr/bin/env python
import os
import sys
from fl0w3r import error, normpath, run, read_config

args = sys.argv

if len(args) < 2:
    print "Usage: cloudeT4 [input directory]"
	  print "Generates the polarimetric decomposition parameters of the following binaries:"
	  print "\tpolsarpro403/Soft/data_process_sngl/h_a_alpha_decomposition_T4.exe"
	  print "\tpolsarpro403/Soft/data_process_sngl/h_a_alpha_eigenvalue_set_T4.exe"
	  print "\tpolsarpro403/Soft/data_process_sngl/h_a_alpha_eigenvector_set_T4.exe"
    sys.exit(1)

PSPexeDIR = "../../PolSARpro_v4.2.0_Install_Linux/Soft/data_process_sngl/"

cwd = args[1]

nlin, ncol = read_config(cwd + "config.txt")

out1 = "h_a_alpha_decomposition_T4"
out2 = "h_a_alpha_eigenvalue_set_T4"
out3 = "h_a_alpha_eigenvector_set_T4"

od1 = cwd+out1+'/'
od2 = cwd+out2+'/'
od3 = cwd+out3+'/'

#h_a_alpha_decomposition_T4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol alpbetepsdelgamnhu lambda alpha entropy anisotropy CombHA CombH1mA Comb1mHA Comb1mH1mA
#h_a_alpha_eigenvalue_set_T4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol eigen1234 proba1234 anisotropy anisotropy12 anisotropy34 asymetry polarisation_fraction erd rvi pedestal shannon lueneburg
#h_a_alpha_eigenvector_set_T4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol alpha1234 beta1234 epsilon1234 delta1234 gamma1234 nhu1234 alpbetepsdelgamnhu

cmd1 = PSPexeDIR+out1+".exe " + cwd +" "+od1+" 1 0 0 "+str(nlin)+" "+str(ncol)+" 1 1 1 1 1 1 1 1 1 "
cmd2 = PSPexeDIR+out2+".exe " + cwd +" "+od2+" 1 0 0 "+str(nlin)+" "+str(ncol)+" 1 1 1 1 1 1 1 1 1 1 1 1"
cmd3 = PSPexeDIR+out3+".exe " + cwd +" "+od3+" 1 0 0 "+str(nlin)+" "+str(ncol)+" 1 1 1 1 1 1 1 "
 
print "mkdir "+od1
print "mkdir "+od2
print "mkdir "+od3

print cmd1
print cmd2
print cmd3

run("mkdir "+od1)
run("mkdir "+od2)
run("mkdir "+od3)

run(cmd1)
run(cmd2)
run(cmd3)

run("cp "+cwd+"config.txt "+od1)
run("cp "+cwd+"config.txt "+od2)
run("cp "+cwd+"config.txt "+od3)

run("eh "+od1)
run("eh "+od2)
run("eh "+od3)
