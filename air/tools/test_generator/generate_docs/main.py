#!/usr/bin/python

import sys
sys.path.append('../../TOOLS')

from req import *
from test import *
from traceability import *
#from common.utils import cprint

if len(sys.argv) ==  2:
    argname = sys.argv[1]
else:
    print "Syntax: main.py <requirement xml file>"
    error ("Invalid command line!")     
    exit(-1)

#Fetch requirements and Section names list
print("Processing Requirements...", 'green')
requirements, sectionNames = parseReqXml(argname)

print("Processing Tests...", 'green')
testDefs = processReqTests(requirements, 'tests/')

#generate requirements and tests tables
print("Creating Requirements_Tables.html...", 'green')
reqsHtml = req2Html(requirements, sectionNames, testDefs)
outfile = open("Requirements_Tables.html", "w")
outfile.write(reqsHtml)

#generate traceabiltity matrix
genTraceability(requirements, testDefs, "traceability.html")

#count num of parsed reqs
print(countReqs(requirements), 'green')

#print list of assigned tests
printTestList(testDefs)