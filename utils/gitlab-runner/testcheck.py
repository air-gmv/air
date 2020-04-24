#!/usr/bin/python
import sys

with open("testresult.txt",'r') as f:
    text = f.read()
    # f.close()

words = "TEST ALL PASS"
test_ok = False

for item in text.split("\n"):
  if words in item:
    test_ok = True

if(test_ok == False):
    sys.stderr.write("FAILURE DETECTED")
