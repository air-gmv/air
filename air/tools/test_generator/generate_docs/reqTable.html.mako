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


		<%def name="newTable(req, TestDefs, processedTests)">
			<table>
				<tr>
					<td class='title'>${req.nametag}</td>
					<td class='title' style='width:75%;'>
					${req.name}
					</td>
				</tr>
				<tr>
					<td class='subtitle'>${req.method}</td>
					<td class='subtitle' rowspan='3'>
					%if req.description:
						${req.description}
					%endif
					%if req.comment:
						<br>
						<i> ${req.comment}</i> 
					%endif
					</td>
				</tr>
				<tr>
					<td class='subtitle'>
					AIR
					%if req.sima in ["True", "true"]:
						/ SIMA
					%endif
					</td>
				</tr>
				<tr>
				<td class='subtitle'>${req.priority}</td></tr>
				<tr><td>${req.origin}</td>
				<td>
				%if req.parents:
				Parent: ${", ".join(req.parents)}
					%if req.parents and req.related:
						<br/>
					%endif
				%endif
				%if req.related:
				Related: ${", ".join(req.related)}
				%endif
				%if not req.related and not req.parents:
				No Parent/Related Requirements
				%endif
				</td>
				</tr>
			</table>
			%if req.tests:
			<table style='border-top:0px'>
				<tr><td colspan='3' class='title' style='border-top:0px'>TESTS</td></tr>
				<%
				testname = ""
				testdescription = ""
				for test in req.tests:
					testname = test['name']
					if test['name'] in TestDefs.keys() and TestDefs[test['name']]: 
						testdescription = TestDefs[test['name']].name
					elif test['description']:
						testdescription = test['description']
					newTest(testname, testdescription)
					%>
			</table>
			%endif

			%if req.conditions:
			<table style='border-top:0px'>
				<tr><td colspan='3' style='border-top:0px' class='title'>Conditions to Validate / Truth Table</td></tr>
				%for condition in req.conditions:
				<tr>
					<td style='width:30%'>${condition['action']}</td>
					<td>${condition['result']}</td>
					<td>${condition['test']}</td>
				</tr>
				%endfor
			</table>
			%endif

			%if req.tests:
			<%
			for test in req.tests:
				if test['name'] not in processedTests:
					try:
						newTestTable(TestDefs[test['name']].toHtml())
						processedTests.append(test['name'])
					except:
						newTestTable('Error in Test Table ' + test['name'])
			%>
			%endif
		<br>
		</%def>

		<%def name="newTestTable(testHtmlCode)">
			<br>
			${testHtmlCode}
			<br>
		</%def>

		<%def name="newSection(SectionName, SecIndex)">
		<br>
		<br>
		<strong>
			${SecIndex+1}: ${SectionName}
		</strong>
		<br>
		<br>
		</%def>

		<%def name="newSubsection(SubsectionName, SecIndex, SubsecIndex)">
		<br>
		<br>
		<strong>
			${SecIndex+1}.${SubsecIndex+1}: ${SubsectionName}
		</strong>
		<br>
		<br>
		</%def>

		<%def name="newTest(testname, testdescription)">
		<tr><td>${testname}</td><td colspan='2'>${testdescription}</td></tr>
		</%def>

	<body>
		<%
			processedTests = []
			if SectionNames: #we've got sections
				for isection in SectionNames.keys():
					section = SectionNames[isection]
					newSection(section[0], isection)
					#isection is a key but also the index
					if section[1]: 	#we've got subsections
						
						#handle reqs out of subsections
						for req in Requirements:
							if req.section == int(isection) and req.subsection == None:
								newTable(req, TestDefs, processedTests)

						#handle reqs inside subsections
						SubsectionNames = section[1]
						for isubsection in SubsectionNames.keys():
							#isubsection is a key but also the index
							subsection = SubsectionNames[isubsection]
							newSubsection(subsection, isection, isubsection)
							for req in Requirements:
								if req.section == int(isection) and req.subsection == int(isubsection):
									newTable(req, TestDefs, processedTests)

					else: 					#no subsections
						for req in Requirements:
							if req.section == int(isection):
								newTable(req, TestDefs, processedTests)
			else:
				for req in Requirements:
					newTable(req, TestDefs, processedTests)
		%>
	</body>




</html>