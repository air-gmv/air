#!/usr/bin/env python
#
# Copyright (C) 2008-2019  GMVIS Skysoft S.A.
#
# The license and distribution terms for this file may be
# found in the file LICENSE in this distribution or at
# air/LICENSE
#
# ===================================================================
# A simple upd client sending integer values
# The value is given as command line parameter
# In case the value is not an integer 
# an exception is thrown
# ===================================================================
# The IP Address and the udp port are hard coded
# Adapt the code to your configuration
# ===================================================================

import socket
import time 
import sys
#PC_ADDRESS = '10.0.0.1'
PC_ADDRESS='192.168.0.3'
RECV_PORT = 13000

#SOURCE#
sensor_in = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    sensor_in.bind((PC_ADDRESS, RECV_PORT))
except socket.error, err:
    print "Couldn't be a udp server on port %d : %s" % (
            RECV_PORT, err)
    raise SystemExit

sleep = 2;

while True:

	#datagram = sensor_in.recv(10)
	datagram = sensor_in.recvfrom(100)
	if not datagram:
		break
	print datagram
	time.sleep(1)



