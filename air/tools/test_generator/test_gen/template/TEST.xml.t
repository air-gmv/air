<?xml version="1.0" encoding="UTF-8"?> 
<ARINC_653_Module xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ModuleName="${name}"> 
	
    <!-- =========================================================================================
    System Health Monitor Table
    ========================================================================================= -->	
    <System_HM_Table>
        <System_State_Entry Description="Module Initialization" SystemState="0">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Configuration Error" ErrorIdentifier="8" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Module Config Error" ErrorIdentifier="9" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Stack error" ErrorIdentifier="10" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Debug Error" ErrorIdentifier="11" ErrorLevel="MODULE"/>
        </System_State_Entry>
        <System_State_Entry Description="PMK Execution" SystemState="1">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Configuration Error" ErrorIdentifier="8" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Module Config Error" ErrorIdentifier="9" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Stack error" ErrorIdentifier="10" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Debug Error" ErrorIdentifier="11" ErrorLevel="MODULE"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition Initialization" SystemState="2">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Partition Conf Error" ErrorIdentifier="8" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Partition Init Error" ErrorIdentifier="9" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Stack error" ErrorIdentifier="10" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Debug Error" ErrorIdentifier="11" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="APEX Error" ErrorIdentifier="12" ErrorLevel="PARTITION"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition Operating System" SystemState="3">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Partition Conf Error" ErrorIdentifier="8" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Partition Init Error" ErrorIdentifier="9" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Stack error" ErrorIdentifier="10" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Debug Error" ErrorIdentifier="11" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="APEX Error" ErrorIdentifier="12" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Deadline Miss" ErrorIdentifier="13" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="App Hardware Error" ErrorIdentifier="14" ErrorLevel="PARTITION"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition Application" SystemState="4">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Partition Conf Error" ErrorIdentifier="8" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Partition Init Error" ErrorIdentifier="9" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Stack error" ErrorIdentifier="10" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Debug Error" ErrorIdentifier="11" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="APEX Error" ErrorIdentifier="12" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Deadline Miss" ErrorIdentifier="13" ErrorLevel="PARTITION"/>
        </System_State_Entry>
        <System_State_Entry Description="Health Monitor Processing" SystemState="6">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Stack error" ErrorIdentifier="10" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Debug Error" ErrorIdentifier="11" ErrorLevel="MODULE"/>
        </System_State_Entry>
    </System_HM_Table>

    <Module_HM_Table>
        <System_State_Entry Description="Module Initialization" SystemState="0">
            <Error_ID_Action Action="SHUTDOWN" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="IGNORE" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="SHUTDOWN" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Configuration Error" ErrorIdentifier="8"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Module Config Error" ErrorIdentifier="9"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Debug Error" ErrorIdentifier="11"/>
        </System_State_Entry>
        <System_State_Entry Description="PMK Execution" SystemState="1">
            <Error_ID_Action Action="SHUTDOWN" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="SHUTDOWN" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="IGNORE" Description="Configuration Error" ErrorIdentifier="8"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Module Config Error" ErrorIdentifier="9"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Debug Error" ErrorIdentifier="11"/>
        </System_State_Entry>
        <System_State_Entry Description="Health Monitor Processing" SystemState="6">
            <Error_ID_Action Action="SHUTDOWN" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="SHUTDOWN" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Debug Error" ErrorIdentifier="11"/>
        </System_State_Entry>
    </Module_HM_Table>
	
    <!-- =========================================================================================
    Partition Table
    ========================================================================================= -->
	
    % for p in range(0,partitions):	
    <!-- partition ${p} -->
    <Partition 	PartitionIdentifier="${p}" 
                PartitionName="p${p}" 
                Criticality="LEVEL_A" 
                SystemPartition="false" 
                EntryPoint="test_main">
        <PartitionConfiguration Personality="${personality}" Cores="1">
            <Libs>LIBAIR;LIBPRINTF;IMASPEX;LIBTEST</Libs>
            <Cache>CODE; DATA</Cache>
            <Memory Size="0x500000" />
            <Permissions>
                FPU_CONTROL; CACHE_CONTROL; GLOBAL_TIME; SET_TOD; SET_PARTITION_MODE;
            </Permissions>
        </PartitionConfiguration>
    </Partition>
    % endfor
	
	<% p += 1 %>
    <!-- partition ${p} : Control partition -->
    <Partition 	PartitionIdentifier="${p}" 
                PartitionName="p${p}" 
                Criticality="LEVEL_A" 
                SystemPartition="false" 
                EntryPoint="test_printf">
        <PartitionConfiguration Personality="${personality}" Cores="1">
            <Libs>LIBAIR;LIBPRINTF;IMASPEX;LIBTEST</Libs>
            <Cache>CODE; DATA</Cache>
            <Memory Size="0x500000" />
            <Permissions>
                FPU_CONTROL; CACHE_CONTROL; GLOBAL_TIME; SET_TOD; SET_PARTITION_MODE; MODULE_CONTROL;
            </Permissions>
        </PartitionConfiguration>
    </Partition>	
	
    <!-- =========================================================================================
    Module Schedules
    ========================================================================================= -->	

    <!-- module schedule 1 -->
    <Module_Schedule  ScheduleIdentifier="0" ScheduleName="test_sched" InitialModuleSchedule="true" MajorFrameSeconds="${mtf}">
        % for p in range(0,partitions+1):		
		<% part_slot = round(mtf / (partitions + 1),4) %>
        <Partition_Schedule PartitionIdentifier="${p}" 
                            PartitionName="p${p}" 
                            PeriodSeconds="${mtf}" 
                            PeriodDurationSeconds="${part_slot}">
            <Window_Schedule 	WindowIdentifier="${p}01" 
                                WindowStartSeconds="${p * part_slot}" 
                                WindowDurationSeconds="${part_slot}" 
                                PartitionPeriodStart="true"/>
        </Partition_Schedule>
        % endfor		
    </Module_Schedule>
	
    <!-- =========================================================================================
    Partition Health Monitor Table
    ========================================================================================= -->
    % for p in range(0,partitions+1):		
    <Partition_HM_Table PartitionIdentifier="${p}" PartitionName="p${p}" PartitionCallback="hm_part_callback">
        <System_State_Entry Description="Partition Initialization" SystemState="2">
            <Error_ID_Action Action="IGNORE" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="COLD_START" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="COLD_START" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="COLD_START" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="IGNORE" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="COLD_START" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="WARM_START" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="COLD_START" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="IDLE" Description="Partition Conf Error" ErrorIdentifier="8"/>
            <Error_ID_Action Action="IDLE" Description="Partition Init Error" ErrorIdentifier="9"/>
            <Error_ID_Action Action="COLD_START" Description="Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="IGNORE" Description="Debug Error" ErrorIdentifier="11"/>
            <Error_ID_Action Action="WARM_START" Description="APEX Error" ErrorIdentifier="12"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition Operating System" SystemState="3">
            <Error_ID_Action Action="IGNORE" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="COLD_START" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="COLD_START" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="COLD_START" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="IGNORE" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="COLD_START" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="COLD_START" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="COLD_START" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="COLD_START" Description="Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="IGNORE" Description="Debug Error" ErrorIdentifier="11"/>
            <Error_ID_Action Action="WARM_START" Description="APEX Error" ErrorIdentifier="12"/>
            <Error_ID_Action Action="WARM_START" Description="Deadline Miss" ErrorIdentifier="13"/>
            <Error_ID_Action Action="COLD_START" Description="App Hardware Error" ErrorIdentifier="14"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition Application" SystemState="4">
            <Error_ID_Action Action="IGNORE" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="COLD_START" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="COLD_START" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="COLD_START" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="IGNORE" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="WARM_START" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="WARM_START" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="COLD_START" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="COLD_START" Description="Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="IGNORE" Description="Debug Error" ErrorIdentifier="11"/>
            <Error_ID_Action Action="WARM_START" Description="APEX Error" ErrorIdentifier="12"/>
            <Error_ID_Action Action="WARM_START" Description="Deadline Miss" ErrorIdentifier="13"/>
        </System_State_Entry>
    </Partition_HM_Table>
    % endfor
	
    <!-- =========================================================================================
    Connection Table
    ========================================================================================= -->
    <Connection_Table>  
	
    </Connection_Table>
	
    <AIR_Configuration TicksPerSecond="${tps}" RequiredCores="1"/>
</ARINC_653_Module>  
