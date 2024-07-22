#!/usr/bin/env python
import os
import sys
from misc import error, normpath, run, read_config
'''
tsvm_decomposition.exe

Parameters:
 (string)	-id  	input directory
 (string)	-od  	output directory
 (string)	-iodf	input-output data format
 (int)   	-nwr 	Nwin Row
 (int)   	-nwc 	Nwin Col
 (int)   	-ofr 	Offset Row
 (int)   	-ofc 	Offset Col
 (int)   	-fnr 	Final Number of Row
 (int)   	-fnc 	Final Number of Col
 (int)   	-fl1 	Flag AlpPhiTauPsi
 (int)   	-fl2 	Flag Alpha
 (int)   	-fl3 	Flag Phi
 (int)   	-fl4 	Flag Tau
 (int)   	-fl5 	Flag Psi

Optional Parameters:
 (string)	-mask	mask file (valid pixels)
 (string)	-errf	memory error file
 (noarg) 	-help	displays this message
 (noarg) 	-data	displays the help concerning Data Format parameter
'''


PSPexeDIR = "/home/" + os.popen("whoami").read().strip() + "/" + "GitHub/polsarpro/Soft/bin/data_process_sngl/" # "GitHub/PolSARpro_v4.2.0_Install_Linux/Soft/data_process_sngl/"
# cwd = args[1]
cwd = "./"
run("eh2cfg ./T11.hdr")
nlin, ncol = read_config(cwd + "config.txt")

out1 = "tsvm_decomposition"

od1 = od2 = od3 = "./"
cmd1 = PSPexeDIR+out1+".exe -iodf T3 -id " + cwd + " -od " + od1 + " -ofr 0 -ofc 0 -nwr 1 -nwc 1 -fnr " + str(nlin)+" -fnc "+str(ncol)+" -fl1 1 -fl2 1 -fl3 1 -fl4 1 -fl5 1"
run(cmd1)
run("eh ./")
run("envi_header_cleanup.py")
