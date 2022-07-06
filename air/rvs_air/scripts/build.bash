#!/bin/bash
./configure -f ../utils/gitlab-runner/.sparc_gr740_config
make tools
make pmk
make pos
make libs
cd examples/hello_world
configure
make
cd ../bare_c
configure
make
cd ../hm
configure
make
cd ../math
configure
make
cd ../ports
configure
make
cd ../shm
configure
make
cd ../testsuites/paranoia
configure
make
cd ../smp01
configure
make
cd ../smp02
configure
make
cd ../../private-example/private/validation/TEST-DEF-00009
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
cd ../TEST-DEF-01561
configure
make
cd ../TEST-DEF-01570
configure
make
cd ../TEST-DEF-01590
configure
make
cd ../TEST-DEF-01610
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
# cd ../TEST-DEF-02101
# configure
# make
# cd ../TEST-DEF-02102
# configure
# make
# cd ../TEST-DEF-02105
# configure
# make
# cd ../TEST-DEF-02107
# configure
# make
# cd ../TEST-DEF-02108
# configure
# make
cd ../TEST-DEF-80000
configure
make
cd ../TEST-DEF-80010
configure
make
cd ../TEST-DEF-80060
configure
make
cd ../TEST-DEF-80080
configure
make
cd ../TEST-DEF-80300
configure
make
cd ../TEST-DEF-01750
configure
make
cd ../TEST-DEF-80400
configure
make
cd ../TEST-DEF-80401
configure
make
cd ../TEST-DEF-80420
configure
make