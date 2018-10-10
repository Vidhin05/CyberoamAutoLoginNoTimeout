"""
author: Vivek Gover and Vidhin Parmar
A cyberoam client emulator that automatically logs in after every 2 hours.
Enter your username and password before running the script.
If you're from a different institute, change the URL to your institute's Cyberoam URL
Press ctrl-c to logout.
"""
# This code runs in python3 only
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


def login():
    payload = {
        'mode': '191',
        'username': username,
        'password': password
    }
    with requests.Session() as s:
        p = s.post('https://10.100.56.55:8090/httpclient.html', data=payload, verify=False)
        soup = BeautifulSoup(p.text, "xml")
        for i in soup.find_all('message'):
            response = i.text
            print(username + ": " + response)
    return response


def logoff():
    payload = {'mode': 193, 'username': username}
    with requests.Session() as n:
        j = n.post('https://10.100.56.55:8090/httpclient.html', data=payload, verify=False)
        soup = BeautifulSoup(j.text, "xml")
        for i in soup.find_all('message'):
            response = i.text
            print(username + ": " + response)


if __name__ == "__main__":
    username = input('Enter username:\n')
    password = input('Enter password:\n')
    signal.signal(signal.SIGINT, signal_handler)
    result = login()

    while result == 'You have successfully logged in':
        time.sleep(7100)
        result = login()
