#!/usr/bin/env python
import os
import sys
from fl0w3r import error, normpath, run, read_config

args = sys.argv

if len(args) < 2:
    print("Usage: cloudeT4 [input directory]")
    print("Generates the polarimetric decomposition parameters of the following binaries:")
    print("\tpolsarpro403/Soft/data_process_sngl/h_a_alpha_decomposition_T4.exe")
    print("\tpolsarpro403/Soft/data_process_sngl/h_a_alpha_eigenvalue_set_T4.exe")
    print("\tpolsarpro403/Soft/data_process_sngl/h_a_alpha_eigenvector_set_T4.exe")
    sys.exit(1)

PSPexeDIR = "/home/" + os.popen("whoami").read().strip() + "/" + "GitHub/polsarpro/Soft/bin/data_process_sngl/" # "GitHub/PolSARpro_v4.2.0_Install_Linux/Soft/data_process_sngl/"
cwd = args[1]
nlin, ncol = read_config(cwd + "config.txt")

out1 = "h_a_alpha_decomposition"
out2 = "h_a_alpha_eigenvalue_set"
out3 = "h_a_alpha_eigenvector_set"

od1 = cwd+out1+'/'
od2 = cwd+out2+'/'
od3 = cwd+out3+'/'

#h_a_alpha_decomposition_T4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol alpbetepsdelgamnhu lambda alpha entropy anisotropy CombHA CombH1mA Comb1mHA Comb1mH1mA
#h_a_alpha_eigenvalue_set_T4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol eigen1234 proba1234 anisotropy anisotropy12 anisotropy34 asymetry polarisation_fraction erd rvi pedestal shannon lueneburg
#h_a_alpha_eigenvector_set_T4 in_dir out_dir Nwin offset_lig offset_col sub_nlig sub_ncol alpha1234 beta1234 epsilon1234 delta1234 gamma1234 nhu1234 alpbetepsdelgamnhu

cmd1 = PSPexeDIR+out1+".exe -iodf T4 -id " + cwd +" -od " + od1 + " -ofr 0 -ofc 0 -nwr 1 -nwc 1 -fnr " + str(nlin)+" -fnc "+str(ncol)+" -fl1 1 -fl2 1 -fl3 1 -fl4 1 -fl5 1 -fl6 1 -fl7 1 -fl8 1 -fl9 1"
cmd2 = PSPexeDIR+out2+".exe -iodf T4 -id " + cwd +" -od " + od2 + " -ofr 0 -ofc 0 -nwr 1 -nwc 1 -fnr " + str(nlin)+" -fnc "+str(ncol)+" -fl1 1 -fl2 1 -fl3 1 -fl4 1 -fl5 1 -fl6 1 -fl7 1 -fl8 1 -fl9 1 -fl10 1 -fl11 1 -fl12 1"
cmd3 = PSPexeDIR+out3+".exe -iodf T4 -id " + cwd +" -od " + od3 + " -ofr 0 -ofc 0 -nwr 1 -nwc 1 -fnr " + str(nlin)+" -fnc "+str(ncol)+" -fl1 1 -fl2 1 -fl3 1 -fl4 1 -fl5 1 -fl6 1 -fl7 1 -fl8 1 -fl9 1"
 
f = open("cloudeT4.sh", "wb");

def r(cmd):
    f.write((cmd + "\n").encode())
    # aa = os.system(cmd)

def w():
    r("wait")

r("mkdir -p " + od1 + " &")
r("mkdir -p " + od2 + " &")
r("mkdir -p " + od3 + " &")
w()

r(cmd1) # + " &")
r(cmd2) # + " &")
r(cmd3) # + " &")
w()

r("cp " + cwd + "config.txt " + od1)
r("cp " + cwd + "config.txt " + od2)
r("cp " + cwd + "config.txt " + od3)
w()

r("eh " + od1 + " &")
r("eh " + od2 + " &")
r("eh " + od3 + " &")
w()

f.close()

a = os.system("chmod 755 cloudeT4.sh")
a = os.system("./cloudeT4.sh")

