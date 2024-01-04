puts "Starting"

set fsbl 0

connect -symbols

source /tools/Xilinx/Vitis/2022.1/scripts/vitis/util/zynqmp_utils.tcl

# Reset entire system
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
rst -system

#Reset A53, load and run FSBL
targets -set -filter {name =~"APU*" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
enable_a32_mode 0

if {$fsbl == 1 } {
    puts "Initializing with FSBL"
    configparams force-mem-access 1
    targets -set -filter {name =~ "*A53*0" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
    rst -processor

    dow $env(AIR)/../utils/hw_scripts/Ultrascale/fsbl_A53_32b.elf

    set bp_32_18_fsbl_bp [bpadd -addr &XFsbl_Exit]
    con -block -timeout 60
    bpremove $bp_32_18_fsbl_bp
} else {
    puts "Initializing with psu_init"
    source $env(AIR)/../utils/hw_scripts/Ultrascale/psu_init.tcl
    psu_init
    after 1000
    psu_ps_pl_isolation_removal
    after 1000
    psu_ps_pl_reset_config
    catch {psu_protection}
}

#set halt on debug on system counter
mwr 0xFF260000 0x00000003 

# Enable Cross Triggering to halt system counter when cpu halts
set bp_10_4_0 [bpadd -target-id all -ct-input {16} -ct-output {63} -skip-on-step 0]

#Load app 
targets -set -filter {name =~ "*A53*0" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
rst -proc
configparams force-mem-access 0

# set bp_end [bpadd -addr &bsp_shutdown_core]

dow $env(APP)

after 500
con
# con -block
# puts "Test ended"
# con
# after 500
# exit
