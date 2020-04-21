<?xml version="1.0" encoding="UTF-8"?> 
<ARINC_653_Module xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" ModuleName="${name}"> 
	
	<!-- =========================================================================================
	System Health Monitor Table
	========================================================================================= -->	
	<System_HM_Table>
		<System_State_Entry SystemState="0" Description="Module Initialization">
			<Error_ID_Level ErrorIdentifier="1"  Description="Configuration Error" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="2"  Description="Module Config Error" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="5"  Description="Segmentation Error" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="7"  Description="Illegal Instruction" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="8"  Description="Divide by zero" 		ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="9"  Description="Overflow Error" 		ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="10" Description="Floating Point Error" ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="12" Description="Stack error" 			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="14" Description="Power Interrupt"	 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="15" Description="Debug Error" 			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="16" Description="I/O Error" 			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="17" Description="Double Fault"			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="18" Description="Violation Error" 		ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="19" Description="Device Error" 		ErrorLevel="MODULE"/>
		</System_State_Entry>
		<System_State_Entry SystemState="1" Description="PMK Execution">
			<Error_ID_Level ErrorIdentifier="1"  Description="Configuration Error" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="2"  Description="Module Config Error" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="5"  Description="Segmentation Error" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="7"  Description="Illegal Instruction" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="8"  Description="Divide by zero" 		ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="9"  Description="Overflow Error" 		ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="10" Description="Floating Point Error" ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="12" Description="Stack error" 			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="14" Description="Power Interrupt"	 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="15" Description="Debug Error" 			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="16" Description="I/O Error" 			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="17" Description="Double Fault"			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="18" Description="Violation Error" 		ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="19" Description="Device Error" 		ErrorLevel="MODULE"/>
		</System_State_Entry>
		<System_State_Entry SystemState="2" Description="Partition Initialization">
			<Error_ID_Level ErrorIdentifier="3"  Description="Partition Conf Error" ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="4"  Description="Partition Init Error" ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="5"  Description="Segmentation Error" 	ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="7"  Description="Illegal Instruction" 	ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="8"  Description="Divide by zero" 		ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="9"  Description="Overflow Error" 		ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="10" Description="Floating Point Error" ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="12" Description="Stack error" 			ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="14" Description="Power Interrupt"	 	ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="15" Description="Debug Error" 			ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="16" Description="I/O Error" 			ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="17" Description="Double Fault"			ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="18" Description="Violation Error" 		ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="19" Description="Device Error" 		ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="20" Description="APEX Error" 			ErrorLevel="PARTITION"/>			
		</System_State_Entry>
		<System_State_Entry SystemState="3" Description="Partition Operating System">
			<Error_ID_Level ErrorIdentifier="5"  Description="Segmentation Error" 	ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="6"  Description="Deadline Miss" 		ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="7"  Description="Illegal Instruction" 	ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="8"  Description="Divide by zero" 		ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="9"  Description="Overflow Error" 		ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="10" Description="Floating Point Error" ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="12" Description="Stack error" 			ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="13" Description="App Hardware Error" 	ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="14" Description="Power Interrupt"	 	ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="15" Description="Debug Error" 			ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="16" Description="I/O Error" 			ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="17" Description="Double Fault"			ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="18" Description="Violation Error" 		ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="19" Description="Device Error" 		ErrorLevel="PARTITION"/>
			<Error_ID_Level ErrorIdentifier="20" Description="APEX Error" 			ErrorLevel="PARTITION"/>
		</System_State_Entry>		
		<System_State_Entry SystemState="4" Description="Partition Application">
			<Error_ID_Level ErrorIdentifier="5"  Description="Segmentation Error" 	ErrorLevel="PROCESS" ErrorCode="MEMORY_VIOLATION"	/>
			<Error_ID_Level ErrorIdentifier="6"  Description="Deadline Miss" 		ErrorLevel="PROCESS" ErrorCode="DEADLINE_MISSED"	/>
			<Error_ID_Level ErrorIdentifier="7"  Description="Illegal Instruction" 	ErrorLevel="PROCESS" ErrorCode="ILLEGAL_REQUEST"	/>
			<Error_ID_Level ErrorIdentifier="8"  Description="Divide by zero" 		ErrorLevel="PROCESS" ErrorCode="NUMERIC_ERROR"		/>
			<Error_ID_Level ErrorIdentifier="9"  Description="Overflow Error" 		ErrorLevel="PROCESS" ErrorCode="NUMERIC_ERROR"		/>
			<Error_ID_Level ErrorIdentifier="10" Description="Floating Point Error" ErrorLevel="PROCESS" ErrorCode="NUMERIC_ERROR"		/>
			<Error_ID_Level ErrorIdentifier="11" Description="Application Error" 	ErrorLevel="PROCESS" ErrorCode="APPLICATION_ERROR"	/>
			<Error_ID_Level ErrorIdentifier="12" Description="Stack error" 			ErrorLevel="PROCESS" ErrorCode="STACK_OVERFLOW"		/>
			<Error_ID_Level ErrorIdentifier="13" Description="App Hardware Error" 	ErrorLevel="PROCESS" ErrorCode="HARDWARE_FAULT"		/>
			<Error_ID_Level ErrorIdentifier="14" Description="Power Interrupt"	 	ErrorLevel="PROCESS" ErrorCode="POWER_FAILURE"		/>
			<Error_ID_Level ErrorIdentifier="15" Description="Debug Error" 			ErrorLevel="PROCESS" ErrorCode="ILLEGAL_REQUEST"	/>
			<Error_ID_Level ErrorIdentifier="16" Description="I/O Error" 			ErrorLevel="PROCESS" ErrorCode="HARDWARE_FAULT"		/>
			<Error_ID_Level ErrorIdentifier="17" Description="Double Fault"			ErrorLevel="PROCESS" ErrorCode="ILLEGAL_REQUEST"	/>
			<Error_ID_Level ErrorIdentifier="18" Description="Violation Error" 		ErrorLevel="PROCESS" ErrorCode="MEMORY_VIOLATION"	/>
			<Error_ID_Level ErrorIdentifier="19" Description="Device Error" 		ErrorLevel="PROCESS" ErrorCode="HARDWARE_FAULT"		/>
			<Error_ID_Level ErrorIdentifier="20" Description="APEX Error" 			ErrorLevel="PROCESS" ErrorCode="APPLICATION_ERROR"	/>
		</System_State_Entry>		
		<System_State_Entry SystemState="5" Description="Error Handler">
			<Error_ID_Level ErrorIdentifier="5"  Description="Segmentation Error" 	ErrorLevel="PROCESS" ErrorCode="MEMORY_VIOLATION"	/>
			<Error_ID_Level ErrorIdentifier="6"  Description="Deadline Miss" 		ErrorLevel="PROCESS" ErrorCode="DEADLINE_MISSED"	/>
			<Error_ID_Level ErrorIdentifier="7"  Description="Illegal Instruction" 	ErrorLevel="PROCESS" ErrorCode="ILLEGAL_REQUEST"	/>
			<Error_ID_Level ErrorIdentifier="8"  Description="Divide by zero" 		ErrorLevel="PROCESS" ErrorCode="NUMERIC_ERROR"		/>
			<Error_ID_Level ErrorIdentifier="9"  Description="Overflow Error" 		ErrorLevel="PROCESS" ErrorCode="NUMERIC_ERROR"		/>
			<Error_ID_Level ErrorIdentifier="10" Description="Floating Point Error" ErrorLevel="PROCESS" ErrorCode="NUMERIC_ERROR"		/>
			<Error_ID_Level ErrorIdentifier="11" Description="Application Error" 	ErrorLevel="PROCESS" ErrorCode="APPLICATION_ERROR"	/>
			<Error_ID_Level ErrorIdentifier="12" Description="Stack error" 			ErrorLevel="PROCESS" ErrorCode="STACK_OVERFLOW"		/>
			<Error_ID_Level ErrorIdentifier="13" Description="App Hardware Error" 	ErrorLevel="PROCESS" ErrorCode="HARDWARE_FAULT"		/>
			<Error_ID_Level ErrorIdentifier="14" Description="Power Interrupt"	 	ErrorLevel="PROCESS" ErrorCode="POWER_FAILURE"		/>
			<Error_ID_Level ErrorIdentifier="15" Description="Debug Error" 			ErrorLevel="PROCESS" ErrorCode="ILLEGAL_REQUEST"	/>
			<Error_ID_Level ErrorIdentifier="16" Description="I/O Error" 			ErrorLevel="PROCESS" ErrorCode="HARDWARE_FAULT"		/>
			<Error_ID_Level ErrorIdentifier="17" Description="Double Fault"			ErrorLevel="PROCESS" ErrorCode="ILLEGAL_REQUEST"	/>
			<Error_ID_Level ErrorIdentifier="18" Description="Violation Error" 		ErrorLevel="PROCESS" ErrorCode="MEMORY_VIOLATION"	/>
			<Error_ID_Level ErrorIdentifier="19" Description="Device Error" 		ErrorLevel="PROCESS" ErrorCode="HARDWARE_FAULT"		/>
			<Error_ID_Level ErrorIdentifier="20" Description="APEX Error" 			ErrorLevel="PROCESS" ErrorCode="APPLICATION_ERROR"	/>
		</System_State_Entry>	
		<System_State_Entry SystemState="6" Description="Health Monitor Processing">
			<Error_ID_Level ErrorIdentifier="5"  Description="Segmentation Error" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="7"  Description="Illegal Instruction" 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="8"  Description="Divide by zero" 		ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="9"  Description="Overflow Error" 		ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="10" Description="Floating Point Error" ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="12" Description="Stack error" 			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="14" Description="Power Interrupt"	 	ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="15" Description="Debug Error" 			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="16" Description="I/O Error" 			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="17" Description="Double Fault"			ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="18" Description="Violation Error" 		ErrorLevel="MODULE"/>
			<Error_ID_Level ErrorIdentifier="19" Description="Device Error" 		ErrorLevel="MODULE"/>
		</System_State_Entry>
	</System_HM_Table >	
	
	<!-- =========================================================================================
	Module Health Monitor Table
	========================================================================================= -->
	<Module_HM_Table >
		<System_State_Entry SystemState="0" Description="Module Initialization">
			<Error_ID_Action ErrorIdentifier="1"  Action="SHUTDOWN"	/>
			<Error_ID_Action ErrorIdentifier="2"  Action="SHUTDOWN"	/>
			<Error_ID_Action ErrorIdentifier="5"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="7"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="8"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="9"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="10" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="12" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="14" Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="15" Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="16" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="17" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="18" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="19" Action="RESET"	/>
		</System_State_Entry>
		<System_State_Entry SystemState="1" Description="PMK Execution">
			<Error_ID_Action ErrorIdentifier="1"  Action="SHUTDOWN"	/>
			<Error_ID_Action ErrorIdentifier="2"  Action="SHUTDOWN"	/>
			<Error_ID_Action ErrorIdentifier="5"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="7"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="8"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="9"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="10" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="12" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="14" Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="15" Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="16" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="17" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="18" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="19" Action="RESET"	/>
		</System_State_Entry>
		<System_State_Entry SystemState="6" Description="Health Monitor Processing">
			<Error_ID_Action ErrorIdentifier="1"  Action="SHUTDOWN"	/>
			<Error_ID_Action ErrorIdentifier="2"  Action="SHUTDOWN"	/>
			<Error_ID_Action ErrorIdentifier="5"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="7"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="8"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="9"  Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="10" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="12" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="14" Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="15" Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="16" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="17" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="18" Action="RESET"	/>
			<Error_ID_Action ErrorIdentifier="19" Action="RESET"	/>
		</System_State_Entry>
	</Module_HM_Table>
	
	<!-- =========================================================================================
	Partition Table
	========================================================================================= -->
	
