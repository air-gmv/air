sudo rm -fr /tmp/digilent-adept2-*
sudo rm -fr /dev/shm/diligent-adept2-*

# Zedboard 1
hw_server -stcp::3121 -p3000 -d -e "set jtag-port-filter Digilent/Zed/210248B84B56"  

# Zedboard 2
hw_server -stcp::3122 -p3100 -d -e "set jtag-port-filter Digilent/Zed/210248B77F10"

# Others from Xilinx
hw_server -stcp::3123 -p3200 -d -e "set jtag-port-filter Xilinx"