puts "Starting"

set fsbl 0

if { $argc == 1 } {
    set multicore [lindex $argv 0]
    if {$multicore > 1} {
        puts "Multicore example!"
    }
} else {
    set multicore 1
}

connect -symbols

source /tools/Xilinx/Vitis/2022.1/scripts/vitis/util/zynqmp_utils.tcl

# Reset entire system
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
rst -system

#Reset A53, load and run FSBL
targets -set -filter {name =~"APU*" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
enable_a32_mode 0

if {$multicore == 2} {
    enable_a32_mode 1
} elseif {$multicore == 4} {
    enable_a32_mode 1
    enable_a32_mode 2
    enable_a32_mode 3
} else {
    if {$multicore != 1} { # multicore is not 1, 2 or 4
        puts "Invalid multicore value"
        puts $multicore
        exit   
    }
}

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
    after 500
    psu_ps_pl_isolation_removal
    after 500
    psu_ps_pl_reset_config
    catch {psu_protection}
}

# set halt on debug on system counter
mwr 0xFF260000 0x00000003 

# Enable Cross Triggering to halt system counter when cpu 0 halts
set bp_10_4_0 [bpadd -target-id all -ct-input {16} -ct-output {63} -skip-on-step 0]

# if {$multicore > 1} {
#     # Cross trigger to stop CPU1 when CPU0 is stopped
#     bpadd -ct-input 16 -ct-output 24
#}

# Always start core 0
targets -set -filter {name =~ "*A53*0" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
rst -proc -isa A32 -clear-registers
configparams force-mem-access 0

dow $env(APP)

#Set breakpoint to the last action before powering down AIR
set bp_end [bpadd arm_set_cpu_clock_divisor]

con -block

if {$multicore >= 2} {
    targets -set -filter {name =~ "*A53*1" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
    rst -proc -isa A32 -clear-registers
    configparams force-mem-access 0

    after 1000

    # Also start core 1 at the program entry point.
    con -addr 0x100000

    if {$multicore == 4} {
        targets -set -filter {name =~ "*A53*2" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
        rst -proc -isa A32 -clear-registers
        configparams force-mem-access 0

        after 500

        # Also start core 2 at the program entry point.
        con -addr 0x100000

        targets -set -filter {name =~ "*A53*3" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
        rst -proc -isa A32 -clear-registers
        configparams force-mem-access 0

        after 500

        # Also start core 3 at the program entry point.
        con -addr 0x100000
    }
}

targets -set -filter {name =~ "*A53*0" && jtag_cable_name =~ "Avnet USB-to-JTAG/UART Pod V1 1234-oj1A"}
stpout
puts "Exiting in 3 seconds"
after 3000
exit