% for p in range(0,num_partitions):	
	<!-- partition ${p} -->
	<Partition 	PartitionIdentifier="${p}" 
				PartitionName="p${p}" 
				Criticality="LEVEL_A" 
				SystemPartition="false" 
				EntryPoint="test_init">
		<!-- AIR PARTITION EXTENSION -->
		<tnsa:air_partition_ext 
			xmlns:tnsa="PartitionExt"  
			air_partition_personality="RTEMSIMPR"
			% if p == 1:
			test="${test_type}">
			% else:
			test="NONE">
			% endif
			<supervisor>false</supervisor>
			<cache permission="true" data="true" code="true" />
			<tod_sync>false</tod_sync>
			<localtime>${localtime}</localtime>
			<cache_control>true</cache_control>
			<fp_control>true</fp_control>
			<sched_control>false</sched_control>			
		</tnsa:air_partition_ext>
	</Partition>
% endfor
	
	
	<!-- =========================================================================================
	Module Schedules
	========================================================================================= -->	

	<!-- module scheudle 1 -->
	<Module_Schedule  ScheduleIdentifier="0" ScheduleName="test_sched" InitialModuleSchedule="true" MajorFrameSeconds="${mtf}">
% for p in range(0,num_partitions):				
		<Partition_Schedule PartitionIdentifier="${p}" 
							PartitionName="p${p}" 
							PeriodSeconds="${mtf}" 
							PeriodDurationSeconds="${part_slots[p][0]}">
			<Window_Schedule 	WindowIdentifier="${p}01" 
								WindowStartSeconds="${part_slots[p][1]}" 
								WindowDurationSeconds="${part_slots[p][0]}" 
								PartitionPeriodStart="true"/>
		</Partition_Schedule>
