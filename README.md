# Welcome to AIR v5.6.0

AIR has a user manual at https://gmvdrive.gmv.com/index.php/s/eScXCAybbecmT9b

You have two ways of getting AIR
1) AIR is intregrated in TASTE toolchain available at  https://taste.tuxfamily.org/
2) Download from repository (email to airproduct@gmv.com to register)

Below is an example on installing and using AIR.
This example is use SPARC target, if ARM read section 4.2.1 of user manual.
To install AIR after cloning from git, you should do the following:

## 1 - Install RTEMS toolchain for RTEMS 5
Execute the following:

```
git clone git://git.rtems.org/rtems-source-builder.git rsb
cd rsb
./source-builder/sb-check
cd rtems
../source-builder/sb-set-builder --prefix=/opt/rtems/5 5/rtems-sparc
```

Currently supported toolchain version : rtems-5.1-2019.07.25

## 2 - Configure and build AIR
AIR configure requires python-mako, if not installed execute (in case of Debian)

    sudo apt-get install python-mako

Clone AIR repo for example in */home/taste/work/*


Add the *air_repo/air* to the PATH env variable, we recommend doing the following way in .bashrc file:

    export AIR=/home/taste/work/AIR/air
    export PATH=$PATH:$AIR

    git clone https://spass-git-ext.gmv.com/AIR/AIR.git
RTEMS 5 tool chain should be in the PATH, e.g:
    `export PATH=$PATH:/opt/rtems/5/bin` 

AIR supports the following paravirtualized RTOS versions
- RTEMS 4.8i (Edisoft)
- RTEMS 5
- BARE OS (Metal-bare)

AIR paravirtualized RTOS are obtained via git submodules. For example to use AIR 
RTEMS 5 version please execute in root folder of the repository.

    git submodule init air/pos/rtems5
    git submodule update

## 3 - Using AIR configurator tool

From here you can use AIR GUI, by executing On *air_repo/air* :
    ./air-gui 

Alternatively if you prefer command line, do the following step:
On *air_repo/air* execute

    ./configure 

Configurator interface will enquire the user for options (architecture, BSP, FPU, Cache, Debugger, RTOS). For example

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
If not using the GUI, go to any example inside *air_repo/air/examples*

Execute

    ../../configure

This generate the makefiles taking as input the XML configuration file.

It generates a parent makefile and then a makefile per partition inside the respective partition folder.
 
Executing

    make all

will create the respective binary in the created *executables* folder.

## 5 - Updating AIR repo to AIR v5.6.0 and configurator v4.3
Since configurator v4.3 the RTOS paravirtualized versions are obtained via git submodules.
Pulling new AIR content into old AIR repo will generate conflicts. To get a new clean
AIR repo please execute (please backup any working folder)

    git fetch
    git reset origin/master --hard
    
Further information email air-product@gmv.com

## 6 - Support
### Via Gitlab
Users can submit issues on gitlab, where they can follow development team addressing them.
Gitlab address:
https://spass-git-ext.gmv.com/AIR

### Via Email
Email to airproduct@gmv.com or raise issue at our Gitlab
