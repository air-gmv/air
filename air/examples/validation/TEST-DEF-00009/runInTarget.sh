#!/bin/bash
sshpass -p '$(AIR_RUNNER_PASSWORD)' scp executable/AIRAPP.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
sshpass -p '$(AIR_RUNNER_PASSWORD)' scp runAIRAPP.sh $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
sshpass -p '$(AIR_RUNNER_PASSWORD)' ssh -tt $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP) << 'ENDSSH'
unlock.sh
board-commands.sh runAIRAPP.sh
ENDSSH

