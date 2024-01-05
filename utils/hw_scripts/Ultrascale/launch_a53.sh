#!/bin/bash
set -e

LOCKFILE="/var/lock/ultrascale.lock"

# Argument handling
## Check if an argument is provided
if [ $# -ne 1 ]; then
  echo "Usage: $0 <executable_path>"
  exit 1
fi

if ! lockfile -r 0 "$LOCKFILE"; then
  echo "Board lockfile is locked. Remove lock? (y/n) "
  read -r answer
  if [ "$answer" == "y" ]; then
    rm -f "$LOCKFILE"
    exit 0
  fi
fi

export APP="$1"

ipcrm -a

putty /dev/ttyUSB7 -serial -sercfg "115200,8,n,1,N" &

xsct -interactive $AIR../utils/hw_scripts/Ultrascale/launch_a53.tcl

rm -f /var/lock/ultrascale.lock

pkill putty
