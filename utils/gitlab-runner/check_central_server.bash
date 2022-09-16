#!/bin/bash

PC=$(uname -n)

function write() {
	
	if test $PC = "$1" && ! test -e $STATE_FOLDER; then
		rsync -avzrh . $STATE_FOLDER/
	else
		if $(ssh gitlab-runner@$2 -- "test -d $STATE_FOLDER") ; then
			rsync -avzrh gitlab-runner@$2:$STATE_FOLDER ~/state/
			echo "Fetched files from main server."
		else
			rsync -azvrh . $STATE_FOLDER/
			echo "Copied files locally."
		fi
	fi

}

if ! test -e /tmp/rsync-"$CI_COMMIT_SHORT_SHA".lock ; then
	touch /tmp/rsync-"$CI_COMMIT_SHORT_SHA".lock
else 
	echo "Waiting for another job to finish rsync..."
	while test -e /tmp/rsync-"$CI_COMMIT_SHORT_SHA".lock 
		do
			sleep 1
		done
	echo "Another job fetched the files for me."
	exit 0
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
		echo "Invalid runner tags $CI_RUNNER_TAGS"
		exit 1
		;;
esac

rm /tmp/rsync-"$CI_COMMIT_SHORT_SHA".lock 

exit 0
