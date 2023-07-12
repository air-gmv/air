# Welcome to AIR v5.6.1b

AIR has a user manual that you can request by e-mailing airproduct@gmv.com

You have two ways to obtain AIR:
1) Download it from GitHub at https://github.com/air-gmv/air.
2) AIR is integrated in the TASTE toolchain, which is available at https://taste.tuxfamily.org/ inside the ~/tool-src/air folder.

Below is an example of how to install and use AIR. This example focuses on using the SPARC target. If you are using ARM, please refer to section 4.2.1 of the user manual.

To install AIR after cloning it from git, follow these steps:


## 1 - Install RTEMS toolchain for RTEMS 5
Execute the following commands:

```
git clone git://git.rtems.org/rtems-source-builder.git rsb
git checkout 5
cd rsb
./source-builder/sb-check
cd rtems
../source-builder/sb-set-builder --prefix=/opt/rtems/5 5/rtems-sparc
```
Currently supported toolchain version : rtems-5.1-2019.07.25

You may need the texinfo package, which can be installed by executing sudo apt-get install texinfo or a similar command.


## 2 - Configure and build AIR
AIR configuration requires python-mako. If it is not installed, execute the following commands (for Debian):

    sudo apt-get install python3-mako
    sudo apt install python3-tk
    sudo apt install python3-venv

Clone AIR repo for example in */home/taste/work/*


Add the air_repo/air to the PATH environment variable. 
We recommend doing it the following way in the .bashrc file:

    export AIR=/home/taste/work/AIR/air
    export PATH=$PATH:$AIR

Clone the AIR repository from https://github.com/air-gmv/air.git.

The RTEMS 5 toolchain should be in the PATH, for example:
    `export PATH=$PATH:/opt/rtems/5/bin` 

The board support package depends on the chosen architecture:
- SPARC   
    -  laysim_gr740 
    - leon4    
    - leon3_or_tsim2 
    - tsim 
- ARM  
    - zynqz1

Floating Point Unit option can be enabled/disabled. 

AIR supports the following paravirtualized RTOS versions:
- RTEMS 4.8i (Edisoft)
- RTEMS 5
- POSIX RTEMS 5
- BARE OS (Metal-bare)

The paravirtualized RTOS and AIR GUI for AIR are obtained via git submodules. 

To use the AIR RTEMS 5 version and AIR GUI, please execute the following commands in the root folder of the repository:

    git submodule init air/pos/rtems5
    git submodule init air/tools/air-gui
    git submodule update


## 3 - Using AIR configurator tool

Do the following step:
On *air_repo/air* execute

    ./configure 

Alternatively, an alpha version of AIR GUI is available. 
To access it, execute the following command in the air_repo/air/tools/air-gui/ directory:
    ./launch.sh



The configurator interface will prompt the user for options (architecture, BSP, FPU, Cache, Debugger, RTOS). 
For example:

* Press 0 for sparc 
* Press 2 for leon4
* Press 0 for fpu enabled
* Press 0 for GRMON
* Press 1 to install all RTOS

After configurator finishes execute:

```
make clean
make
```

## 4- Build an example ready to run on a target board

Go to any example inside *air_repo/air/examples*

Execute

    ../../configure

This will generate the makefiles taking the XML configuration file as input.

It generates a parent makefile and then a makefile per partition inside the respective partition folder.
 
Executing

    make all

will create the respective binary in the created *executables* folder.


## 5 - Updating AIR repo to AIR v5.6.1 and configurator v4.4
Since configurator v4.3 the RTOS paravirtualized versions are obtained via git submodules.
Pulling new AIR content into old AIR repo will generate conflicts. To get a new clean
AIR repo please execute (please backup any working folder)

    git fetch
    git reset origin/master --hard
    
Further information email air-product@gmv.com


## 6 - Support
### Via Github
Users can submit issues on github, where they can follow development team addressing them.
Github address:
https://github.com/air-gmv/air
The development team addresses internally in a closed gitlab platform, sometimes associated gitlab links are provided.

### Via Email
Email to airproduct@gmv.com
