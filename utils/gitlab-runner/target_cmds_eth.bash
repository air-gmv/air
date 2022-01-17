#!/bin/bash
wall << EFO
The Automatic Test of gitlab will try run test on board N2X
EFO

nohup ./doTest.bash &
./host_test.py

cat nohup.out
