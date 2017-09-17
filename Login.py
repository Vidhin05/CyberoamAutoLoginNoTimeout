import requests
from bs4 import BeautifulSoup
from requests.packages.urllib3.exceptions import InsecureRequestWarning
requests.packages.urllib3.disable_warnings(InsecureRequestWarning)


def login(username, password):

    payload = {
        'mode': '191',
        'username': username,
        'password': password
    }

    with requests.Session() as s:
        p = s.post('https://10.100.56.55:8090/httpclient.html', data=payload, verify=False)
        soup = BeautifulSoup(p.text, "xml")
        check = 'You have successfully logged in'
        for i in soup.find_all('message'):
                result = i.text
                print(result)
    return result


def logoff(username):
        payload = {'mode': 193, 'username': username}
        with requests.Session() as n:
            j = n.post('https://10.100.56.55:8090/httpclient.html', data=payload, verify=False)
            soup = BeautifulSoup(j.text, "xml")
            for i in soup.find_all('message'):
                result = i.text
                print(result)


if __name__ == "__main__":
    user = input('Enter username\n')
    passcode = input('Enter password\n')
    result = login(user, passcode)

    if result == 'You have successfully logged in':
        cmd = input('Do you want to log off ? (0/1)\n')
        if cmd == '0':
            logoff(user)
