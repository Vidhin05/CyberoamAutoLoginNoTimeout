echo -n -e "\033]0;Cyberoam\007"
python ~/Downloads/cyberoamAutoLoginNoTimeout/cyberoamAutoLoginNoTimeout.py
osascript -e 'tell application "Terminal" to close (every window whose name contains "Cyberoam")' &