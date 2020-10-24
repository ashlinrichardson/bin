#!/usr/bin/env python2.7
import os
import sys
import time
import multiprocessing
from fl0w3r import error, exists, readlines
from _thread import allocate_lock, start_new_thread

args = sys.argv
if(len(args) < 2):
    error('multicore.py:\nusage: multicore [text file with one system command per line] # [optional parameter: number of cores]')

fn, ncpu = args[1], multiprocessing.cpu_count()

if len(args) > 2:
    ncpu = int(args[2])

if not exists(fn):
    error('file: ' + fn.strip() + ' not found')

task = readlines(fn)
tasks, n_task = [x.strip() for x in task], len(task)

# concurrency control 
lock, p_lock = allocate_lock(), allocate_lock()
threads_alive, next_j, j_max = ncpu, 0, n_task - 1

# printf with lock
def cprint(s):
    global p_lock
    p_lock.acquire()
    print(s)
    p_lock.release()

# worker thread takes a task from the list
def threadfun(my_id):
    global next_j, j_max, lock, threads_alive, tasks
    while True:
        # acquire lock, fetch task index
        lock.acquire()
        j, next_j = next_j, next_j + 1
        lock.release()

        # kill off a thread if no more work
        if(j > j_max):
            threads_alive -= 1
            return

        cprint(tasks[j])
        # run a task
        os.popen(tasks[j]).read()


# sleep while task being processed
def wait_to_finish():
    poll_int = .01
    while True:
        time.sleep(poll_int)
        if(threads_alive == 0):
            sys.exit(0)

for i in range(0, ncpu):
    start_new_thread(threadfun, (i, ))
wait_to_finish()
