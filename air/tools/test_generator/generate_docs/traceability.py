import sys
sys.path.append('../../TOOLS')

from req import *
from test import *

def genTraceability (Requirements, TestDefs, htmlFilename = "traceability.html"):

	v_dict = dict(
		Requirements = Requirements,
		TestDefs = TestDefs)

	mytemplate = Template(
			filename="traceability.html.mako",
			input_encoding='utf-8',
			output_encoding='utf-8',
			default_filters=['decode.utf8'],
			encoding_errors='replace')

	file_ctx = mytemplate.render(**v_dict)

	outfile = open(htmlFilename, "w")
	outfile.write(file_ctx)


#testDefs is a list of tests
def printTestList(testDefs, level = 0):
	for testname in testDefs.keys():
		if testDefs[testname]:
			print "entry"
                        outstr = testDefs[testname].nametag + ":\t"
			outstr += testDefs[testname].name
			print outstr

#init execution
# requirements, sectionNames = parseReqXml("reqs.xml")
# testDefs = processReqTests(requirements)

# genTraceability(requirements, testDefs)





############################################
###   trash code for the mako template

		# %elif req.parents:
		# 	<td style='border-right:0px'>
		# 		Parents:
		# 	</td>
		# 	<td style='border-left:0px'>
		# 		%for parent in req.parents:
		# 		<i>${parent}<i/>
		# 		<br/>
		# 	%endfor
		# 	</td>

		