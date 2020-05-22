#!/bin/bash
./testScp.sh
script -c "sshpass -p $PASSWORD1 ssh -tt $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP) 'bash -s' < target_cmds.bash" testresult.txt 


