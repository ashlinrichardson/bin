'''20220423 fix filenames by removing brackets,
and replacing space w underscore'''
import os
def run(c):
    return [os.system(i) for i in c]

run(['filenames_remove_brackets',
     'filenames_replace_space_underscore'])
