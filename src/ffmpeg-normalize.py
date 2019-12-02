import os
import sys
from subprocess import PIPE, Popen

def err(msg):
    print("Error: " + msg)
    sys.exit(1)

def run(cmd):
    print("run " + cmd)
    a = os.system(cmd)
    if a!= 0:
        err("command failed: " + cmd)

args = sys.argv
if len(args) < 3:
    err("ffmpeg-normalize [input file] [output file] (e.g. both files mp3 format)")

in_f = args[1]
out_f = args[2]

if not os.path.exists(in_f):
    err("could not find input file: " + str(in_f))

if os.path.exists(out_f):
    err("output file already exists: " + out_f)

# need to collect data from stderr stream
cmd = "ffmpeg -i \"" + in_f + "\" -filter:a volumedetect -f null /dev/null"
p = Popen(cmd, shell = True, stdout = PIPE, stderr = PIPE)
stdout, stderr = p.communicate() # get both streams


'''
['[Parsed_volumedetect_0', '@', '0x55695fb80ee0]', 'mean_volume:', '-22.1', 'dB']
['[Parsed_volumedetect_0', '@', '0x55695fb80ee0]', 'max_volume:', '-0.5', 'dB']
'''
mean_volume, max_volume = None, None
for line in stderr.split("\n"):
    w = line.strip().split()
    try:
        if w[-1] == 'dB':
            v = w[4] # value in dB
            if w[3] == 'mean_volume:':
                mean_volume = v
            if w[3] == 'max_volume:':
                max_volume = v
    except:
        pass

print("mean_volume", mean_volume)
print("max_volume", max_volume)

# should check first in both cases, before adding quotes

# ffmpeg.org/ffmpeg-filters.html#loudnorm
cmd = 'ffmpeg -i "' + in_f + '" -filter:a loudnorm ' + out_f 
run(cmd)

'''
from https://trac.ffmpeg.org/wiki/AudioVolume

Peak and RMS Normalization

To normalize the volume to a given peak or RMS level, the file first has to be analyzed using the volumedetect filter:

ffmpeg -i input.wav -filter:a volumedetect -f null /dev/null

Read the output values from the command line log:

[Parsed_volumedetect_0 @ 0x7f8ba1c121a0] mean_volume: -16.0 dB
[Parsed_volumedetect_0 @ 0x7f8ba1c121a0] max_volume: -5.0 dB
...

... then calculate the required offset, and use the volume filter as shown above.
Loudness Normalization

If you want to normalize the (perceived) loudness of the file, use the loudnorm filter, which implements the EBU R128 algorithm:

ffmpeg -i input.wav -filter:a loudnorm output.wav

This is recommended for most applications, as it will lead to a more uniform loudness level compared to simple peak-based normalization. However, it is recommended to run the normalization with two passes, extracting the measured values from the first run, then using the values in a second run with linear normalization enabled. See the loudnorm filter documentation for more. 
'''
