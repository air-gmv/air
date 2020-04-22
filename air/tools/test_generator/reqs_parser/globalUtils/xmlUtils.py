"""@package xmlUtils
Holds the xml class and all needed xml routines to handle with xml files.
"""
################################################################################
#  Project     : AIRII
#  Date        : 2010/09/09
#  Author      : BRSV
#  Version     : $Revision: 1.3 $
#  Description : This module is used to place definitions for xml manipulation
#  History     : 
#  $Log: xmlUtils.py,v $
#  Revision 1.3  2011/06/14 15:39:08  brsv
#  inclusion of doxygen comments
#
#  Revision 1.2  2011/03/03 15:19:58  brsv
#  *** empty log message ***
#
#  Revision 1.3  2010/11/03 18:35:44  brsv
#  partitionFilesHandler added. Partition files are now generated (ports, 
#  init and makefile)
#  usr_hm_tables.c file initiated - first phase
#
#  Revision 1.2  2010/10/25 17:33:07  brsv
#  Some minor improvements
#
#  Revision 1.1  2010/10/12 16:27:30  brsv
#  First issue
#
################################################################################

#--------------------------[ IMPORTS ]------------------------------------------

import xml.dom.minidom
from types import ListType       # For ListType test
from globalUtils import sys

#--------------------------[ GLOBAL VARIABLES ]---------------------------------

#--------------------------[ CLASSES IMPLEMENTATION ]---------------------------

