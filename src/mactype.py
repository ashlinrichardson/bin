'''for ubuntu installed on a mac, find out the type of mac'''
import os
print(os.popen('sudo dmidecode | grep Mac').read())
