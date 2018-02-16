#!/bin/sh
export RTEMS_API=5
export RTEMS_CPU=sparc
export RTEMS_BSP=leon3
#export RTEMS_MAKEFILE_PATH=/opt/rtems/$RTEMS_API/$RTEMS_CPU-rtems$RTEMS_API/$RTEMS_BSP
#echo $RTEMS_MAKEFILE_PATH

pwd=`pwd`
export AIR_ROOT=$pwd/../../../
export AIR_INSTALL=$AIR_ROOT/install
export AIR_PMK=$AIR_INSTALL/pmk
export AIR_POS=$AIR_INSTALL/pos
export AIR_LIBS=$AIR_INSTALL/libs
export AIR_TOOLS=$AIR_INSTALL/tools

rm -rf b-$RTEMS_BSP/
make -f Makefile-5
