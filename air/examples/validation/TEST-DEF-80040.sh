#!/bin/bash
set -e
for i in $(ls -d TEST-DEF*/); do
	if [ "${i%%/}" != "TEST-DEF-00045" ]; then
		python ../TOOLS/MemmapValidator/analyse.py ${i%%/}; 
	fi
done
