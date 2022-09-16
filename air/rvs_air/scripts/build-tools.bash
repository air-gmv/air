#!/bin/bash
./configure -f ../utils/gitlab-runner/.sparc_gr740_config
make tools
make pmk
make pos
make libs