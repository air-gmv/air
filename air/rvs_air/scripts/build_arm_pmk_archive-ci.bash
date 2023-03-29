#!/bin/bash

# Manually including rvs object files with pmk and libs archives since the RVSLink is not triggered due to Rtems forcing the use of gcc for linking
cd $AIR/install/
arm-rtems5-ar -q pmk/pmk.a $AIR/rvs_air/intermediate/instr/rvs.o $AIR/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/imaspex/imaspex.a $AIR/rvs_air/intermediate/instr/rvs.o $AIR/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libair/libair.a $AIR/rvs_air/intermediate/instr/rvs.o $AIR/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libcov/libcov.a $AIR/rvs_air/intermediate/instr/rvs.o $AIR/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libiop/libiop.a $AIR/rvs_air/intermediate/instr/rvs.o $AIR/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libprintf/libprintf.a $AIR/rvs_air/intermediate/instr/rvs.o $AIR/rvs_air/intermediate/instr/rvs_map.o
arm-rtems5-ar -q libs/libtest/libtest.a $AIR/rvs_air/intermediate/instr/rvs.o $AIR/rvs_air/intermediate/instr/rvs_map.o
