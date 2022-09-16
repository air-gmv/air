#!/bin/bash
make -C examples/hello_world distclean
make -C examples/bare_c distclean
make -C examples/hm distclean
make -C examples/math distclean
make -C examples/ports distclean
make -C examples/shm distclean
make -C examples/testsuites/paranoia distclean
make -C examples/testsuites/smp01 distclean
make -C examples/testsuites/smp02 distclean
make -C examples/private-example/private/validation/TEST-DEF-00009 distclean
make -C examples/private-example/private/validation/TEST-DEF-00010 distclean
make -C examples/private-example/private/validation/TEST-DEF-00012 distclean
make -C examples/private-example/private/validation/TEST-DEF-00021 distclean
make -C examples/private-example/private/validation/TEST-DEF-00022 distclean
make -C examples/private-example/private/validation/TEST-DEF-00023 distclean
make -C examples/private-example/private/validation/TEST-DEF-00500 distclean
make -C examples/private-example/private/validation/TEST-DEF-00510 distclean
make -C examples/private-example/private/validation/TEST-DEF-00540 distclean
make -C examples/private-example/private/validation/TEST-DEF-00560 distclean
make -C examples/private-example/private/validation/TEST-DEF-00600 distclean
make -C examples/private-example/private/validation/TEST-DEF-01390 distclean
make -C examples/private-example/private/validation/TEST-DEF-01561 distclean
make -C examples/private-example/private/validation/TEST-DEF-01570 distclean
make -C examples/private-example/private/validation/TEST-DEF-01590 distclean
make -C examples/private-example/private/validation/TEST-DEF-01610 distclean
make -C examples/private-example/private/validation/TEST-DEF-01650 distclean
make -C examples/private-example/private/validation/TEST-DEF-01730 distclean
make -C examples/private-example/private/validation/TEST-DEF-01740 distclean
make -C examples/private-example/private/validation/TEST-DEF-01750 distclean
make -C examples/private-example/private/validation/TEST-DEF-01741 distclean
make -C examples/private-example/private/validation/TEST-DEF-02100 distclean
make -C examples/private-example/private/validation/TEST-DEF-02101 distclean
# make -C examples/private-example/private/validation/TEST-DEF-02102 distclean
# make -C examples/private-example/private/validation/TEST-DEF-02105 distclean
# make -C examples/private-example/private/validation/TEST-DEF-02107 distclean
# make -C examples/private-example/private/validation/TEST-DEF-02108 distclean
make -C examples/private-example/private/validation/TEST-DEF-80000 distclean
make -C examples/private-example/private/validation/TEST-DEF-80010 distclean
make -C examples/private-example/private/validation/TEST-DEF-80060 distclean
make -C examples/private-example/private/validation/TEST-DEF-80080 distclean
make -C examples/private-example/private/validation/TEST-DEF-80300 distclean
make -C examples/private-example/private/validation/TEST-DEF-80400 distclean
make -C examples/private-example/private/validation/TEST-DEF-80401 distclean
make -C examples/private-example/private/validation/TEST-DEF-80420 distclean
make -C pmk clean
make -C pos clean
make -C libs clean
find . -name \memdump.bin -type f -delete
