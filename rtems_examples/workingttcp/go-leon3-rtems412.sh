rm -rf o-optimize-leon3/
export RTEMS_MAKEFILE_PATH=/opt/rtems/4.12/sparc-rtems5/leon3
make -f Makefile-4.12
#cp o-optimize/ethgmv.exe /home/assert/Desktop/C_DRIVE/vmshare/
#tar -zcvf l.tar.gz o-optimize/ethgmv.exe 
#sshpass -p "latitudee5440" scp l.tar.gz ngmp@$(AIR_RUNNER_IP):~/drivers

#sshpass -p "latitudee5440" scp o-optimize/ethgmv.exe ngmp@$(AIR_RUNNER_IP):~/drivers/workingttcp.exe

