#!/bin/sh -e
#
# a script file to run by jenkins each time there is a commit to test it
# 

#cd ./air
cd ./xky

pwd=`pwd`
export PATH=$PATH:/opt/rtems/5/bin:$pwd

if [ -f ./Makefile ]; then
    echo "### clean ###\n";
    make clean
fi

echo "\n\n### configure [ SPARC, LEON4, ENABLE FPU ] ###\n";
../tools/jenkins/run-configure.empty
#../tools/jenkins/run-configure.exp

sleep 2

echo "\n\n### make ###\n";
make


echo "\n\n### compile examples ###\n";
cd ./examples/

for entry in */
do
    echo "### check [ $entry ] ###"

    if [ $entry = "iop_example/" -o $entry = "math/" -o $entry = "multi/" -o $entry = "router_example/" -o $entry = "testsuites/" ]; then
        echo "### avoid [ $entry ] ###"
        continue
    fi

    cd $entry

    if [ -f ./Makefile ]; then
        echo "### clean [ $entry ] ###\n";
        make clean
    fi

    echo "\n\n### configure [ $entry ] ###\n";
    configure   

    echo "\n\n### make [ $entry ] ###\n";
    make 

    cd -
done
