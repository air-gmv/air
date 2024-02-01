#!/bin/bash

PC=$(uname -n)


function write() {
	if test $PC != "$1" ; then
		rsync -azvrh . gitlab-runner@$2:$($pwd/memdump.bin)
		echo "Published files to main server."
	fi

}

case $CI_RUNNER_TAGS in
	*SPARC* )
		write "TASTE-VM-9-1-beta2-64bit-1" $(AIR_SPARC_1_IP)
		;;
	*ARM* )
		write "AIR-ARM-VM-debian-64-1" $(AIR_ARM_1_IP)
		;;
	*OTHER* )
		write "TASTE-VM-9-1-beta2-64bit-2" $(AIR_SPARC_2_IP)
		;;
	*)
		echo "Invalid hostname $PC"
		exit 1
		;;
esac


exit 0
