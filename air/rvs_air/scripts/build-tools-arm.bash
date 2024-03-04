#!/bin/bash
./configure -f ../utils/gitlab-runner/.arm_arty7z_config
make tools
make pmk
make pos
make libs