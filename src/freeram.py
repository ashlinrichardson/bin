'''20220413 free ram on ubuntu'''
import os

def run(c):
    print(c)
    return os.system(c)

run('free -m')
run('sudo sync')
run('sudo sysctl -w vm.drop_caches=3')
run('free -m')
