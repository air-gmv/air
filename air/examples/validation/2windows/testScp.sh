#!/bin/bash
cd executable/
sshpass -p $PASSWORD1 scp AIRAPP.exe dtms@83.240.244.237:~
cd ..
sshpass -p $PASSWORD1 scp runAIRAPP.sh dtms@83.240.244.237:~
sshpass -p $PASSWORD1 ssh dtms@83.240.244.237 'ls'