class XmlFile:
  """
  This class defines an object to handle XML files.
  """
  path     = None
  contents = None
  
  ############################################################################
  ## Constructor.
  ############################################################################
  def __init__(self, path):
    """
    Create dom document representation from XML file. (Constructor)
    Example:
      xmlFileTmp = XmlFile("filePath") 
    """
    try:
      self.path = path
      self.contents = xml.dom.minidom.parse(self.path)
    except Exception, inst:
      strTmp = "Unable to parse XML file: " + str(path) + "\nError: "+ str(inst)
      self.path = None
      self.contents = strTmp

  ############################################################################
  ## Destructor.
  ############################################################################
  def __del__(self):
    """
    Destroy dom document representation from XML file. (Destructor.
    Automatically called. Explicit call not needed)
    """
    self.contents.unlink()
    self.contents = None
    
  def getNodeElementsFromTag(self, tag):
    """
    Obtain all the elements referring the given tag
    OUTPUT:
      * list of elements
    """
    
    retList   = []
    try:
      retList = self.contents.getElementsByTagName(tag)
    except:
      retList   = []
      print "Unknown Error: %s\n" % sys.exc_info()[1] 

    return retList
    
  #=============================================================================
  # By given a tag name within the contents of an already open XML file, the
  # function returns a list with all the values found within the tag inside the 
  # file, in tuples of ("tagName", "tagValue)", where tagName could be the same
  # or other if is a child node of the initial tag, or returns empty list if 
  # tag does not exist
  #=============================================================================
  def getValueFromTag(self, tag):
    """
    By given a tag name within the contents of an already open XML file, the
    function returns a list with all the values found within the tag inside the 
    file, in tuples of ("tagName", "tagValue)", where tagName could be the same
    or other if is a child node of the initial tag, or returns empty list if 
    tag does not exist.
    FIXME:
    """
    
    retList   = []
    try:
      elemList = self.contents.getElementsByTagName(tag)
      
      if len(elemList) > 0:
        # Cycle through all elements found with this tag
        for elem in elemList:
          # Cycle through all nodes found within tag
          for child in elem.childNodes:
  #          print child
            # If the node found represents another element
            if child.nodeType == 1:
              # cycle through all tags within the given tag
              for tmp in child.childNodes:
                # if it is another tag child
                if tmp.nodeType == 1:
                  for tmpChild in tmp.childNodes:
                    # If it is another tag node
                    if tmpChild.nodeType == 1:
                      "FIXME: It is already a level to low... create a "\
                      "recursion manner to continue append the next results"
                      retList.append((tmpChild.tagName, "..."))
                    # Validate if text is valid
                    elif tmpChild.data.find("\n\t") == -1:
                      retList.append((tmp.tagName, tmpChild.data))
                # Validate if text is valid
                elif tmp.data.find("\n\t") == -1:
                  retList.append((child.tagName, tmp.data))
            # If node represents only text append the text within if it's valid
            elif (child.nodeType == 3 and child.data.find("\n\t") == -1):
              retList.append((tag, child.data))
    except:
      retList   = []
      print "Unknown Error: %s\n" % sys.exc_info()[1] 

    return retList
  
  
  #=============================================================================
  # Check if an attribute exists in file contents
  #=============================================================================
  def hasAttribName(self, tagName, attrName, attrValue):
    """
    Check if an attribute exists in file contents
    """
    
    try:
      elemList = self.contents.getElementsByTagName(tagName)
      
      for elem in elemList:
        if elem.hasAttribute(attrName) == True:
          if elem.getAttribute(attrName) == attrValue:
            return True
        
    except:
      return False
    
    return False
  
  #=============================================================================
  # getElemByAttrName
  #=============================================================================
  def getElemByAttrName(self, tagName, attrName, attrValue):
    """
    Get the element if an attribute exists in file contents
    """
    
    try:
      elemList = self.contents.getElementsByTagName(tagName)
      
      for elem in elemList:
        if elem.hasAttribute(attrName) == True:
          if elem.getAttribute(attrName) == attrValue:
            return elem
    except:
      return None
    
    return None
  
  #=============================================================================
  # Sets an attribute value if exists
  #=============================================================================
  def setAttrValue(self, tagName, attrName, attrValue):
    """
    Sets an attribute value if exists
    """
    
    resValue = True
    
    try:
      elemList = self.contents.getElementsByTagName(tagName)
        
      if elemList[0].hasAttribute(attrName) == True:
        elemList[0].setAttribute(attrName, attrValue)
        
      else:
        print 'No attribute "%s=%s" found in this document' \
              % (attrName, attrValue)
        resValue = False
        
    except:
      resValue = False
      
    return resValue
  
  #=============================================================================
  # Search in the element (Node) for the attribute with the given value and 
  # return the result of the search
  #=============================================================================
  def hasAttrValueInElem(self, elem, attrName, attrValue):
    """
    Search in the element (Node) for the attribute with the given value and 
    return the result of the search
    """
    
    resValue = False
    
    try:
      if elem.hasAttribute(attrName) == True:
        if elem.getAttribute(attrName) == attrValue:
          resValue = True
      
    except:
      print "ERROR: %s" % sys.exc_info()[1]
      resValue = False
      
    return resValue
  
  #=============================================================================
  # Set in the element pass as argument the given attribute
  #=============================================================================
  def setElemAttrValue(self, elem, attrName, attrValue):
    """
    Set in the element pass as argument the given attribute
    """
    
    resValue = True
    
    try:
      # This is possible because python keep the elements by reference
      elem.setAttribute(attrName, attrValue)
      
    except:
      print "ERROR: %s" % sys.exc_info()[1]
      resValue = False
      
    return resValue
    
  #=============================================================================
  # Remove the element from the xml contents with a specific attribute
  #=============================================================================
  def removeElemByAttr(self, tag, attrName, attrValue):
    """
    Remove the element from the xml contents with a specific attribute
    """
    
    found = False
    try:
      elemList = self.contents.getElementsByTagName(tag)
      
      for elem in elemList:
        # Search for the right element
        if elem.hasAttribute(attrName) == True:
          if elem.getAttribute(attrName) == attrValue:
            # Remove element
            elem.parentNode.removeChild(elem)
            print 'Element "%s" with attribute "%s" = "%s" removed' \
                  % (tag, attrName, attrValue)
            found = True
    except:
      print sys.exc_info()[1]
      return False
    
    if found == False:
      print 'The element "%s" with attribute "%s" = "%s" was not found' \
            % (tag, attrName, attrValue)
    
    return True
  
