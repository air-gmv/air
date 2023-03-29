#!/bin/bash

PC=$(uname -n)


function write() {
	
	if test $PC != "$1" ; then
			rsync -azvrhq --stats $STATE_FOLDER gitlab-runner@$2:~/state/
			echo "Published files to main server."
	fi

}

if ! test -e /tmp/rsync-write-"$CI_COMMIT_SHORT_SHA".lock ; then
	touch /tmp/rsync-write-"$CI_COMMIT_SHORT_SHA".lock
else 
	echo "Waiting for another job to finish writing to main server..."
	while test -e /tmp/rsync-write-"$CI_COMMIT_SHORT_SHA".lock
	do
		sleep 1
	done
	touch /tmp/rsync-write-"$CI_COMMIT_SHORT_SHA".lock
	echo "Writing to main server..."
fi

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

rm /tmp/rsync-write-"$CI_COMMIT_SHORT_SHA".lock

exit 0
