#!/bin/bash
./configure -f ../utils/gitlab-runner/.arm_qemu_config
make pmk
make libs
make pos
make tools

cd $AIR/examples/hello_world
configure
make
