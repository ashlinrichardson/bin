'''20230411 resize png to 50% and place in folder ./resize'''
import os

def run(c):
    print(c)
    return os.system(c)

run('mkdir -p resize')
run('mogrify -resize 50% -quality 100 -path ./resize *.png')
