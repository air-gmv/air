#!/bin/bash
wall << EFO
The Automatic Test of gitlab will try run test on board N2X
EFO

ps -uax | grep grmon
lockfile -s 5 -r 5 /var/lock/gr740.grmonlock || exit 1
rm -f /var/lock/gr740.grmonlock
./killgrmon.bash &
board-commands.sh runAIRAPP.bash &
./host_test.py &
wait
ls
killall sleep
killall -9 grmon
killall killgrmon.bash
exit
