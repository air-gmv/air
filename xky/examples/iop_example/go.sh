#!/bin/bash
make clean
make
sparc-rtems-objdump -d iop/p0.exe > iop/p0.txt
sparc-rtems-objdump -d sampling/p1.exe > sampling/p1.txt
sparc-rtems-objdump -d queuing/p2.exe > queuing/p2.txt
sparc-rtems-objdump -d executable/XKYAPP.exe > executable/XKYAPP.txt
sparc-rtems-nm executable/XKYAPP.exe > executable/XKYAPP.num

figlet "Copying files to remote computer..."
read -s -p "Enter password: " password
sshpass -p password scp executable/XKYAPP.exe gmvs@$(AIR_RUNNER_IP):~/executable/XKYAPP.exe
sshpass -p password scp iop/p0.txt.exe gmvs@$(AIR_RUNNER_IP):~/executable/p0.txt
sshpass -p password scp sampling/p1.txt gmvs@$(AIR_RUNNER_IP):~/executable/p1.txt
sshpass -p password scp queuing/p2.txt gmvs@$(AIR_RUNNER_IP):~/executable/p2.txt
sshpass -p password scp executable/XKYAPP.num gmvs@$(AIR_RUNNER_IP):~/executable/XKYAPP.num
figlet "Files copied"
