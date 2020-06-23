<?xml version="1.0" encoding="UTF-8"?> 
<ARINC_653_Module xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ModuleName="${name}"> 
	
    <!-- =========================================================================================
    System Health Monitor Table
    ========================================================================================= -->	
    <System_HM_Table>
        <System_State_Entry Description="Partition is not Running" SystemState="0">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Configuration Error" ErrorIdentifier="8" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Config Error" ErrorIdentifier="9" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Stack error" ErrorIdentifier="10" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Debug error" ErrorIdentifier="11" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 4" ErrorIdentifier="12" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 5" ErrorIdentifier="13" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 6" ErrorIdentifier="14" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 7" ErrorIdentifier="15" ErrorLevel="MODULE"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition is Initializing" SystemState="1">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Configuration Error" ErrorIdentifier="8" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Config Error" ErrorIdentifier="9" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Stack error" ErrorIdentifier="10" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Debug error" ErrorIdentifier="11" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 4" ErrorIdentifier="12" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 5" ErrorIdentifier="13" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 6" ErrorIdentifier="14" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 7" ErrorIdentifier="15" ErrorLevel="MODULE"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition is Running" SystemState="2">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Configuration Error" ErrorIdentifier="8" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Initialization Error" ErrorIdentifier="9" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Stack error" ErrorIdentifier="10" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Debug error" ErrorIdentifier="11" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined APEX Error" ErrorIdentifier="12" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Error 5" ErrorIdentifier="13" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 6" ErrorIdentifier="14" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 7" ErrorIdentifier="15" ErrorLevel="MODULE"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition Execution is Halted" SystemState="3">
            <Error_ID_Level Description="Power Interrupt" ErrorIdentifier="0" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Illegal Instruction" ErrorIdentifier="1" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Segmentation Error" ErrorIdentifier="2" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Unimplemented Error" ErrorIdentifier="3" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Floating Point Error" ErrorIdentifier="4" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Overflow Error" ErrorIdentifier="5" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="Divide by zero" ErrorIdentifier="6" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="I/O Error" ErrorIdentifier="7" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Partition Conf Error" ErrorIdentifier="8" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Partition Init Error" ErrorIdentifier="9" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Stack error" ErrorIdentifier="10" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Debug error" ErrorIdentifier="11" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined APEX Error" ErrorIdentifier="12" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined User defined Deadline Miss" ErrorIdentifier="13" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined User defined App Hardware Error" ErrorIdentifier="14" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Error 7" ErrorIdentifier="15" ErrorLevel="MODULE"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition is Ready but not Running" SystemState="4">
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
            <Error_ID_Level Description="User defined Stack error" ErrorIdentifier="10" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Debug error" ErrorIdentifier="11" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined APEX Error" ErrorIdentifier="12" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Deadline Miss" ErrorIdentifier="13" ErrorLevel="PARTITION"/>
            <Error_ID_Level Description="User defined Error 6" ErrorIdentifier="14" ErrorLevel="MODULE"/>
            <Error_ID_Level Description="User defined Error 7" ErrorIdentifier="15" ErrorLevel="MODULE"/>            
        </System_State_Entry>
    </System_HM_Table>

    <Module_HM_Table>
        <System_State_Entry Description="Partition is not Running" SystemState="0">
            <Error_ID_Action Action="SHUTDOWN" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="IGNORE" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="SHUTDOWN" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Configuration Error" ErrorIdentifier="8"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Module Config Error" ErrorIdentifier="9"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Module Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Module Debug error" ErrorIdentifier="11"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 4" ErrorIdentifier="12"/>            
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 5" ErrorIdentifier="13"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 6" ErrorIdentifier="14"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 7" ErrorIdentifier="15"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition is Initializing" SystemState="1">
            <Error_ID_Action Action="SHUTDOWN" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="SHUTDOWN" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="SHUTDOWN" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="IGNORE" Description="User defined Configuration Error" ErrorIdentifier="8"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Module Config Error" ErrorIdentifier="9"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Module Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Module Debug error" ErrorIdentifier="11"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 4" ErrorIdentifier="12"/>            
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 5" ErrorIdentifier="13"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 6" ErrorIdentifier="14"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 7" ErrorIdentifier="15"/>            
        </System_State_Entry>
        <System_State_Entry Description="Partition is Running" SystemState="2">
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 5" ErrorIdentifier="13"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 6" ErrorIdentifier="14"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 7" ErrorIdentifier="15"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition Execution is Halted" SystemState="3">
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 7" ErrorIdentifier="15"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition is Ready but not Running" SystemState="4">
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 6" ErrorIdentifier="14"/>
            <Error_ID_Action Action="SHUTDOWN" Description="User defined Error 7" ErrorIdentifier="15"/>            
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
            <Memory Size="0x1500000" />
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
            <Memory Size="0x1500000" />
            <Permissions>
                FPU_CONTROL; CACHE_CONTROL; GLOBAL_TIME; SET_TOD; SET_PARTITION_MODE; MODULE_CONTROL;
            </Permissions>
        </PartitionConfiguration>
    </Partition>	
	
    <SharedMemory Id="1" Name="TTEST" Size="0x10000" Unit="0x1000">
        % for p in range(0,partitions+1):		
        <PartitionAccess PartitionIdentifier="${p}" PartitionName="p${p}" Permissions="RW"/>
        % endfor
    </SharedMemory>    
      
        
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
                            PeriodDurationSeconds="${part_slot}"
                        % if (p == 0):
                            SetModuleSchedule="true">
                        % else:
                            SetModuleSchedule="false">
                        % endif
            <Window_Schedule 	WindowIdentifier="${p}01" 
                                WindowStartSeconds="${p * part_slot}" 
                                WindowDurationSeconds="${part_slot}" 
                                PartitionPeriodStart="true"/>
        </Partition_Schedule>
        % endfor		
    </Module_Schedule>
    
        <!-- module schedule 2 -->
    <Module_Schedule  ScheduleIdentifier="1" ScheduleName="test_sched2" InitialModuleSchedule="false" MajorFrameSeconds="${mtf*5}">
        % for p in range(0,partitions+1):		
		<% part_slot = round(mtf*5 / (partitions + 1),4) %>
        <Partition_Schedule PartitionIdentifier="${p}" 
                            PartitionName="p${p}" 
                            PeriodSeconds="${mtf*5}" 
                            PeriodDurationSeconds="${part_slot}"
                        % if (p == 0):
                            SetModuleSchedule="true">
                        % else:
                            SetModuleSchedule="false">
                        % endif
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
    <Partition_HM_Table PartitionIdentifier="${p}" PartitionName="p${p}" PartitionCallback="partition_HM_callback">
        <System_State_Entry Description="Partition is Running" SystemState="2">
            <Error_ID_Action Action="IGNORE" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="COLD_START" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="IGNORE" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="IGNORE" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="IGNORE" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="COLD_START" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="WARM_START" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="COLD_START" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="IDLE" Description="Partition Conf Error" ErrorIdentifier="8"/>
            <Error_ID_Action Action="IDLE" Description="Partition Init Error" ErrorIdentifier="9"/>
            <Error_ID_Action Action="COLD_START" Description="User defined Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="IGNORE" Description="User defined Debug error" ErrorIdentifier="11"/>
            <Error_ID_Action Action="WARM_START" Description="User defined Module APEX Error" ErrorIdentifier="12"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition Execution is Halted" SystemState="3">
            <Error_ID_Action Action="IGNORE" Description="Power Interrupt" ErrorIdentifier="0"/>
            <Error_ID_Action Action="COLD_START" Description="Illegal Instruction" ErrorIdentifier="1"/>
            <Error_ID_Action Action="COLD_START" Description="Segmentation Error" ErrorIdentifier="2"/>
            <Error_ID_Action Action="COLD_START" Description="Unimplemented Error" ErrorIdentifier="3"/>
            <Error_ID_Action Action="IGNORE" Description="Floating Point Error" ErrorIdentifier="4"/>
            <Error_ID_Action Action="COLD_START" Description="Overflow Error" ErrorIdentifier="5"/>
            <Error_ID_Action Action="COLD_START" Description="Divide by zero" ErrorIdentifier="6"/>
            <Error_ID_Action Action="COLD_START" Description="I/O Error" ErrorIdentifier="7"/>
            <Error_ID_Action Action="COLD_START" Description="User defined Stack error" ErrorIdentifier="10"/>
            <Error_ID_Action Action="IGNORE" Description="User defined Debug error" ErrorIdentifier="11"/>
            <Error_ID_Action Action="WARM_START" Description="User defined APEX Error" ErrorIdentifier="12"/>
            <Error_ID_Action Action="WARM_START" Description="User defined Deadline Miss" ErrorIdentifier="13"/>
            <Error_ID_Action Action="COLD_START" Description="User defined App Hardware Error" ErrorIdentifier="14"/>
        </System_State_Entry>
        <System_State_Entry Description="Partition is Ready but not Running" SystemState="4">
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
