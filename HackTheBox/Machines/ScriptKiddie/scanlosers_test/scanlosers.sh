#!/bin/bash

log=hackers

cat $log | cut -d' ' -f3- | sort -u | while read ip; do
	echo ${ip};
	sh -c "echo abc ${ip} 2>&1 >/dev/null" & 
done

if [[ $(wc -l < $log) -gt 0 ]]; then echo -n > $log; fi
