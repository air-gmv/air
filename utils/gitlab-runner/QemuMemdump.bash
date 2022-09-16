#!/bin/bash
expect << EOF
spawn telnet localhost 4444
expect -re ".*>"
send "{ 'execute': 'qmp_capabilities' }"
expect -re ".*>"
send "{'execute':'dump-guest-memory','arguments':{'paging':false,'protocol':'file:memdump.bin'}}\r"
expect -re ".*>"
send "exit\r"
EOF

