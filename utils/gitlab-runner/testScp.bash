#!/bin/bash
cd executable/
sshpass -p $PASSWORD1 scp AIRAPP.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
cd ..
sshpass -p $PASSWORD1 scp runAIRAPP.bash $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
sshpass -p $PASSWORD1 scp doTest.bash $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
sshpass -p $PASSWORD1 scp killgrmon.bash $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~
sshpass -p $PASSWORD1 ssh $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP) 'ls'

