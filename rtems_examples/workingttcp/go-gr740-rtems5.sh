rm -rf b-gr740/
export RTEMS_MAKEFILE_PATH=/opt/rtems/5/sparc-rtems5/gr740
export RTEMS_API=5
export RTEMS_BSP=gr740
make -f Makefile-5
#cp o-optimize/ethgmv.exe /home/assert/Desktop/C_DRIVE/vmshare/
#tar -zcvf l.tar.gz o-optimize/ethgmv.exe 
#sshpass -p "latitudee5440" scp l.tar.gz ngmp@$(AIR_RUNNER_IP):~/drivers

#sshpass -p "latitudee5440" scp o-optimize/ethgmv.exe ngmp@$(AIR_RUNNER_IP):~/drivers/workingttcp.exe

