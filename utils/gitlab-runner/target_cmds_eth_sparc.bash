#!/bin/bash
wall << EFO
The Automatic Test of gitlab will try run test on board GR740
EFO

ps -uax | grep grmon
killall -9 grmon

/opt/grmon-pro-2.0.92/bin/grmon -u -nb -eth 192.168.0.30 -gdb
/opt/rtems5/bin//sparc-rtems5-gdb AIRAPP.exe -x gdb_sparc_rvs_coverage_ci.gdb

killall -9 grmon

exit
