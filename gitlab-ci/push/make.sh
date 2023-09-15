#!/bin/bash

# make pmk, libs and tools without dependencies
make -C pmk all
make -C libs all
make tools

# make pos or copy if one already exists

# Initialize submodules
git submodule update --init --recursive

missing_pos=false

if git submodule summary | grep -q 'air/pos/rtems5'; then
  echo "Submodule has changes, running make pos"
  make pos
else
  latest_commit_sha=$(git rev-parse HEAD^)
  echo "Latest Commit SHA: $latest_commit_sha"
  latest_state_folder="/home/gitlab-runner/state/${latest_commit_sha}"
  echo "Latest State Folder: $latest_state_folder"
  
  if [ -d "$latest_state_folder" ]; then
    air_folder="$latest_state_folder/air"
    pos_folder="$air_folder/install/pos"
    bare_folder="$pos_folder/bare"
    rtems5_folder="$pos_folder/rtems5"
    
    if [ -d "$air_folder" ] && [ -d "$pos_folder" ] && [ -d "$bare_folder" ] && [ -d "$rtems5_folder" ]; then
      if [ "$(ls -A "$bare_folder")" ] && [ "$(ls -A "$rtems5_folder")" ]; then
        echo "Copying pos from the latest state folder: $latest_state_folder"
        cp -r "$latest_state_folder/air/install/pos" ./install
      else
        echo "Either 'bare' or 'rtems5' directory is empty."
        missing_pos=true
      fi
    else
      echo "Missing one or more required subdirectories: 'air', 'pos', 'bare', or 'rtems5'."
      missing_pos=true
    fi
  else
    echo "No changes in the submodule, but no latest state folder found."
    missing_pos=true
  fi
fi

if [ "$missing_pos" = true ]; then
  echo "Error: Unable to copy 'pos' or missing required subdirectories."
  make pos
fi
