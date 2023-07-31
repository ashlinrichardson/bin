''' 20230525 expunge a file / file pattern, from a repo'''
import os
import sys
def run(c):
	print(c)
	return os.system(c)
args = sys.argv

if len(args) < 2:
	print('Error: usage:\npython3 git_expunge.py [pattern] # pattern could be: file_to_delete.extension, may include wildcards')

run("git filter-branch --index-filter 'git rm -rf --cached --ignore-unmatch " + pattern + "' HEAD")
print('to confirm: run this:')
print('  git push --force  origin master')
