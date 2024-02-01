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
import os 

if os.stat("testresult.txt").st_size == 0 : # empty file
    print("Test text dump file is empty, failing test")
    sys.exit(-1)

with open("testresult.txt",'r') as f:
    text = f.read()

# False by default
test_ok = False

# Exit if test suceeds and prints PASS 
for item in text.split("\n"):
    if "END OF TEST ALL PASS" in item:
        sys.exit(0)  

# Error if it was not printed
sys.exit(-1)

    
