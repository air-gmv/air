#!/bin/bash
./testScp.sh
script -c "sshpass -p 'cgd17832201' ssh -tt dtms@83.240.244.237 'bash -s' < target_cmds.bash" testresult.txt 


