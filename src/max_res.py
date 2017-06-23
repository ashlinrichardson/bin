'''get max res, according to mac os system profiler '''
import os
print os.popen("system_profiler SPDisplaysDataType | grep Resolution").read()
