#!/usr/bin/python
# =============================================================================
#  Copyright (C) GMVIS Skysoft S.A., 2014-2020
# =============================================================================
#   This file is part of the AIR Operating System configuration sub-system.
#   The license and distribution terms for this file may be found in the file
#   LICENSE in the root directory of the AIR OS distribution.
# =============================================================================
# author $(AIR_RUNNER_USER)

import sys

with open("testresult.txt",'r') as f:
    text = f.read()
    # f.close()

print text
error_list = ["FAILURE DETECTED", "Killed", "| FAILED, " , "giving up on"]
test_ok = True

for word in error_list:
    for item in text.split("\n"):
        if word in item:
            test_ok = False
            if (word == error_list[0]) or (word == error_list[3]):
                sys.stderr.write("Unit Test Failure\n")
            if (word == error_list[1]):
                sys.stderr.write("Test Timeout Failure\n")
            if (word == error_list[3]):
                sys.stderr.write("Test Timeout, Board was locked\n")                

if(test_ok == False):
    sys.stderr.write("FAILURE DETECTED")
else:
    print ("Test has PASSED with no errors\n")
