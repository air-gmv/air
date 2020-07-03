<!-- 
<!DOCTYPE html>
<html lang="en-US">
	<head>
		<meta meta charset='utf-8'>
		<style type='text/css'>
			* { font-family: verdana; font-size:12px; }
			table {width:550px; border:1px solid #000; border-collapse:collapse;}
			table td {border:1px solid #000; padding:2px; vertical-align:top}
			table .title { background: #D9D9D9; font-weight:bold }
			table .subtitle { background: #F2F2F2; }
			table tr:last-child td {border-bottom:0px}
		</style>
		<title></title>
	</head> -->

<%
	if schedules:
		scheduleString = ''
		for schedule in schedules:
			for partition in schedule['partitions']:
				auxStr = 'P' + str(partition['id']) + ': ' + str(partition['start_ms']) + '-' + str(int(partition['start_ms']) + int(partition['duration_ms'])) + '  '
				scheduleString += auxStr
			scheduleString += '<br/>'
	else:
		scheduleString = 'Default'
%>

<table>
	<tr>
		<td class='title'>${nametag}</td>
		<td class='title' style='width:75%;' colspan='2'>${name}</td>
	</tr>
	<tr>
		<td class='subtitle'>Description</td>
		<td class='subtitle' colspan='2'>${description}</td>
	</tr>

	<tr>
		<td rowspan='4' class='subtitle'>Environment </td>
		<td>Partitions</td> <td>${partitions + " + 1"}</td>
	</tr>
	<tr>
		<td>MTF</td> <td>${mtf}</td>
	</tr>
	<tr>
		<td>Schedule</td> <td>${scheduleString}</td>
	</tr>
	<tr>
		<td>ms_per_tick</td> <td>${ms_per_tick}</td>
	</tr>

	<tr>
		<td class='subtitle'>Input</td>
		%if input:
		<td colspan='2'>${input}</td>
		%else:
		<td colspan='2'>N/A</td>
		%endif
	</tr>
	<tr>
		<td class='subtitle'>Expected Output</td>
		%if output:
		<td colspan='2'>${output}</td>
		%else:
		<td colspan='2'>N/A</td>
		%endif
	</tr>
	<tr>
		<td class='subtitle'>Expected Faults</td>
		%if faults:
		<td colspan='2'>${faults}</td>
		%else:
		<td colspan='2'>N/A</td>
		%endif
	</tr>
	</table>


	<table style='border-top:0px'>
	<tr>
		<td class='title' colspan='3' style='border-top:0px'>Conditions to validate / Truth Table</td>
	</tr>
	
	<%newConditions(validation['initial'], validation['conditions'])%>

	<table style='border-top:0px'>
	<tr>
		<td class='title' colspan='3' style='border-top:0px'>Test Steps</td>
	</tr>

	<%
	currPartition = -1
	for teststep in teststeps:
		if int(teststep['partition']) != currPartition:
			newPartition(int(teststep['partition']))
			currPartition = int(teststep['partition'])
		newStep(teststep['id'], teststep['repeat'], teststep['description'])
	%>
	</table>
</table>





### => Initial Conditions and Validation Conditions

<%def name="newConditions(initial_conditions, conditions)">

	<tr>
			<td class='subtitle' style='width:30%'>Initial Conditions</td>
			%if initial_conditions:
			<td class='subtitle' colspan='2'>${"<br>".join(initial_conditions)}</td>
			%else:
			<td class='subtitle' colspan='2'> N/A </td>
			%endif
	</tr>
</table>
%if conditions:
<table style='border-top:0px'>
	<tr>
			<td style='border-top:0px' class='subtitle' colspan='2'>Action / Condition</td>
			<td style='border-top:0px;width:30%' class='subtitle'>Expected Result</td>
	</tr>
	%for condition in  conditions:
	<tr>
			<td colspan='2'>${condition[0]}</td>
			<td>${condition[1]}</td>
	</tr>
	%endfor
	%endif
</table>
</%def>


### => New teststep

<%def name="newStep(id, repeat, description)">
<tr>
	<td class='subtitle' style='width:5%'>${str(id)}</td>
	<td>${description}</td>
</tr>
</%def>

### => New partition in teststeps

<%def name="newPartition(id)">
<tr>
	<td class='subtitle' colspan='2'>Partition ${id}</td>
</tr>
</%def>