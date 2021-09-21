#!/bin/bash
ps -uax | grep grmon
lockfile -s 5 -r 5 /var/lock/gr740.grmonlock || exit 1
rm -f /var/lock/gr740.grmonlock
./killgrmon.bash &
board-commands.sh runAIRAPP.bash
ls
killall sleep
killall -9 grmon
killall killgrmon.bash
exit
