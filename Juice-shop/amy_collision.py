#!/bin/python

import hashlib

def digest_and_compare(test, comp):
    print("Trying: {}".format(test), end="")
    digest = hashlib.md5(test.encode()).hexdigest()
    if(digest == comp):
        print("\t[Collision FOUND]")
        return True
    print("\t[FAILED]")
    return False
    

if __name__ == "__main__":
    stored_hash = "030f05e45e30710c3ad3c32f00de0473"
    padding = "....................."
    for A in range(ord('A'), ord('Z') + 1 ):
        for B in range(0, 10):
            for C in range(ord('a'), ord('z') +1):
                test_str = "{}{}{}{}".format(chr(A), B, chr(C), padding)
                if(digest_and_compare(test_str, stored_hash)):
                    exit()            