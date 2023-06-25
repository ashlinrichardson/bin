# 20230618 example of downloading file in chunks over a bad internet connection
lftp -c 'set net:idle 5
         set net:max-retries 0
         set net:reconnect-interval-base 3
         set net:reconnect-interval-max 3
         pget -n 10 -c "https://github.com/bcgov/wps-research/archive/refs/heads/master.zip"'
