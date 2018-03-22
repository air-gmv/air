rm -rf b-gr740/
export RTEMS_MAKEFILE_PATH=/opt/rtems/5/sparc-rtems5/gr712rc
export RTEMS_BSP=gr740
make -f Makefile-5
#cp o-optimize/ethgmv.exe /home/assert/Desktop/C_DRIVE/vmshare/
#tar -zcvf l.tar.gz o-optimize/ethgmv.exe 
#sshpass -p "latitudee5440" scp l.tar.gz ngmp@83.240.244.231:~/drivers

#sshpass -p "latitudee5440" scp o-optimize/ethgmv.exe ngmp@83.240.244.231:~/drivers/workingttcp.exe

