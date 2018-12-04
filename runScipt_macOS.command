# This is a double click to run and auto-close terminal window on execution finish script for macs
echo -n -e "\033]0;Cyberoam\007"
python "Path to script"
osascript -e 'tell application "Terminal" to close (every window whose name contains "Cyberoam")' &