#!/bin/bash
./configure -f ../utils/gitlab-runner/.arm_zynqz1_config
make tools
make pmk
make pos
make libs