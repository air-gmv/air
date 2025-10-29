#!/bin/bash

EXAMPLE=$1
wall << EFO
GitLab CI/CD: SPARC test $EXAMPLE will try to run on board GR740
EFO

ps -uax | grep grmon
pkill -f "/opt/grmon-pro-2.0.92/bin/grmon -u -nb -eth 192.168.0.30 -gdb"

nohup /opt/grmon-pro-2.0.92/bin/grmon -u -nb -eth 192.168.0.30 -gdb &

sleep 5

/opt/rtems5/bin/sparc-rtems5-gdb AIRAPP.exe -x gdb_sparc_rvs_coverage-ci > testresult.txt 2>&1

pkill -f "/opt/grmon-pro-2.0.92/bin/grmon -u -nb -eth 192.168.0.30 -gdb"

wall << EFO
GitLab CI/CD: SPARC test $EXAMPLE has finished executing
EFO

exit
