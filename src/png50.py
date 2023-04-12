'''20230411 resize png to 50% and place in folder ./resize'''
import os
import multiprocessing as mp

def parfor(my_function,  # function to run in parallel
           my_inputs,  # inputs evaluated by worker pool
           n_thread=mp.cpu_count()): # cpu threads to use

    if n_thread == 1:  # don't use multiprocessing for 1-thread
        return [my_function(my_inputs[i]) for i in range(len(my_inputs))]
    else:
        n_thread = (mp.cpu_count() if n_thread is None else n_thread)
        return mp.Pool(n_thread).map(my_function, my_inputs)

def run(c):
    print(c)
    return os.system(c)

lines = [x.strip() \
         for x in os.popen('ls -1 *.png').readlines()]

d = 'resize50'
run('mkdir -p ' + d)

c = ['mogrify -resize 50% -quality 100 -path ' + d + ' ' + x \
     for x in lines]

parfor(run, c)
