#!/bin/bash
wall << EFO
The Automatic Test of gitlab will try run test on board N2X
EFO

ps -uax | grep grmon
killall -9 grmon
#nohup /opt/grmon-pro-2.0.92/bin//board-commands.sh runAIRAPP.bash &
#./host_test.py

#cat nohup.out

/opt/grmon-pro-2.0.92/bin/grmon -u -nb -eth 192.168.0.30 -gdb -c runAIRAPP.bash -echo &

/opt/rtems5/bin//sparc-rtems5-gdb -x gdb_sparc_rvs_coverage-ci /home/gitlabrunner/AIRAPP.exe --cd=.

ls
killall -9 grmon
exit
