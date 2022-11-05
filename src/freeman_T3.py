'''
 A processing error occured ! 
 Not enough input arguments
 Usage:

freeman_decomposition.exe

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

Optional Parameters:
 (string)	-mask	mask file (valid pixels)
 (string)	-errf	memory error file
 (noarg) 	-help	displays this message
 (noarg) 	-data	displays the help concerning Data Format parameter
'''
import os
import sys
args = sys.argv
from fl0w3r import error, normpath, run, read_config
err = error
# PolSARPro executable folder!
d = "/home/" + os.popen("whoami").read().strip() + "/" + "GitHub/polsarpro/Soft/bin/data_process_sngl/"
f = d + 'freeman_decomposition.exe'
print(f)

if len(args) < 3:
    err("freeman_T3 [input dir] [window size]")
cwd = args[1]
if not os.path.exists(cwd) or not os.path.isdir(cwd):
    err("please check input folder")

nwin = int(args[2])
cfg = cwd + "config.txt"

def p(param_name, value):
    return '-' + param_name + ' ' + str(value)

nlin, ncol = read_config(cwd + "config.txt")

run(' '.join([f,
              p('id', cwd),
              p('od', cwd),
              p('iodf', 'T3'),
              p('nwr', nwin),
              p('nwc', nwin),
              p('ofr', '0'),
              p('ofc', '0'),
              p('fnr', nlin),
              p('fnc', ncol)
             ]))






