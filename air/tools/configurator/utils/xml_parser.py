## @package utils.xml
#  @author pfnf
#  @brief XML utility functions

__author__ = 'pfnf'

import os
import types
import sys

from localization.common import *
from localization.logger import *
from subprocess import Popen, PIPE
from os import path
sys.modules['_elementtree'] = None

## Evil monkey patch hack to include the current line in XMLParser
#   shameless stolen from lxml package :P
#   also binds to of the lib functions as methods of element
from xml.etree.ElementTree import XMLParser
class XMLCustomParser(XMLParser):
    def _start(self, *args, **kwargs):
        element = super(self.__class__, self)._start(*args, **kwargs)
        
        # Add Source Line
        element.sourceline = self._parser.CurrentLineNumber

        # Add xmlParseTag and xmlParseAttribute as methods to element
        element.parse_tag  = types.MethodType(xmlParseTag, element)
        element.parse_attr = types.MethodType(xmlParseAttribute, element)
        element.parse_text = types.MethodType(xmlParseText, element)

        return element
    


## Performs a XML syntactic validation using xmllint
# @param xml_file Path to the XML file
# @param xml_schema Path to the XML schema file
# @param logger Logger to log the validation processor
# @return True if the file is valid, false otherwise
def xmlSchemaValidation(xml_file, xml_schema, logger):

    if not path.isfile(xml_file):
        return False

    try:
        # Call xmllint
        proc = Popen(["xmllint", "--noout", "--nowarning", "--schema", xml_schema, xml_file],
                     stdout = PIPE, stderr = PIPE)

        # Wait for xmllint execution
        proc.wait()
        output = proc.communicate()[1].split(xml_file + ":")
        logger.write(1, 'XMLLINT Schema Validation Output:', LOG_COLOR_BOLD, indent = '  ', subindent = '    ')

        # Check if an error occurred
        if proc.returncode != 0:
            if len(output):

                for line in output[1:]:
                    logger.write(1, COMMON_XMLLINT_ERROR0.format(line), LOGGER_ERROR)
            logger.write(1, 'XML failed the schema validation...', LOGGER_ERROR)

        else:
            logger.write(1, 'XML passed the schema validation!', 'green')
            return True

    except Exception as why:
        logger.write(1, str(why), LOGGER_ERROR)
        return False

    return True

## Parses a XML tag from the XML node
# @param xml XML node tree
# @param tagName XML tag to parse
# @param minRequired Minimum required tags
# @param maxAllowed Maximum allowed tags
# @param logger Logger to output the errors
# @return Parsed value if possible, None otherwise
def xmlParseTag(xml, tag, minRequired, maxAllowed, logger):

    # Try to get the XML tags
    try:
        xmlList = xml.findall(tag)
    except:
        logger.logger.exception("Exception, See details below")
        xmlList = []

    # Check if required element is missing
    if minRequired > 0 and (not xmlList or len(xmlList) < minRequired):
        logger.error(LOG_REQUIRED_TAG_MISSING, tag)
        return []

    if maxAllowed > 0 and xmlList:

        # Check if more elements are available than the required
        if len(xmlList) > maxAllowed:
            logger.warning(LOG_TOO_MANY_TAGS, len(xmlList), tag, maxAllowed)

        # Return list only with the required elements
        xmlList = xmlList[0:maxAllowed]
        if minRequired == 1 and maxAllowed == 1: xmlList = xmlList[0]


    return xmlList

## Parses a XML attribute from the XML node
# @param xml XML node tree
# @param attr XML attribute to parse
# @param definition [Conversion function, Validation function]
# @param required Attribute is required
# @param logger Logger to output the errors
# @param default Default value for the attribute
# @return Parsed value if possible, None otherwise
def xmlParseAttribute(xml, attr, definition, required, logger, default = None):

    converter = definition[0]
    validator = definition[1]

    # Try to parse value from XML
    value = None
    valueStr = None
    try:
        valueStr = xml.attrib.get(attr, None)
        if valueStr is not None: value = converter(valueStr)
    except:
        logger.logger.exception("Exception, See details below")
        value = None

    # Check if required attribute was found
    if valueStr is None and required:
        logger.error(LOG_ATTRIBUTE_MISSING, attr, xml.tag)
        return None

    # Apply default value, if defined
    if valueStr is None and default is not None:
        return default

    # Check if attribute is valid
    if valueStr is not None and callable(validator):
        try:
            valid = validator(value)
        except:
            logger.logger.exception("Exception, See details below")
            valid = False

        # Invalid value
        if not valid:
            if default is None:
                logger.error(LOG_ATTRIBUTE_INVALID_0, xml.sourceline, valueStr, attr)
            else:
                value = default
                logger.warning(LOG_ATTRIBUTE_INVALID_1, xml.sourceline,valueStr, attr, default)

    return value

## Parses a XML attribute from the XML node
# @param xml XML node tree
# @param attr XML attribute to parse
# @param definition [Conversion function, Validation function]
# @param required Attribute is required
# @param logger Logger to output the errors
# @param default Default value for the attribute
# @return Parsed value if possible, None otherwise
def xmlParseText(xml, definition, logger, default = None):

    converter = definition[0]
    validator = definition[1]

    # Try to parse value from XML
    value = None
    valueStr = None
    try:
        valueStr = xml.text
        if valueStr is not None: value = converter(valueStr)
    except:
        logger.logger.exception("Exception, See details below")
        value = None

    # Check if text is valid
    if callable(validator):
        try:
            valid = validator(value)
        except:
            logger.logger.exception("Exception, See details below")
            valid = False

        # Invalid value
        if not valid:
            if default is None:
                logger.error(LOG_TEXT_INVALID_0, xml.sourceline, valueStr, xml.tag)
            else:
                value = default
                logger.warning(LOG_ATTRIBUTE_INVALID_1, xml.sourceline,valueStr, xml.tag, default)

    return value

## Opens a XML file in to a python object
#  @param xmlFile path to the XML file
#  @return Processable XML object, None in case of error
def xmlOpen(xmlFile, logger, xmlSchema = None):

    # Validate against the schema is possible
    #if xmlSchema and not xmlSchemaValidation(xmlFile, xmlSchema, None):
    #	errors.append("File '{0}' failed the XML schema validation".format(xmlFile))

    # Open the xml file
    try:
        from xml.etree import cElementTree
        eTree = cElementTree.parse(xmlFile, parser = XMLCustomParser())
        xml = eTree.getroot()
        # Remove classname from tags nodes
        for xmlNode in xml.getiterator(): xmlNode.tag = xmlNode.tag.split('}', 1)[-1]

    # Handle Exception
    except Exception as why:
        logger.error("Unexpected error opening '{0}': {1}", xmlFile, str(why))
        return None

    return xml

## Performs a XML syntactic validation using xmllint
# @param xml_file Path to the XML file
# @param xml_schema Path to the XML schema file
# @param logger Logger to log the validation processor
# @return True if the file is valid, false otherwise
def xmlSchemaValidation(xml_file, xml_schema, errors):

    if not path.isfile(xml_file):
        return False

    try:
        # Call xmllint
        proc = Popen(["xmllint", "--noout", "--nowarning", "--schema", xml_schema, xml_file],
                     stdout = PIPE, stderr = PIPE)

        # Wait for xmllint execution
        proc.wait()

        # Check if an error occurred
        if proc.returncode != 0:
            return False
        else:
            return True

    except Exception as why:
        return False




