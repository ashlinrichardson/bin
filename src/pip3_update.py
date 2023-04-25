# from https://stackoverflow.com/questions/2720014/how-to-upgrade-all-python-packages-with-pip
import os
def run(c):
    print(c)
    return os.system(c)

run('pip3 freeze > pip3_requirements.txt')

# Open the text file, replace the == with >=, or have sed do it for you:

run("sed -i 's/==/>=/g' pip3_requirements.txt")

run("pip install -r pip3_requirements.txt --upgrade")

run("rm pip3_requirements.txt")
