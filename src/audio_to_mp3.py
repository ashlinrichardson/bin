'''20230514 Convert all files in present directory, to mp3. Audio format assumed

Application: some audio files have unexpected codecs. Ffmpeg will sort it out!'''
import os
import sys

lines = [x.strip() for x in os.popen("ls -1 ").readlines()]

if not os.path.exists('mp3'):
    os.mkdir('mp3')

out_f = open('multicore.txt', 'wb')
for f in lines:
    w = f.split('.')
    w = w[:-1]
    o_f = '.'.join(w)
    cmd = 'ffmpeg -i ' + f + ' "mp3/' + o_f + '.mp3"'
    print(cmd)
    out_f.write((cmd + '\n').encode())
out_f.close()
print('+w multicore.txt')
print('to finish the conversion run this command:')
print('multicore multicore.txt')
