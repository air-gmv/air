#!/bin/bash
./testScp.sh
script -c "sshpass -p '$(AIR_RUNNER_PASSWORD)' ssh -tt $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP) 'bash -s' < target_cmds.bash" testresult.txt 


