rm -rf b-leon3/
export RTEMS_MAKEFILE_PATH=/opt/rtems/5/sparc-rtems5/leon3
make -f Makefile-5
#cp o-optimize/ethgmv.exe /home/assert/Desktop/C_DRIVE/vmshare/
#tar -zcvf l.tar.gz o-optimize/ethgmv.exe 
sshpass -p "$(AIR_RUNNER_PASSWORD)" scp l.tar.gz $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/drivers

#sshpass -p "$(AIR_RUNNER_PASSWORD)" scp o-optimize/ethgmv.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/drivers/workingttcp.exe

sshpass -p "$(AIR_RUNNER_PASSWORD)" scp b-leon3/init.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/examples/air5/RTEMS_APP.exe
