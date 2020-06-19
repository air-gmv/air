#!/bin/bash
ps -uax | grep grmon
lockfile -s 5 -r 5 /var/lock/n2x.grmonlock || exit 1
rm -f /var/lock/n2x.grmonlock
./killgrmon.bash &
board-commands.sh runAIRAPP.bash
ls
killall sleep
killall -9 grmon
killall killgrmon.bash
exit
