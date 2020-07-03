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
	</head>


	<table>
		<tr>
			<td class='title'>Requirement</td>
			<td class='title' style='width:60%;' colspan='2'>Tests / Child Requirements</td>
		</tr>

%for req in Requirements:
	%if req.tests:
		<tr>
			%if req.ParentOf and req.related:
			<td class='subtitle' rowspan='${len(req.tests) + 2 }'>
			%elif bool(req.related) != bool(req.ParentOf):
			<td class='subtitle' rowspan='${len(req.tests) + 1 }'>
			%else:
			<td class='subtitle' rowspan='${len(req.tests)}'>
			%endif
				<b>${req.nametag}</b>
				<br/>
				${req.name}
			</td>
			<%newTestRows(req.tests[0], TestDefs)%>
			
		</tr>
		%for test in req.tests[1:]:
		<tr>
			<%newTestRows(test, TestDefs)%>
			%endfor
		</tr>
		%if req.ParentOf:
			<tr>
				<td style='border-right:0px'>
					Child Requirements:
				</td>
				<td style='border-left:0px'>
					%for ParentOf in req.ParentOf:
					<i>${ParentOf}<i/>
					<br/>
					%endfor
		%endif
		%if req.related:
			%if req.ParentOf:
				</tr>
			%endif
			<tr>
				<td style='border-right:0px'>
					Related Requirements:
				</td>
				<td style='border-left:0px'>
					%for related in req.related:
					<i>${related}<i/>
					<br/>
					%endfor
		%endif
	%else:
		<tr>
			<td class='subtitle'>
				<b>${req.nametag}</b>
				<br/>
				${req.name}
			</td>
		%if req.ParentOf:
			<td style='border-right:0px'>
				Child Requirements:
			</td>
			<td style='border-left:0px'>
				%for ParentOf in req.ParentOf:
				<i>${ParentOf}<i/>
				<br/>
				%endfor
		%elif req.method and 'Testing' not in req.method:
		 	<td colspan='2'>Requirement fullfilled by ${req.method}</td>
		%else:
			<td colspan='2'>No tests assigned</td>
		%endif
		</tr>
	%endif
%endfor

	</table>


<%def name="newTestRows(test, TestDefs)">
	<td>
		${test['name']}
	</td>
	<td>
		%if TestDefs[test['name']]:
		${TestDefs[test['name']].name}
		%else:
		${test['description']}
		%endif
	</td>
</%def>




