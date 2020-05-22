#!/bin/bash
configure
make clean && make
./testScp.bash
script -c "sshpass -p $PASSWORD1 ssh -tt $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP) 'bash -s' < target_cmds.bash" testresult.txt 
./testcheck.py

