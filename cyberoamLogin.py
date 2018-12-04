"""
author : Vidhin Parmar
E-mail : ividhin@gmail.com
Github Username: Vidhin05
A cyberoam client emulator that automatically logs in after every 2 hours.
Enter your credentials before running the script.
If you're from a different institute, replace the URL by your institute's Cyberoam URL.
Press ctrl-c to logout.
Press ctrl-\ to exit without quitting.
"""
# This code uses python3

import signal
import sys
import time

import requests
from bs4 import BeautifulSoup
from requests.packages.urllib3.exceptions import InsecureRequestWarning

requests.packages.urllib3.disable_warnings(InsecureRequestWarning)


def signal_handler(signal, frame):
    logoff()
    sys.exit(0)


def signal_handler2(signal, frame):
    sys.exit(0)


def login():
    payload = { 'mode': '191', 'username': credentials[0], 'password': credentials[1]}
    with requests.Session() as s:
        p = s.post('https://10.100.56.55:8090/httpclient.html', data=payload, verify=False)
        soup = BeautifulSoup(p.text, "xml")
        for i in soup.find_all('message'):
            response = i.text
            print(credentials[0] + ": " + response)
    return response


def logoff():
    payload = {'mode': 193, 'username': credentials[0]}
    with requests.Session() as n:
        j = n.post('https://10.100.56.55:8090/httpclient.html', data=payload, verify=False)
        soup = BeautifulSoup(j.text, "xml")
        for i in soup.find_all('message'):
            response = i.text
            print(credentials[0] + ": " + response)


if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler1)
    signal.signal(signal.SIGQUIT, signal_handler2)
    
    credentials = ("your_username" , "your_password")
    while login() == 'You have successfully logged in':
        time.sleep(7100)
