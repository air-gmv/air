#!/bin/bash
sshpass -p 'cgd17832201' scp executable/AIRAPP.exe dtms@83.240.244.237:~
sshpass -p 'cgd17832201' scp runAIRAPP.sh dtms@83.240.244.237:~
sshpass -p 'cgd17832201' ssh -tt dtms@83.240.244.237 << 'ENDSSH'
unlock.sh
board-commands.sh runAIRAPP.sh
ENDSSH

