#!/bin/bash
./configure -f ../utils/gitlab-runner/.arm_zynqz1_config
make pmk
make libs
make pos
make tools

cd $AIR/examples/hello_world
configure
make
