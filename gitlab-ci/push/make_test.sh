#!/bin/bash

# Try to copy install folder if there exists one
latest_commit_sha=$(git rev-parse HEAD^)
echo "Latest Commit SHA: $latest_commit_sha"
latest_state_folder="/home/gitlab-runner/state/${latest_commit_sha}"
echo "Latest State Folder: $latest_state_folder"

if [ -d "$latest_state_folder" ]; then
  install_folder="$latest_state_folder/air/install"
    
  if [ -d "$install_folder" ]; then
      libs_folder="$install_folder/libs"
      pmk_folder="$install_folder/pmk"
      pos_folder="$install_folder/pos"
      tools_folder="$install_folder/tools"

      if [ -d "$libs_folder" ] && [ -n "$(ls -A "$libs_folder")" ] && \
          [ -d "$pmk_folder" ] && [ -n "$(ls -A "$pmk_folder")" ] && \
          [ -d "$pos_folder" ] && [ -n "$(ls -A "$pos_folder")" ] && \
          [ -d "$tools_folder" ] && [ -n "$(ls -A "$tools_folder")" ]; then
          echo "All subfolders are non-empty"
          echo "Copying install from the latest state folder: $latest_state_folder"
          cp -r "$latest_state_folder/air/install" .
      else
          echo "One or more subfolders are empty"
      fi
  else
      echo "install folder not found in $latest_state_folder"
  fi
else
  echo "$latest_state_folder not found"
fi

# Make pos from scratch if there are any changes in rtems
git submodule update --init --recursive # Initialize submodules

if git submodule summary | grep -q -e 'air/pos/rtems48i' -e 'air/pos/rtems5'; then
  echo "Submodule has changes, running make pos"
  rm -rf install/pos
  make pos
fi

# Make anything that still needs to be made
make -C pos all
make -C pmk all
make -C libs all
make tools