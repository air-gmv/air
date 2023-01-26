#!/bin/bash
./configure -f ../utils/gitlab-runner/.sparc_gr740_config
make pmk
make libs
make pos
make tools