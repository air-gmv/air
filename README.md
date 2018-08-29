# Welcome to AIR

AIR has a user manual at https://gmvdrive.gmv.com/index.php/s/eScXCAybbecmT9b

To install AIR after cloning from git, you should do the following:

1) Install RTEMS toolchain for RTEMS 5

Execute the following:

git clone git://git.rtems.org/rtems-source-builder.git rsb

$ cd rsb
$ ./source-builder/sb-check

$ cd rtems
$ ../source-builder/sb-set-builder --prefix=/opt/rtems/5 5/rtems-sparc

2) Configure and build AIR

Add the air_repo/air to the PATH env variable, we recommend doing the following way in .bashrc file:

- export AIR=/home/taste/work/air/air
- export PATH=$PATH:$AIR


AIR configure requires python-mako, if not installed execute (in case of Debian)
- sudo apt-get install python-mako


Go to the folder air_repo/air”
Run “configure” and set the following options (for SPARC LEON4 with FPU enable)

Press 0 for sparc
Press 2 for leon4
Press 0 for fpu enabled

Then write “make clean” then next do a “make all”.

A) To build an example ready to run on a target board

Then go to any example inside air/example.
 
Run “configure” on the example and it will generate the makefiles taking as input the XML configuration file.
 
It generates a parent makefile and then a makefile per partition inside the respective partition folder.
 
Running make all at the example will create the respective binary.

B) Support
Email to $(AIR_RUNNER_USER)@gmv.com or raise issue at our Gitlab
