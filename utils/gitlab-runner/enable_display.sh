#!/bin/bash

TASTE_PW=$1

# Check if the Xvfb process is running
if ps aux | grep "[X]vfb" >/dev/null; then
    echo "The Xvfb process is running. Continuing."
else
    echo "The Xvfb process is not running. Starting it now..."
    sudo -S <<< "$TASTE_PW" Xvfb :99 &
    echo "The Xvfb process was successfully started."
fi
