# Welcome to AIR v5.4.1

AIR has a user manual at https://gmvdrive.gmv.com/index.php/s/eScXCAybbecmT9b

You have two ways of getting AIR
1) AIR is intregrated in TASTE toolchain available at  https://taste.tuxfamily.org/
2) Download from repository where following instructions apply:

Below is a guide for SPARC target, regarding ARM read section 4.2.1.
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

    git clone https://github.com/air-gmv/air.git
RTEMS 5 tool chain should be in the PATH, e.g:
    `export PATH=$PATH:/opt/rtems/5/bin` 

AIR supports the following paravirtualized RTOS versions
- RTEMS 4.8i (Edisoft)
- RTEMS 5

AIR paravirtualized RTOS are obtained via git submodules. For example to use AIR 
RTEMS 5 version please execute in *air_repo/air*

    git submodule init air/pos/rtems5
    git submodule update

## 3 - Using AIR configurator tool
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
Then go to any example inside *air_repo/air/examples*

Execute

    ../../configure

This generate the makefiles taking as input the XML configuration file.

It generates a parent makefile and then a makefile per partition inside the respective partition folder.
 
Executing

    make all

will create the respective binary in the created *executables* folder.

## 5 - Updating AIR repo to AIR v5.4.0 and configurator v4.3
Since configurator v4.2 the RTOS paravirtualized versions are obtained via git submodules.
Pulling new AIR content into old AIR repo will generate conflicts. To get a new clean
AIR repo please execute (please backup any working folder)

    git fetch
    git reset origin/master --hard
    
Further information email air-product@gmv.com

## 6 - Support
### Via Gitlab
Users can submit issues on gitlab, where they can follow development team addressing them.
Gitlab address:
$(AIR_GIT_REMOTE_URL)/AIR

### Via Email
Email to avos_pt@gmv.com or raise issue at our Gitlab
