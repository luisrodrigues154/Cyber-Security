#!/bin/python
data_password = "1152 1344 1056 1968 1728 816 1648 784 1584 816 1728 1520 1840 1664 784 1632 1856 1520 1728 816 1632 1856 1520 784 1760 1840 1824 816 1584 1856 784 1776 1760 528 528 2000".split(" ")

for i in range(0, len(data_password)):
    ch_int = int(data_password[i], 10)
    data_password[i] = chr(ch_int >> 4)

print("Password: {0}".format("".join(data_password)))