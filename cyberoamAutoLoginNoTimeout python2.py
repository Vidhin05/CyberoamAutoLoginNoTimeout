"""
author:Vidhin
A cyberoam client emulator that automatically logs in after every 2 hours.
Enter your username and password before running the script.
If you're from a different institute, change the URL to your institute's Cyberoam URL.
Press ctrl-c to logout.
"""
# This code runs in python2 only

import cookielib
import re
import signal
import ssl
import sys
import time
import urllib

import mechanize
from bs4 import BeautifulSoup

# Enter your login credentials here.
username = "your_username"
password = "your_password"


# logout once an interrupt is received.
def signal_handler(signal, frame):
    logout()
    sys.exit(0)


# DA-IICT's Cyberoam portal
url = "https://10.100.56.55:8090/httpclient.html"


def browser():
    br = mechanize.Browser()
    br.set_handle_robots(False)
    br.addheaders = [('user-agent', 'Chrome')]

    # Cookie Jar
    cj = cookielib.LWPCookieJar()
    br.set_cookiejar(cj)

    # Browser options
    br.set_handle_equiv(True)
    br.set_handle_gzip(True)
    br.set_handle_redirect(True)
    br.set_handle_referer(True)
    return br


def post_request(username, password, value, mode):
    br = browser()
    # POST request values
    values = {
        "mode": mode,
        "username": username,
        "password": password,
        "btnSubmit": value
    }

    data = urllib.urlencode(values)
    page = br.open(url, data)
    response = page.read()
    br._factory.is_html = True
    soup = BeautifulSoup(response, "lxml")

    regex = re.compile(r"<message><!\[CDATA\[(.*)\]\]><\/message>")

    x = re.search(regex, response)
    print username + ": " + x.group(1)
    return br


def login():
    br = post_request(username, password, "Login", "191")
    return br


def logout():
    br = post_request(username, password, "Logout", "193")
    return br


def bypass_ssl():
    # Bypassing SSL certification #
    try:
        _create_unverified_https_context = ssl._create_unverified_context
    except AttributeError:
        # Legacy Python that doesn't verify HTTPS certificates by default
        pass
    else:
        # Handle target environment that doesn't support HTTPS verification
        ssl._create_default_https_context = _create_unverified_https_context
    return


def main():
    bypass_ssl()
    signal.signal(signal.SIGINT, signal_handler)

    while True:
        login()
        time.sleep(7100)


if __name__ == '__main__':
    main()
