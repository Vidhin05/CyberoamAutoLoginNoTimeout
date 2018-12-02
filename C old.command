echo -n -e "\033]0;Cyberoam\007"
/Users/vidhin/anaconda3/envs/python2/bin/python ~/Downloads/cyberoamAutoLoginNoTimeout.py
osascript -e 'tell application "Terminal" to close (every window whose name contains "Cyberoam")' &