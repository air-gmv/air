#!/bin/bash

PC=$(uname -n)


function write() {
	
	if test $PC != "$1" ; then
			rsync -azvrh $STATE_FOLDER gitlab-runner@$2:~/state/
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

rm /tmp/rsync-write-"$CI_COMMIT_SHORT_SHA".lock

exit 0