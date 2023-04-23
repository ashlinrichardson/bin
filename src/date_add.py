import os
import sys
import datetime
args = sys.argv

def err(m):
    print("Error:", m); sys.exit(1)

if len(args) < 3:
    err("add_date [date yyyymmdd / yyyymmddhhmmss] [number of days to add]")

def parse(s):
    if not (len(s) == 8 or len(s) == 14):
        err("date expected in yyyymmdd or yyyymmddhhmmss format")
    
    year, month, day = [int(x) for x in [s[:4], s[4:6], s[6:8]]]
    hour, minute, second = None, None, None
    
    def c(s, a, b):
        try:
            return int(s[a:b])
        except:
            return None
    
    hr = c(s, 8, 10)
    mn = c(s,10,12)
    sc = c(s, 12, 14)
    
    # print(year, month, day, hr, mn, sc)
    if hr == None or mn == None or sc == None:
        return datetime.datetime(year, month, day)
    else:
        return datetime.datetime(year=year,
                                 month=month,
                                 day=day,
                                 hour=hr,
                                 minute=mn,
                                 second=sc)
d1 = parse(args[1])
print(d1 + datetime.timedelta(days=int(args[2])))
