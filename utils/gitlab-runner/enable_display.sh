#!/bin/bash

TASTE_PW=$1

# Check if the Xvfb process is running
if ps aux | grep "[X]vfb" >/dev/null; then
    echo "The Xvfb process is running. Continuing."
else
    echo "The Xvfb process is not running. Starting it now..."
    sudo -b -n -S <<< "$TASTE_PW" Xvfb :99 &
    echo "The Xvfb process was successfully started."
    # currently gets stuck on the pipeline past this point
fi

exit 0
