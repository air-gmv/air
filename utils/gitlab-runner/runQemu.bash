#!/bin/bash

function Memdump() {
    expect << EOF
spawn telnet localhost 4444
expect -re ".*>"
send "{ 'execute': 'qmp_capabilities' }"
expect -re ".*>"
send "{'execute':'dump-guest-memory','arguments':{'paging':false,'protocol':'file:memdump.bin'}}\r"
expect -re ".*>"
send "exit\r"
EOF

}

( /opt/qemu/bin/qemu-system-arm -M arm-generic-fdt-7series -serial mon:stdio -display none -dtb /home/air/workspace/dtb/system.dtb -kernel executable/AIRAPP.exe & echo $! > qemu.pid ) | tee testresult.txt &

while sleep 1
do
    if grep -q -m1 "AIR has been terminaded by the application." testresult.txt
    then 
        Memdump
        kill $(cat qemu.pid)
        exit 0
    fi
done

