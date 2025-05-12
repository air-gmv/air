#!/bin/bash

PC=$(uname -n)

function write() {
	if test $PC = "$1" && ! test -e $STATE_FOLDER; then
		rsync -avzrhq --stats . $STATE_FOLDER/
		echo "Copied files locally."
	else
		if $(ssh gitlab-runner@$2 -- "test -d $STATE_FOLDER") ; then
			rsync -avzrhq gitlab-runner@$2:$STATE_FOLDER ~/state/
			echo "Fetched files from main server."
		else
			rsync -azvrhq --stats . $STATE_FOLDER/
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
	*SPARC* | *sparc* )
		write "TASTE-VM-9-1-beta2-64bit-1" $(AIR_SPARC_1_IP)
		;;
	*ARM* )
		write "AIR-ARM-VM-debian-64-1" $(AIR_ARM_1_IP)
		;;
	*OTHER* )
		write "TASTE-VM-9-1-beta2-64bit-2" $(AIR_SPARC_2_IP)
		;;
	*)
		echo "Invalid runner tags $CI_RUNNER_TAGS"
		exit 1
		;;
esac

rm /tmp/rsync-"$CI_COMMIT_SHORT_SHA".lock 

exit 0
