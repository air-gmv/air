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

def check_errors_in_file(file_path, error_list):
    try:
        with open(file_path, 'r') as file:
            content = file.read()
            for error_phrase in error_list:
                if error_phrase in content:
                    return True
            return False
    except FileNotFoundError:
        return False

# Example usage:
file_path = "testresult.txt"
error_list = ["FAILURES DETECTED", "Killed", "| FAILED" , "giving up on", "could not load", "not found", "failed","FAILED"]

if os.stat("testresult.txt").st_size == 0 : # empty file
    print("Test text dump file is empty, failing test")
    sys.exit(-1)
else : 
    if check_errors_in_file(file_path, error_list):
        sys.exit(-1)
    else:
        sys.exit(0)
