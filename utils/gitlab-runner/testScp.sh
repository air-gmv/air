#!/bin/bash
cd executable/
sshpass -p '$(AIR_RUNNER_PASSWORD)' scp AIRAPP.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
cd ..
sshpass -p '$(AIR_RUNNER_PASSWORD)' scp runAIRAPP.sh $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
sshpass -p '$(AIR_RUNNER_PASSWORD)' ssh $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP) 'ls'

