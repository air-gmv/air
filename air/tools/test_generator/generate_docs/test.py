#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
import sys
sys.path.append('../../configurator')

from sys import maxint
from utils.xml_parser import xmlOpen
from utils.logger import Logger1
import re
from mako.template import Template
from utils.temp import LOGGER_ERROR, LOGGER_WARNING



class cTest(object):
	#test is a 
	def __init__(self, test, logger = None):

		if not logger: logger = Logger1()
		self.logger = logger

		attribList = [str, lambda x:True]

		self.nametag = htmlNL(test.parseAttribute('nametag', attribList, True, logger))
		self.name = htmlNL(test.parseAttribute('name', attribList, True, logger))
		self.id = htmlNL(test.parseAttribute('id', attribList, True, logger))
		self.partitions = htmlNL(test.parseAttribute('partitions', attribList, True, logger))
		self.mtf = htmlNL(test.parseAttribute('mtf', attribList, True, logger))
		self.ms_per_tick = htmlNL(test.parseAttribute('ms_per_tick', attribList, True, logger))
		self.description = test.parseTag('description', 0, 1, logger)
		if self.description: self.description = htmlNL("".join(self.description[0].text))
		self.comment = test.parseTag('comment', 0, 1, logger)
		if self.comment: self.comment = htmlNL("".join( comm.text for comm in self.comment))
		self.input = test.parseTag('input', 0, 1, logger)
		if self.input: self.input = "".join( inp.text for inp in self.input)
		self.output = test.parseTag('output', 0, 1, logger)
		if self.output: self.output = htmlNL("".join( outp.text for outp in self.output))
		self.faults = test.parseTag('faults', 0, 1, logger)
		if self.faults: self.faults = htmlNL("".join( faults.text for faults in self.faults))

		#begin processing schedules
		#self.schedules is a list of dicts with 'id'=integer and 
		#'partitions'=list of dicts with 'id','duration_ms' and 'start_ms'
		self.schedules = None
		hasSchedules = test.parseTag('schedules', 0, 1, logger)
		if hasSchedules:
			schedules = test.parseTag('schedules/schedule', 1, maxint, logger)
			for schedule in schedules:
				processedSchedule = dict()
				processedSchedule['id'] = schedule.parseAttribute('id', attribList, True, logger)
				processedSchedule['partitions'] = []
				partitionList = schedule.parseTag('partition', 1, maxint, logger)
				for partition in partitionList:
					processedPartition = dict()
					processedPartition['id'] = partition.parseAttribute('id', attribList, True, logger)
					processedPartition['duration_ms'] = partition.parseAttribute('duration_ms', attribList, True, logger)
					processedPartition['start_ms'] = partition.parseAttribute('start_ms', attribList, True, logger)
					processedSchedule['partitions'].append(processedPartition)
				try:
					self.schedules.append(processedSchedule)
				except:
					self.schedules = [processedSchedule]

		#begin parsing validation steps
		testvalidation = test.parseTag('validation', 0, 1, logger)
		
		self.validation = dict(
				initial = [],
				conditions = []
				)

		if not testvalidation: 
			#logger.write(1, 'No Test Validation tags found', LOGGER_ERROR)
			logger.errorFlag = True
		else:
			for initial_condition in testvalidation[0].parseTag('initial', 0, 1, logger):
				self.validation['initial'].append(initial_condition.parseAttribute('description', attribList, True, logger))
			for condition in testvalidation[0].parseTag('condition', 0, maxint, logger):
				self.validation['conditions'].append([condition.parseAttribute('description', attribList, True, logger), condition.parseAttribute('result', attribList, True, logger)])

		#begin parsing test procedures
		teststeps = test.parseTag('testprocedure/teststep', 0, maxint, logger)
		self.teststeps = []
		if not teststeps: 
			logger.write(1, 'No Test Step tags found', LOGGER_ERROR)
			logger.errorFlag = True
		else:
			for teststep in teststeps:
				newstep = dict(
					id = teststep.parseAttribute('id', attribList, True, logger),
					partition = teststep.parseAttribute('partition', attribList, True, logger),
					repeat = teststep.parseAttribute('repeat', attribList, True, logger),
					description = htmlNL(str.strip( teststep.parseTag('description', 0, 1, logger)[0].text))
					)

				self.teststeps.append(newstep)





	def __str__(self):
		out_str = 30*"=" + "\n"
		out_str += "Nametag: " + str(self.nametag) + "\n"
		out_str += "Name: " + str(self.name) + "\n"
		out_str += "ID: " + str(self.id) + "\n"
		out_str += "Partitions: " + str(self.partitions) + "\n"
		out_str += "MTF: " + str(self.mtf) + "\n"
		out_str += "ms_per_tick: " + str(self.ms_per_tick) + "\n"
		if self.description:
			out_str += "Description: " + self.description + "\n"
		if self.comment:
			out_str += "Comment: " + self.comment + "\n"
		try:
			out_str += "Steps:\n"
			out_str += "ID\tPart.\tDescription\n"
			for test in self.teststeps:
				out_str += str(test['id']) + "\t" + str(test['partition']) + "\t" + \
				str(test['description']) + "\n"
		except: 
			out_str += "TESTSTEP DEFINITION ERROR\n"

		out_str += 30*"=" + "\n"
		return out_str


	def toHtml(self):
                print "toHtml"
		v_dict = dict(
			nametag = str(self.nametag),
			name = self.name,
			id = self.id,
			partitions = self.partitions,
			mtf = self.mtf,
			ms_per_tick = self.ms_per_tick,
			description = self.description,
			schedules = self.schedules,
			input = self.input,
			output = self.output,
			faults = self.faults,
			validation = self.validation,
			teststeps = self.teststeps)

		mytemplate = Template(
			filename="testTable.html.mako",
			input_encoding='utf-8',
			output_encoding='utf-8',
			default_filters=['decode.utf8'],
			encoding_errors='replace')

		file_ctx = mytemplate.render(**v_dict)
		return file_ctx



def htmlNL(istring):
		istring = str.strip(re.sub ('[\t]', '', istring))
		return '<br/>'.join(s for s in istring.split('\n'))



def parseTestXml(xmlFilename):

	logger = Logger1()

	xml = xmlOpen(xmlFilename, logger)
	if logger.haveErrors(): exit()

	xmlTest = xml.parseTag('test', 1, 1, logger)
	test = cTest(xmlTest)
        print "toHt1"
        test.toHtml
	return test





#Example Usage

# test = parseTestXml("tests/TEST-DEF-01370.xml")
# print test
# outfile = open("testHtml.html", "w")
# outfile.write(parseTestXml("tests/TEST-DEF-01370.xml").toHtml())
