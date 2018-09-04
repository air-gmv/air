TASTE is an an open-source tool-chain for embedded software development developed mainly by Elidiss and ISAE to ESA.

Their website is https://taste.tuxfamily.org/

They also explain taste in this presentation https://download.tuxfamily.org/taste/misc/what_is_taste.pdf

Edit this section&nbsp;
 
Use TASTE with AIR

TASTE is going to support AIR, it is not yet official, but is doable following instructions below:

are you using the default TASTE editor from the 9.1?
If yes, you cannot use TSP since it is a ERGO specific enhancement not already merged in the TASTE tool chain.
You can get the ERGO specific editor on our wiki: http://www.ellidiss.fr/public/wiki/attachment/wiki/ErgoUpdatePage/
It is named TASTE-ERGO-0.0.15-linux32(64).tar.gz
If you already have a TASTE VM 9.1 installed, just uncompress the archive where you want to install the editor and add its bin directory
to the head of the PATH variable. Then TASTE scripts will use this ERGO specific version.
Otherwise, I can send you an online installation script I have done for ERGO (available on the ERGO wiki only)

An other point,  the file ~/tool-inst/share/ocarina/AADLv2/ocarina_components.aadl 
is not up to date. It misses a AIR processor to the list of available processors.
Please add the following to this file at line 2639:


processor implementation leon3.AIR
properties
Deployment::Execution_Platform => AIR;
end leon3.AIR;

Then, in the Deployment View you will be able to create a Leon3 processor with AIR hypervisor ( d'n'd leon3.AIR from the processor list to the Deployment View)
By editing the properties of the partitions, you will find a new property ( Execution Platform) to set the partition OS. 
Right now, as far as I know, only AIR OS is supported with AIR hypervisor, thus you will not be able to change its value, but
as soon as new OS will be supported, the list will be populated.
To set the memories value, edit the Node properties.
To set the slots and major frame value, edit the Processor properties

To generate the TSP executable, assuming AIR is installed and configured on your computer, in the Tools menu, 
select "Build the TSP system (in C)" 
That will create a directory named "TSP" into the "binary.c" directory where all the needed files are copied.
The AIR configuration and the code are generated then compiled.
Since the root in TASTE is always deploymentview.final, you will find the generated files into [root]/binary.c/TSP/deploymentview_final

There are two little examples in [TASTE editor root]/Workspace/TSP/ (based on leon3.AIR processor)
One with two unconnected functions bound into two partitions.
An other with two connected functions bound into two partitions.

In any examoke, After calling the "build TSP System in C" the user still needs to build the application by going to

cd binary.c/TSP/deploymentview_final/
and call "make"
