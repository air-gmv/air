#!/bin/bash

cat gdb_init_cov_arm > gdb_auto_cov_${PWD##*/}
for d in */ ; do
    if [ "$d" != executable/ ] && [ "$d" != common/ ] && [ "$d" != config/ ]
    then
        OUTDIR=$(find $d -name *.gcno -printf '%h' -quit)
        if [ ! -z "$OUTDIR" ]
        then
            echo "auto-cov $d${d%/}.exe" >> gdb_auto_cov_${PWD##*/}
        fi
    fi
done




# Launching QEMU
pkill qemu-system-arm
xterm -e "qemu-debug executable/AIRAPP.exe"  &

# Launching GDB
arm-rtems5-gdb -x gdb_auto_cov_${PWD##*/} #>/dev/null 2>&1
pkill qemu-system-arm

#Move gcda files to same location as the gcno files
echo Generated gcda files:
for a in *.gcda ; do
    echo "${a%.gcda}"
    mv $a `find . -name "${a%.gcda}.gcno" -printf '%h'`
done

printf "\n"

#generate coverage report for each partition
GCOV_PATH=$(which arm-rtems5-gcov)
for d in */ ; do
    if [ "$d" != executable/ ] && [ "$d" != common/ ] && [ "$d" != config/ ]
    then
        cd $d
        OUTDIR=$(find . -name *.gcno -printf '%h' -quit)
        if [ ! -z "$OUTDIR" ]
        then
            arm-rtems5-gcov *.c -o $OUTDIR
            mv *.gcov $OUTDIR
            lcov -c -d . -output-file $OUTDIR/${d%/}.info
            genhtml $OUTDIR/${d%/}.info --output-directory $OUTDIR/coverage_report/
        fi
        cd ..
    fi
done


rm gdb_auto_cov_${PWD##*/}