% endfor		
	</Module_Schedule>
	
	<!-- =========================================================================================
	Partition Health Monitor Table
	========================================================================================= -->
% for p in range(0,num_partitions):		
	<Partition_HM_Table PartitionIdentifier="${p}" PartitionName="p${p}" PartitionCallback="hm_part_callback">
		<System_State_Entry SystemState="2" Description="Partition Initialization">
			<Error_ID_Action ErrorIdentifier="3"  Description="Partition Conf Error" Action="IDLE"		/>
			<Error_ID_Action ErrorIdentifier="4"  Description="Partition Init Error" Action="IDLE"		/>
			<Error_ID_Action ErrorIdentifier="5"  Description="Segmentation Error" 	 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="7"  Description="Illegal Instruction"  Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="8"  Description="Divide by zero" 		 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="9"  Description="Overflow Error" 		 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="10" Description="Floating Point Error" Action="WARM_START"/>
			<Error_ID_Action ErrorIdentifier="12" Description="Stack error" 		 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="14" Description="Power Interrupt"	 	 Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="15" Description="Debug Error" 		 Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="16" Description="I/O Error" 			 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="17" Description="Double Fault"		 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="18" Description="Violation Error" 	 Action="WARM_START"/>
			<Error_ID_Action ErrorIdentifier="19" Description="Device Error" 		 Action="WARM_START"/>
			<Error_ID_Action ErrorIdentifier="20" Description="APEX Error" 		     Action="WARM_START"/>
		</System_State_Entry>
		<System_State_Entry SystemState="3" Description="Partition Operating System">
			<Error_ID_Action ErrorIdentifier="5"  Description="Segmentation Error" 	 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="6"  Description="Deadline Miss" 	 	 Action="WARM_START"/>
			<Error_ID_Action ErrorIdentifier="7"  Description="Illegal Instruction"  Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="8"  Description="Divide by zero" 		 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="9"  Description="Overflow Error" 		 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="10" Description="Floating Point Error" Action="WARM_START"/>
			<Error_ID_Action ErrorIdentifier="12" Description="Stack error" 		 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="13" Description="App Hardware error" 	 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="14" Description="Power Interrupt"	 	 Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="15" Description="Debug Error" 		 Action="IGNORE"	/>
			<Error_ID_Action ErrorIdentifier="16" Description="I/O Error" 			 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="17" Description="Double Fault"		 Action="COLD_START"/>
			<Error_ID_Action ErrorIdentifier="18" Description="Violation Error" 	 Action="IGNORE"/>
			<Error_ID_Action ErrorIdentifier="19" Description="Device Error" 		 Action="WARM_START"/>
			<Error_ID_Action ErrorIdentifier="20" Description="APEX Error" 		     Action="WARM_START"/>
		</System_State_Entry> 
	</Partition_HM_Table>
% endfor
	
	<!-- =========================================================================================
	Connection Table
	========================================================================================= -->
	<Connection_Table>  
	
	</Connection_Table>
	
	<!-- AIR MODULE EXTENSION -->
	<!-- system memory is in hexadecimal -->
	<air_module_ext xmlns="ModExt" air_ticks_per_second="${tps}" air_system_memory="400000"/>

	
</ARINC_653_Module>  
