#!/usr/bin/env python
#
# Copyright (C) 2008-2021  GMVIS Skysoft S.A.
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
#BOARD_ADDRESS = '10.0.0.51'
BOARD_ADDRESS= '192.168.0.17'
SENSOR_OUT=14000

PC_ADDRESS='192.168.0.3'
RECV_PORT = 13000

#SOURCE#
sensor_out = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sensor_in = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sensor_in.settimeout(5)

try:
    sensor_in.bind((PC_ADDRESS, RECV_PORT))
except socket.error, err:
    print "Couldn't be a udp server on port %d: %s" % (
            RECV_PORT, err)
    raise SystemExit

received = 0

for numero in range (10):

	try:
		datagram = sensor_in.recvfrom(100)
	except socket.error, err:
		print "error trying to receive on port %d: %s" % (
			SENSOR_OUT, err)
	else:
		received =+ 1
		print "Host received:"
		print datagram

	sensor_out.sendto('R'+str(numero), (BOARD_ADDRESS, SENSOR_OUT))
	print 'Host sending: ' +'R'+str(numero)	
	time.sleep(0.5)

if received == 0:
	print "No messages received by host. FAILED"