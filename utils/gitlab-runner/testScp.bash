#!/bin/bash
cd executable/
sshpass -p $PASSWORD2 scp AIRAPP.exe gitlabrunner@$KNOWLI:~
cd ..
sshpass -p $PASSWORD2 scp runAIRAPP.bash gitlabrunner@$KNOWLI:~
sshpass -p $PASSWORD2 scp doTest.bash gitlabrunner@$KNOWLI:~
sshpass -p $PASSWORD2 scp killgrmon.bash gitlabrunner@$KNOWLI:~
sshpass -p $PASSWORD2 ssh gitlabrunner@$KNOWLI 'ls'

