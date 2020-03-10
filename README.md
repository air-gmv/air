# Welcome to AIR

AIR has a user manual at https://gmvdrive.gmv.com/index.php/s/eScXCAybbecmT9b

You have two way of getting AIR
1) AIR is integrrated in TASTE toolchain available at  https://taste.tuxfamily.org/

2) Download from repository where following instructions apply:

To install AIR after cloning from git, you should do the following:

## 1 - Install RTEMS toolchain for RTEMS 5

Execute the following:

git clone git://git.rtems.org/rtems-source-builder.git rsb

$ cd rsb
$ ./source-builder/sb-check

$ cd rtems
$ ../source-builder/sb-set-builder --prefix=/opt/rtems/5 5/rtems-sparc

Currently supported toolchain version : rtems-5.1-2019.07.25

## 2 - Configure and build AIR
AIR configure requires python-mako, if not installed execute (in case of Debian)

    sudo apt-get install python-mako

Clone AIR repo for example in */home/taste/work/*

    git clone https://spass-git-ext.gmv.com/AIR/AIR.git

Add the *air_repo/air* to the PATH env variable, we recommend doing the following way in .bashrc file:

    export AIR=/home/taste/work/air/air
    export PATH=$PATH:$AIR

AIR supports the following paravirtualized RTOS versions
- RTEMS 4.8i
- RTEMS 5

AIR paravirtualized RTOS are obtained via git submodules. For example to use AIR 
RTEMS 5 version please execute in *air_repo/air*

    git submodule init air/pos/rtems5
    git submodule update

### Using AIR configurator tool
On *air_repo/air* execute

    ./configure

Configurator interface will enquire the user for options (architecture, BSP, FPU, Cache, Debugger, RTOS). For example

Press 0 for sparc

Press 2 for leon4

Press 0 for fpu enabled

Press 0 for GRMON

Press 1 to install all RTOS
...

After configurator finishes execute

    make

### To build an example ready to run on a target board

Then go to any example inside *air_repo/air/examples*

Execute

    ../../configure

This generate the makefiles taking as input the XML configuration file.

It generates a parent makefile and then a makefile per partition inside the respective partition folder.
 
Executing

    make all

will create the respective binary in the created *executables* folder.

## Support
Email to dtms@gmv.com or raise issue at our Gitlab

## Updating AIR repo to AIR v5.3 and configurator v4.3
Since configurator v4.2 the RTOS paravirtualized versions are obtained via git submodules.
Pulling new AIR content into old AIR repo will generate conflicts. To get a new clean
AIR repo please execute (please backup any working folder)

    git fetch
    git reset origin/master --hard
    
Further information email air-product@gmv.com
