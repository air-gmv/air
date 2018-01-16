#!/bin/sh
export RTEMS_API=5
export RTEMS_CPU=sparc
export RTEMS_BSP=leon3
export RTEMS_MAKEFILE_PATH=/opt/rtems/$RTEMS_API/$RTEMS_CPU-rtems$RTEMS_API/$RTEMS_BSP
echo $RTEMS_MAKEFILE_PATH
rm -rf b-$RTEMS_BSP/
make -f Makefile-5
