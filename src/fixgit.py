# if git asks for credentials this is how I might fix it
import os
import sys

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
    cmd = var + '=' + eval(var)
    print(cmd)

git_user, user_name, user_email = None, None, None
for line in lines:
    if len(line.split('remote.origin.url')) > 1:
        git_user = line.split("git@github.com:")[1].split("/")[0]
    if len(line.split('email=')) > 1:
        user_email = line.split("=")[1].strip()
    if len(line.split('name=')) > 1:
        user_name = line.split("=")[1].strip()

pr("git_user")
pr("user_name")
pr("user_email") 

cd = os.getcwd()
w = cd.split(os.path.sep)

def run(x):
    print(x)

run('git config user.email ' + user_email)
run('git config user.name "' + user_name + '"')
run('git remote set-url origin git@github.com:' + git_user + '/' + w[-1] + '.git')