# END OF CLASS DEFINITION

#--------------------------[ FUNCTIONS IMPLEMENTATION ]-------------------------

################################################################################
## This function verifies if a parameter value within the specified format as
## defined by attrValueSlice exists in the nodesList.
################################################################################
def getAttributeValueFromNodesList(nodesList, attrName, attrValueSlice):
    """
    This obtains an attribute value/name from a list of XML nodes.
    Example:
      retList = getAttributeValueFromNodesList(nodeList, "AttrName", 
      "AttrValueSlice")
    """
    retList = []    
    list = nodesList.split(" ")
      
    # cycle splitted list
    for i in range(0, len(list)):
      # check if any item of splitted list contains the slice of the
      # attribute value
      if list[i].find(attrValueSlice) != -1 and list[i].find(attrName) != -1:
        newList = list[i].split('=')
        name = newList[0]
        value = newList[1]
        retList.append((name, value))

    # Check if any evidence was found
    if retList == []:
      str = "No attribute named: " + attrName + " with value slice: "\
            "" + attrValueSlice + " found in the nodesList argument"
      retList.append(str)

    return retList

################################################################################
## This function obtains a list of values of the specified attributes contained
## in the list (node/string). The list returned has the attribute name previous
## to the values found.
################################################################################
def getAttrsValueFromNodesList(list, attributeTag):
    """
    This function obtains a list of values of the specified attributes contained
    in the list (node/string). The list returned has the attribute name previous
    to the values found.
    Ex:
    retList = getAttrsValueFromNodesList(list, ["AttributeName1", 
    "AttributeName2"]):
    retList = [AttributeName1, value1, value2, value3,
               AttributeName2, value1, value2]
    """
    
    # Definitions
    retList       = []
    paramList     = []
    initSplitTag  = ""
    endSplitTag   = "\""
    
    # Cycle the wanted attributes
    for z in range(0, len(attributeTag)):      
      paramList = [] # reset parameters list for values for the next parameter      
      paramList.append(attributeTag[z])
      
      # append characteres for the split function
      initSplitTag = attributeTag[z] + '=\"'
            
      # Cycle all list nodes
      for y in range(0, len(list)):                
        nodeEvidence = list[y].evidence
        
        # Search for attribute in evidence
        if nodeEvidence.find(attributeTag[z]) != -1:
          
          # return value for attribute in question between tags
          attrVal = nodeEvidence.split(initSplitTag)[1].split(endSplitTag)[0]
          # append value in the list of parameter values          
          paramList.append(attrVal)
            
        else:
          attrVal = 'No value for \"%s\" found in nodeList %s!'\
                     %(attributeTag[z], nodeEvidence)          
          paramList.append(attrVal)   
          
      # Append evidence in return list
      retList += paramList
   
    # Check if any evidence was found
    if retList == []:
      str = "None of the specified attributes was found in nodesList parameter."
      retList.append(str)
   
    return retList
  
#===============================================================================
# This functions starts to parse the string, validating if is a xml string 
# well formed, then obtain the attribute values that are given as input
#===============================================================================
def getAttributesFromXmlString(xmlString, attributesToGet):
  """
  This functions starts to parse the string, validating if is a xml string 
  well formed, then obtain the attribute values that are given as input
  
  INPUT:
    * attributesToGet - A list with all the attributes that needed to obtained 
                        if they exist
  OUTPUT:
    * retList         - A list of tuples (AttributeName, AttributeValue) 
                        sorted by order of attributes given, for example:
                        [(<AttributeName>, <AttributeValue>)]
  """
  
  retList   = []
  resValue  = True
  
  # Obtain first the tag name of the string to get the elements from
  try:
    docTypeObject = xml.dom.minidom.parseString(xmlString)
    
    for attr in attributesToGet:
      # Append a tuple like (attributeName, attribueValue)
      retList.append((attr, docTypeObject.documentElement.getAttribute(attr)))
    
  except Exception, inst:
    print inst
    strTmp = "Unable to parse XML string: %s" % xmlString
    retList.append(strTmp)
    resValue = False
    
  return resValue, retList
  
