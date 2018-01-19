rm -rf o-optimize-leon3/
export RTEMS_MAKEFILE_PATH=/opt/rtems/4.12/sparc-rtems5/leon3
make -f Makefile-4.12
#cp o-optimize/ethgmv.exe /home/assert/Desktop/C_DRIVE/vmshare/
#tar -zcvf l.tar.gz o-optimize/ethgmv.exe 
#sshpass -p "$(AIR_RUNNER_PASSWORD)" scp l.tar.gz $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/drivers

#sshpass -p "$(AIR_RUNNER_PASSWORD)" scp o-optimize/ethgmv.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/drivers/workingttcp.exe

