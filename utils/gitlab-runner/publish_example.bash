#!/bin/bash

PC=$(uname -n)


function write() {
	
	if test $PC != "$1" ; then
		rsync -azvrh . gitlab-runner@$2:$(pwd)
		echo "Published files to main server."
	fi

}

case $CI_RUNNER_TAGS in
	*SPARC* )
		write "TASTE-VM-9-1-beta2-64bit-1" 172.19.13.167
		;;
	*ARM* )
		write "AIR-ARM-VM-debian-64-1" 172.19.13.70
		;;
	*OTHER* )
		write "TASTE-VM-9-1-beta2-64bit-2" 172.19.13.189
		;;
	*)
		echo "Invalid hostname $PC"
		exit 1
		;;
esac


exit 0