#===============================================================================
# Obtain the attributes from tag specified for a given file path
#===============================================================================
def getAttrValuesFromTag(xmlFilePath, tagName, attrList):
  """
  Obtain the attributes from tag specified for a given xml file path
  
  Input:
    * - xmlFilePath = Path for the file to analyze
    
    * - attrList    = A list containing the attribute names to obtain from every 
                      elements found with the tagName specified.
  Output:
    * - A list containing other lists like [tagName, ticketValues1, 
        ticketValues2, ...]
        # Each element of the list, except the tagName are in the form:
        ticketValues1 = [(attrName, attrValue), (attrName, attrValue)]
        ...
        In case the attribute specified does not exist the tuple will become as:
        (attrName, "")
        so the attribute value is encouraged to validate.
        
    * - resValue (True, False): In case of error, obtain the explanation from 
                                the retList first position.
                                
    NOTE: retList never returns empty!
  """
  
  retList = []
  resValue = True
  
  # Open file
  inFile = XmlFile(xmlFilePath)
  
  if inFile.path != None:
    
    if attrList != []:
      try:
        # Obtain all the tags found in the file
        elemList = inFile.contents.getElementsByTagName(tagName)
        
        if elemList != []:
          # Append in the first position the tagName
          retList.append(tagName)
          # Cycle through all nodes found for the tag specified
          for elem in elemList:
            tagList = []
            # Cycle through all attributes given as input
            for attr in attrList:
              tagList.append((attr, elem.getAttribute(attr)))
            # append the temp list just for one ticket found for this tag  
            retList.append(tagList)
        else:
          retList.append("FAIL: No elements were found for tag = \"%s\""\
                         % tagName)
          resValue = False
          
      except:
        retList = []
        retList.append("FAIL EXCEPTION: %s -> %s" % (sys.exc_info()[0], 
                                                     sys.exc_info()[1]))
        resValue = False
    else:
      retList.append("FAIL: No attributes were defined, please define at "\
                     "least one attribute")
      resValue = False
  else:
    # Unable to open file
    retList.append("FAIL: %s" % inFile.contents)
    resValue = False
    
  return resValue, retList


#===============================================================================
# This functions starts to parse the string, validating if is a xml string 
# well formed, then obtain the attribute values and setting the new values 
# given as input
#===============================================================================
def setAttrValueToXmlString(xmlString, attrToSet):
  """  
  This functions starts to parse the string, validating if is a xml string 
  well formed, then obtain the attribute values and setting the new values 
  given as input
  
  attrToSet  - A list of Tuple of (<attribute name>, <value to change>)
               ex: [(attrX, ValueY), (attrZ, ValueW)]
  """
  
  retLine   = ""
  resValue  = True
  
  # Obtain first the tag name of the string to get the elements from
  try:
    docTypeObject = xml.dom.minidom.parseString(xmlString)
    
    if len(attrToSet) > 0 and (isinstance(attrToSet, ListType) == True):
    
      for attr in attrToSet:
        
        value = docTypeObject.documentElement.getAttribute(attr[0])
        
        if value != "":
          docTypeObject.documentElement.setAttribute(attr[0], attr[1])
        else:
          resValue &= False
          retLine = "Unable to find attribute \"%s\"" % attr[0]
      
      if resValue == True:
        retLine = docTypeObject.documentElement.toxml()
      
    else:
      resValue  = False
      retLine   = "Please define the attributes list to set new value"
    
  except Exception, inst:
    print inst
    retLine = "Unable to parse XML string: %s" % xmlString
    resValue = False
    
  return resValue, retLine

# FILE END