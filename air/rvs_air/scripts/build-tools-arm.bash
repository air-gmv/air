#!/bin/bash
./configure -f ../utils/gitlab-runner/.arm_qemu_config
make tools
make pmk
make pos
make libs