#!/usr/bin/env python
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
BOARD_ADDRESS= '192.168.0.70'
SENSOR_OUT=14000

#SOURCE#
sensor_out = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

#sensor_out.sendto('0000', (BOARD_ADDRESS, SENSOR_OUT))

numero=0

while True:
        numero=numero+1
	sensor_out.sendto('Queued message '+str(numero), (BOARD_ADDRESS, SENSOR_OUT))
	print 'Sending ' +'Queued message '+str(numero)	
	time.sleep(0.5)



