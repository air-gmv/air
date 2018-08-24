# GR740 candriver

This package contains the **gr740\_candriver** component which initializes the infrastructure on RTMEMS5.1 to trasnmit/receive CAN messages.
It is meant to get **joint** commands, transform it into suitable CAN messages and then get new **joint** samples from the device.

## Caveats \& Tipps

Here we need a user\_init\_pre script to add an include path to the GR740 board-support-package.
Additionally, the misc/aadl-library/ocarina-components.aadl file has to be changed such that the CFLAGS contain *-DCONFIGURE_DRIVER_AMBAPP_GAISLER_CAN*.
Otherwise the gr740.rtems5\_posix code would not include the necessary driver.

After conversation with thanassis.tsiodras@esa.int there is now the possibility to assign to a function so called contextual parameters of type Taste\_directive.
With

> compiler\_option:"..."

as value the CFLAGS of the build process (namely the orchestrator) can be extended.

If you encounter errors like this

> CPU 2:  IU exception (tt = 0x01, instruction access exception)
>         0xded96e80: 00f000f0  unknown opcode: 0x00F000F0

then you might have to increase the stack sizes of your functions in the ConcurrencyView.pro file
or you have to make some local variables global to reduce the amount of stack size usage.


When you use this component in combination with another target like an **x86/linux** system
be **aware** that you should change the encoding to **UPER**.
Otherwise you might get garbage from the GR740 or the Linux system.
