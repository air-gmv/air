<!-- Test description file -->
<!-- Test steps must be sequential, starting at 001 -->
<!-- Repetitions are limited to 0xFFF,i.e. 4095		-->
<!-- Major time frame [seconds]						-->
<!-- Test End partition : partition where the test
	 will end and finish - if needed, this can be 
	 adapted										-->
<TDF>
	<!-- Test Definition -->
	<test 	name="${name}"
			id="${id}" 
			partitions="${partitions}"
			test_end_partition="0"
			repeat="1" 
			mtf="1.5"
			ms_per_tick="10"
			type="TEST_GMV_VAL"
			personality="RTEMS5">
		<description>
			${description}
		</description>
		<requirements>
			% for r in requirements:
			<req name="${r}" />	
			% endfor
		</requirements>
		
		<!-- Test Procedure -->
		<testprocedure repeat="1">
			% for p in range(0,partitions):
			<teststep id="${p}" partition="${p}" repeat="1">
				<description>
					...
				</description>
			</teststep>
			% endfor
			<teststep id="${p+1}" partition="0" repeat="1">
				<description>
					Terminate test.
				</description>
			</teststep>			
			
		</testprocedure>		
	</test>	<!-- Test id="${id}" -->
</TDF>