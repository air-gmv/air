#!/bin/bash
echo RTEMS5 test starting
configure
make clean && make
./testScp.bash
script -c "sshpass -p $PASSWORD1 ssh -tt dtms@83.240.244.237 'bash -s' < target_cmds.bash" testresult.txt 
cmd="./testcheck.py"
$cmd
status=$?
if [ $status -eq 0 ]; then
    echo TEST RTEMS5 OK
else
    echo TEST RTEMS5 FAILED EXITING
    exit 1
fi
echo RTEMS5 test ended

#echo RTEMS 48i test starting
#./rtems48i.bash
#configure
#make clean && make
#./testScp.bash
#script -c "sshpass -p $PASSWORD1 ssh -tt dtms@83.240.244.237 'bash -s' < target_cmds.bash" testresult.txt 
#$cmd
#status=$?
#if [ $status -eq 0 ]; then
#    echo TEST RTEMS48I BASH OK
#else
#    echo TEST RTEMS48I FAILED EXITING
#    exit 1
#fi
#echo RTEMS 48i test ended


