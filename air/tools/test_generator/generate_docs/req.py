#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
import sys
sys.path.append('../../configurator')

from utils.logger import *
from utils.xml_parser import *
from sys import maxint
from lxml import etree
from sys import exit
import re
from mako.template import Template
from mako.runtime import Context
from test import *

class cRequirement(object):

	def __init__(self, req, section = None, subsection = None, logger = None):

		if not logger: logger = Logger()
		self.logger = logger

		attribList = [str, lambda x:True]

		self.section = section
		self.subsection = subsection
		self.nametag = htmlNL(xmlParseAttribute(req,'nametag', attribList, True, logger))
		self.name = htmlNL(xmlParseAttribute(req,'name', attribList, True, logger))
		self.method = htmlNL(xmlParseAttribute(req,'method', attribList, True, logger))
		self.priority = htmlNL(xmlParseAttribute(req,'priority', attribList, True, logger))
		self.sima = xmlParseAttribute(req,'sima', attribList, True, logger)
		self.origin = htmlNL(xmlParseAttribute(req,'origin', attribList, True, logger))
		self.description = htmlNL(req.text)
		self.comment = xmlParseTag(req,'comment', 0, 1, logger)
		self.ParentOf = None
		if self.comment: self.comment = htmlNL(self.comment[0].text)


		listParents = None
		for parent in xmlParseTag(req, 'parents/parent', 0, maxint, logger):
			name = htmlNL(xmlParseAttribute(parent, 'name', attribList, True, logger))
			try:
				listParents.append(str.strip(name))
			except:
				listParents = [str.strip(name)]
		self.parents = listParents

		listRelated = None
		for related in xmlParseTag(req, 'parents/related', 0, maxint, logger):
			name = htmlNL(xmlParseAttribute(related, 'name', attribList, True, logger))
			try:
				listRelated.append(str.strip(name))
			except:
				listRelated = [str.strip(name)]
		self.related = listRelated

		#start parsing conditions
		self.conditions = None
		hasConditions = xmlParseTag(req, 'conditions', 0, 1, logger)
		if hasConditions:
			conditions = xmlParseTag(req, 'conditions/condition', 1, maxint, logger)
			for condition in conditions:
				dictConditions = {
				'action': xmlParseAttribute(condition, 'action', attribList, True, logger),
				'result': xmlParseAttribute(condition, 'result', attribList, True, logger),
				'test': xmlParseAttribute(condition, 'test', attribList, True, logger)
				}
				try:
					self.conditions.append(dictConditions)
				except:
					self.conditions = [dictConditions]

		#start parsing tests 
		listTests = None
		#just check the number of <tests> tag
		tests = xmlParseTag(req, 'tests', 0, 1, logger)
		for test in xmlParseTag(req, 'tests/test', 0, maxint, logger):
			name = xmlParseAttribute(test, 'name', attribList, True, logger)
			description = xmlParseAttribute(test, 'description', attribList, True, logger)
			dictTest = dict(name = name, description = str(description))
			try:
				listTests.append(dictTest)
			except:
				listTests = [dictTest]
		self.tests = listTests

	def __str__(self):
		out_str = 30*"=" + "\n"
		out_str += "Nametag: " + self.nametag + "\n"
		out_str += "Name: " + self.name + "\n"
		out_str += "Method: " + self.method + "\n"
		out_str += "Priority: " + self.priority + "\n"
		out_str += "SIMA: " + self.sima + "\n"
		out_str += "Text: " + self.description + "\n"
		if self.comment:
			out_str += "Comment: " + self.comment + "\n"
		out_str += "Origin: " + self.origin + "\n"
		if self.parents:
			out_str += "Parents: " + " ".join(self.parents) + "\n"
		if self.tests:
			out_str += "Tests: " + "\n"
			for test in self.tests:
				out_str += test['name'] 
				if 'description' in test: out_str += ", " + test['description']
				out_str += "\n"
		out_str += "Section: " + str(self.section) + " "
		out_str += "SubSection: " + str(self.subsection) + "\n"
		out_str += 30*"=" + "\n"

		return out_str


		

