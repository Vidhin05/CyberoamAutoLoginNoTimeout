import requests
from bs4 import BeautifulSoup
from requests.packages.urllib3.exceptions import InsecureRequestWarning
requests.packages.urllib3.disable_warnings(InsecureRequestWarning)
user = input('enter username\n')
passcode = input('enter passcode\n')
payload = {
    'mode': '191',
    'username': user,
    'password': passcode
}

with requests.Session() as s:
    p = s.post('https://10.100.56.55:8090/httpclient.html', data=payload, verify=False)
    soup = BeautifulSoup(p.text, "xml")
    check = 'You have successfully logged in'
    for i in soup.find_all('message'):
            result = i.text
            print(result)
            if check == result:
                cmd = input("Do you wanna log off ? (0/1)\n")
                if cmd == '0':
                    payload = {'mode': 193, 'username':user }
                    with requests.Session() as n:
                        j = n.post('https://10.100.56.55:8090/httpclient.html', data=payload, verify=False)
                        soup = BeautifulSoup(j.text, "xml")
                        for i in soup.find_all('message'):
                            result = i.text
                            print(result)
                else:
                    break
