import subprocess


programs_ = []
shell = subprocess.Popen("find / -type f -executable 2>/dev/null", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

stdout, stderr = shell.communicate()

results = [f.decode() for f in stdout.split(b"\n") if f]

for f in results:
	shell = subprocess.Popen("{} -h", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

	stdout, stderr = shell.communicate()
	print(stdout)
	print(stderr)
	exit(0)