def req2Html(Requirements, SectionNames = None, TestDefs = None):
	v_dict = dict(
		Requirements 	= Requirements,
		SectionNames 	= SectionNames,
		TestDefs 			= TestDefs)

	mytemplate = Template(
		filename="reqTable.html.mako",
		input_encoding='utf-8',
		output_encoding='utf-8',
		default_filters=['decode.utf8'],
		encoding_errors='replace')

	file_ctx = mytemplate.render(**v_dict)
	return file_ctx



#append a list of the requirements each requirement is displayed as parent
#Requirements is a list of requirements
#Subject is the requirement being analyzed
def processParentOf(Requirements, Subject):

	listParentOf = []

	for req in Requirements:
		if req.parents:
			for parent in req.parents:
				if Subject.nametag in parent or parent in Subject.nametag:
					if parent not in listParentOf:
						try:
							listParentOf.append(req.nametag)
						except:
							listParentOf = [parent]
	Subject.ParentOf = listParentOf




def processReqs(node, section = None, subsection = None, logger = None):
	
	retList = None
	if not logger: logger = Logger()

	for req in xmlParseTag(node, 'req', 0, maxint, logger):
		cReq = cRequirement(req, section, subsection, logger)
		try:
			retList.append(cReq)
		except:
			retList = [cReq]

	return retList



def parseReqXml(xmlFilename):
	logger = Logger()
	xml = xmlOpen(xmlFilename, logger)
	if logger.errors != 0: exit()

	attribList = [str, lambda x:True]

	Requirements = None
	SectionNames = dict()
	SubsectionNames = dict()

	#Requirements is a list of all requirements
	#	- each requirement has a section and subsection identifier which is
	# 	the key for the SectionNames and SubsectionNames dictionaries	
	for sec_index, section in enumerate(xmlParseTag(xml, 'section', 0, maxint, logger)):
		sectionName = xmlParseAttribute(section, 'name', attribList, True, logger)
		listSection = processReqs(section, sec_index, logger = logger)
		if listSection:
			for req in listSection:
				try:
					Requirements.append(req)
				except:
					Requirements = [req]

		SubsectionNames = None
		for subsec_index, subsection in enumerate(xmlParseTag(section, 'subsection', 0, maxint, logger)):
			
			subsectionName = xmlParseAttribute(subsection, 'name', attribList, True, logger)
			listSubsection = processReqs(subsection, sec_index, subsec_index, logger = logger)
			if subsectionName and listSubsection:
				try:
					SubsectionNames[subsec_index] = subsectionName
				except:
					SubsectionNames = {subsec_index: subsectionName}
				for req in listSubsection:
					try:
						Requirements.append(req)
					except:
						Requirements = [req]
		SectionNames[sec_index] = [sectionName, SubsectionNames]

	for req in Requirements:
		processParentOf(Requirements, req)

	return Requirements, SectionNames



def processReqTests(Requirements, Path):

	testDefs = dict()

	for req in [r for r in Requirements if r.tests]:
		
		for test in req.tests:
			try:
				if test['name'] not in testDefs.keys():
                                        print Path + test['name']+'.xml'
					testDefs[test['name']] = parseTestXml(Path + test['name']+'.xml')
			except:
				testDefs[test['name']] = None
	return testDefs




def countReqs(Requirements):
	return str(len(Requirements)) + ' tests successfully processed'




#Example usage
# Init execution

# requirements, sectionNames = parseReqXml("reqs_corrected.xml")
# testDefs = processReqTests(requirements)
# txtHtml = req2Html(requirements, sectionNames, testDefs)
# outfile = open("Requirements_Tables.html", "w")
# outfile.write(txtHtml)

