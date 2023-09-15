#!/bin/bash

# Try to copy install folder if there exists one
latest_commit_sha=$(git rev-parse HEAD^)
echo "Latest Commit SHA: $latest_commit_sha"
latest_state_folder="/home/gitlab-runner/state/${latest_commit_sha}"
echo "Latest State Folder: $latest_state_folder"

need_pos=true
need_libs=true
need_pmk=true
need_tools=true
if [ -d "$latest_state_folder" ]; then
  install_folder="$latest_state_folder/air/install"

  if [ -d "$install_folder" ]; then
      libs_folder="$install_folder/libs"
      pmk_folder="$install_folder/pmk"
      pos_folder="$install_folder/pos"
      tools_folder="$install_folder/tools"

      mkdir install

      if [ -d "$pos_folder" ] && [ -n "$(ls -A "$pos_folder")" ]; then
        echo "Copying pos from the latest state folder: $latest_state_folder"
        cp -r "$install_folder/pos" ./install/pos
        need_pos=false
      fi

      if [ -d "$libs_folder" ] && [ -n "$(ls -A "$libs_folder")" ]; then
        echo "Copying libs from the latest state folder: $latest_state_folder"
        cp -r "$install_folder/libs" ./install
        need_libs=false
      fi

      if [ -d "$pmk_folder" ] && [ -n "$(ls -A "$pmk_folder")" ]; then
        echo "Copying pmk from the latest state folder: $latest_state_folder"
        cp -r "$install_folder/pmk" ./install
        need_pmk=false
      fi

      if [ -d "$tools_folder" ] && [ -n "$(ls -A "$tools_folder")" ]; then
        echo "Copying tools from the latest state folder: $latest_state_folder"
        cp -r "$install_folder/tools" ./install
        need_tools=false
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
  if [ -d "/install/pos" ]; then
    rm -rf install/pos
  fi
  make pos
  need_pos=false
fi

# Make anything that wasn't copied
if $need_pos; then
    echo "Copying pos failed or folder was empty, running make pos all"
    make -C pos all
fi

if $need_libs; then
    echo "Copying libs failed or folder was empty, running make libs all"
    make -C libs all
fi

if $need_pmk; then
    echo "Copying pmk failed or folder was empty, running make pmk all"
    make -C pmk all
fi

if $need_tools; then
    echo "Copying tools failed or folder was empty, running make tools"
    make tools
fi