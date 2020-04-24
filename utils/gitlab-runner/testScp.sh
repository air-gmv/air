#!/bin/bash
cd executable/
sshpass -p 'cgd17832201' scp AIRAPP.exe dtms@83.240.244.237:~
cd ..
sshpass -p 'cgd17832201' scp runAIRAPP.sh dtms@83.240.244.237:~
sshpass -p 'cgd17832201' ssh dtms@83.240.244.237 'ls'

