#!/bin/bash
cd executable/
sshpass -p $PASSWORD1 scp AIRAPP.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
cd ..
sshpass -p $PASSWORD1 scp runAIRAPP.sh $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
sshpass -p $PASSWORD1 ssh $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP) 'ls'

