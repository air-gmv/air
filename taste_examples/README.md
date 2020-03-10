TASTE is an an open-source tool-chain for embedded software development developed mainly by Elidiss and ISAE to ESA.

Their website is https://taste.tuxfamily.org/

They also explain taste in this presentation https://download.tuxfamily.org/taste/misc/what_is_taste.pdf

Use TASTE with AIR

TASTE has integrated AIR, it can be found at the following folder
/home/taste/tool-src/air


Then, in the Deployment View you will be able to create a Leon4 processor with AIR hypervisor ( d'n'd leon3.AIR from the processor list to the Deployment View)
By editing the properties of the partitions, you will find a new property ( Execution Platform) to set the partition OS. 
To set the memories value, edit the Node properties.
To set the slots and major frame value, edit the Processor properties

To generate the TSP executable, assuming AIR is installed and configured on your computer, in the Tools menu, 
select "Build the TSP system (in C)" 
That will create a directory named "TSP" into the "binary.c" directory where all the needed files are copied.
The AIR configuration and the code are generated then compiled.
Since the root in TASTE is always deploymentview.final, you will find the generated files into [root]/binary.c/TSP/deploymentview_final


cd binary.c/TSP/deploymentview_final/
and call "make"
