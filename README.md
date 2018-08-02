# Wellcome to AIR

AIR has a user manual at https://gmvdrive.gmv.com/index.php/s/eScXCAybbecmT9b

To install AIR after cloning from git, you should do the following:

1) Install RTEMS toolchain for RTEMS 4.10 (used to build RTEMS48i)

The source-builder tool does the installation.

You should download rtems-source-builder-4.10.3-rc2.tar.xz, located at https://ftp.rtems.org/pub/rtems/releases/4.10/4.10.3-rc2/
BUT IT DOES NO LONGER BUILDS CORRECTLY!

Therefore AIR is now set to instead to use gaisler toolchain available at
https://www.gaisler.com/anonftp/rcc/bin/linux/sparc-rtems-4.10-gcc-4.4.6-1.2.22-linux.tar.bz2

Simply untar the file inside /opt folder as sudo.

2) Install RTEMS toolchain for RTEMS 5

Execute the following:

git clone git://git.rtems.org/rtems-source-builder.git rsb

$ cd rsb
$ ./source-builder/sb-check

$ cd rtems
$ ../source-builder/sb-set-builder --prefix=/opt/rtems/5 5/rtems-sparc

3) Configure and build AIR

Add the air_repo/air to the PATH env variable.

Go to the folder air_repo/air”
Run “configure” and set the following options (for SPARC LEON4 with FPU enable)

Press 0 for sparc
Press 2 for leon4
Press 0 for fpu enabled

Then write “make clean” then next do a “make all”.

A) To build an example ready to run on a target board

The go to any example inside air/example.
 
Run “configure” on the example and it will generate the makefiles taking as input the XML configuration file.
 
It generates a parent makefile and then a makefile per partition inside the respective partition folder.
 
Running make all at the example will create the respective binary.

B) Support
Email to $(AIR_RUNNER_USER)@gmv.com or raise issue at our Gitlab
