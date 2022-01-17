#!/bin/bash
wall << EFO
The Automatic Test of gitlab will try run test on board N2X
EFO

./doTest.bash &
./host_test.py &
wait
