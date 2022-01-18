#!/bin/bash
wall << EFO
The Automatic Test of gitlab will try run test on board N2X
EFO

ps -uax | grep grmon
killall -9 grmon
nohup board-commands.sh runAIRAPP.bash &
./host_test.py

cat nohup.out

ls
killall -9 grmon
exit
