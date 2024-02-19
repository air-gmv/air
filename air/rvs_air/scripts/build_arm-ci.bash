#!/bin/bash
./configure -f ../utils/gitlab-runner/.arm_arty7z_config
make pmk
make libs
make pos
make tools

cd $AIR/examples/hello_world
configure
make
