#!/bin/bash

# Manually including rvs object files with pmk and libs archives since the RVSLink is not triggered due to Rtems forcing the use of gcc for linking
cd $AIR/air-dev/air/install/
arm-rtems5-ar -q pmk/pmk.a $AIR/air-dev/air/rvs_air/intermediate/instr/rvs.o $AIR/air-dev/air/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/imaspex/imaspex.a $AIR/air-dev/air/rvs_air/intermediate/instr/rvs.o $AIR/air-dev/air/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libair/libair.a $AIR/air-dev/air/rvs_air/intermediate/instr/rvs.o $AIR/air-dev/air/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libcov/libcov.a $AIR/air-dev/air/rvs_air/intermediate/instr/rvs.o $AIR/air-dev/air/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libiop/libiop.a $AIR/air-dev/air/rvs_air/intermediate/instr/rvs.o $AIR/air-dev/air/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libprintf/libprintf.a $AIR/air-dev/air/rvs_air/intermediate/instr/rvs.o $AIR/air-dev/air/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libtest/libtest.a $AIR/air-dev/air/rvs_air/intermediate/instr/rvs.o $AIR/air-dev/air/rvs_air/intermediate/instr/rvs_map.o

cd $AIR/examples/hello_world
configure
make

cd $AIR/examples/private-example/private/validation/

cd TEST-DEF-00009
configure
make
cd ../TEST-DEF-00010
configure
make
cd ../TEST-DEF-00012
configure
make
cd ../TEST-DEF-00021
configure
make
cd ../TEST-DEF-00022
configure
make
cd ../TEST-DEF-00023
configure
make
cd ../TEST-DEF-00500
configure
make
cd ../TEST-DEF-00510
configure
make
cd ../TEST-DEF-00540
configure
make
cd ../TEST-DEF-00560
configure
make
cd ../TEST-DEF-00600
configure
make
cd ../TEST-DEF-01390
configure
make
cd ../TEST-DEF-01650 
configure
make
cd ../TEST-DEF-01730 
configure
make
cd ../TEST-DEF-01740
configure
make
cd ../TEST-DEF-01750
configure
make
cd ../TEST-DEF-01741
configure
make
cd ../TEST-DEF-02100
configure
make
cd ../TEST-DEF-02101
configure
make
cd ../TEST-DEF-02102
configure
make
cd ../TEST-DEF-02105
configure
make
cd ../TEST-DEF-02107
configure
make
cd ../TEST-DEF-02108
configure
make
cd ../TEST-DEF-80060 
configure
make
