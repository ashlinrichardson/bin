# 20230602 note: run this from inside an existing repo's folder?
# if git asks for credentials this is how I might fix it
import os
import sys

print("START")

def incr(c): return chr(ord(c) + 1)
def decr(c): return chr(ord(c) - 1)

def enc(s):
    r = list(s)
    for i in range(0, len(r)): r[i] = incr(r[i])
    return "".join(r)

def dec(s):
    r = list(s)
    for i in range(0, len(r)): r[i] = decr(r[i])
    return "".join(r)

user = os.popen('whoami').read().strip()
lines = [x.strip() for x in os.popen("cd /home/" + user + dec("0HjuIvc0cjo0") + "; git config --list").readlines()]

def pr(var): # print out a variable
    try:
        cmd = var + '=' + eval(var)
        print(cmd)
    except:
        print("Variable:", var, ":not defined")

'''
user.email=MY_EMAIL_ADDRESS
user.name=MY_NAME
core.repositoryformatversion=0
core.filemode=true
core.bare=false
core.logallrefupdates=true
remote.origin.url=https://github.com/MY_NAME/MY_REPOSITORY.git
remote.origin.fetch=+refs/heads/*:refs/remotes/origin/*
branch.master.remote=origin
branch.master.merge=refs/heads/master
'''
git_user, user_name, user_email = None, None, None
for line in lines:
    w = line.split('=')
    print([line])
    if w[0] == 'remote.origin.url':
        git_user = w[1].split('/')[-2]
    if w[0] == 'user.email':
        user_email = line.split("=")[1].strip()
    if w[0] == 'user.name':
        print([line])
        user_name = line.split("=")[1].strip()
pr("git_user")
pr("user_name")
pr("user_email") 

cd = os.getcwd()
w = cd.split(os.path.sep)

def run(x):
    print('system:', [x])
    a = os.system(x)

run('git config pull.rebase false')
run('git config user.email ' + user_email)
run('git config user.name "' + user_name + '"')
run('git remote set-url origin git@github.com:' + git_user + '/' + w[-1] + '.git')


'''
hint:   git config pull.rebase false  # merge (the default strategy)
hint:   git config pull.rebase true   # rebase
hint:   git config pull.ff only       # fast-forward only
'''

