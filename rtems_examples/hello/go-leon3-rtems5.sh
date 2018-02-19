rm -rf b-leon3/
export RTEMS_MAKEFILE_PATH=/opt/rtems/5/sparc-rtems5/leon3
make -f Makefile-5
#cp o-optimize/ethgmv.exe /home/assert/Desktop/C_DRIVE/vmshare/
#tar -zcvf l.tar.gz o-optimize/ethgmv.exe 
sshpass -p "latitudee5440" scp l.tar.gz ngmp@83.240.244.231:~/drivers

#sshpass -p "latitudee5440" scp o-optimize/ethgmv.exe ngmp@83.240.244.231:~/drivers/workingttcp.exe

sshpass -p "cgd17832201" scp b-leon3/init.exe dtms@83.240.244.237:~/examples/air5/RTEMS_APP.exe
