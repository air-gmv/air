#!/usr/bin/python
"""@package test
Application test module
"""

################################################################################
#  Project     : AIR
#  Date        : 2020/05/01
#  Author      : JAAC/DTMS
#  Version     : $Revision: 2.0 $
#  Description : This module is used to generate a test framework
#  History     : 
#  $Log: main.py,v $
#  Revision 1.1  2011/10/25 16:30:36  
#  Creation
#
################################################################################

#--------------------------[ IMPORTS ]------------------------------------------

from mako.template import Template
import os
import os.path
import subprocess
import sys
from types import ListType
from xml.dom.minidom import parse

#--------------------------[ Definitions ]--------------------------------------

Syntax = """
  Create test template from test_definition.xml file(s)
  
  Syntax to generates tests with xml and source code template:
      ./testgen.py test_definition_folder
  Syntax to generates tests only xml configuration
      ./testgen.py -xml test_definition_folder

   By selecting a folder it will generate xml for all tesdef* found.
"""

#####  Global constants ######################################################## 
      
# Associative Datastore (key = requirement, value = dictionary of properties)
datastore = {}
#   async_traps   : [int]
#   id            : int
#   ign_err       : [Boolean] - indexed by error identifier
#   name          : string
#   partitions    : int 
#   description   : string
#   repeat        : int
#   mtf           : float
#   p             : current partition
#   main_part     : main test partition (the one that finishes a successful exec)
#   requirements  : [string]
#   sync_traps    : [int]
#   virt_traps    : [int]
#   tps           : ticks per second
#   type          : string (one of VALID_TESTs - see definitions.py)
#   personality   : string (one of VALID OSs - see definitions.py)
#   apex          : string (one of VALID APEXs - see definitions.py)
#   test_steps    : [test_step]
#                 | test_step   ->  id          : int
#                                   partition   : int
#                                   repeat      : int
#                                   description : string

# Partition test files
PX_TESTDEF_H_FILE     = "P%stestdef.h"
PX_TESTDEF_C_FILE     = "P%stestdef.c"
PX_TESTPRNT_C_FILE    = "P%stestprint.c"

# Test definition file (used in reqsparse.py!?!)
TEST_DEF_FILE         = "testdef_%s.xml"

# Main Test File
TESTXML_FILE          = "%s.xml"

# apexes
VALID_APEXs           = ["NONE", "AIRPEX", "IMASPEX"]

# output dir for testdefs_TEST*.xml
TESTGEN_OUTDIR        = os.path.join("testgen", "generated")

# residence of the mako file templates
TEMPLATE_PATH         = "template"

##### Text Functions ###########################################################

def tab(x):
    print '\t' * x,
  
def clean_spaces(text):
    text = text.replace("\n", " ")
    text = text.replace("\t", "")
    text = text.replace("  ", " ")  
    return text
  
# Left align text, split it in line  
def align_paragraph(paragraph, width, tab=0):
    '''
  align paragraph to specified width,
  returns list of paragraph lines
  '''
    lw = 0
    line = []
    result = []
    for word in paragraph.split():
        if lw + len(word) + 1 < width:
            line.append(word)
            lw += len(word) + 1
        else:
            result.append("\t" * tab + " ".join(line))
            lw = 0
            line = []
            line.append(word)
    result.append("\t" * tab + " ".join(line))
    return "\n".join(result)



##### Utility Functions ########################################################

def error (string):
    print "ERROR: " + string
    sys.exit(-1)

def path_join(path_array):
    path = ""
    for p in path_array:
        path = os.path.join(path, p)
    return os.path.abspath(path)



##### MAKO PROCESSING FUNCTIONS ################################################


# process a template file (filename) with the (v_dict) dictionary
# if it is file for a partition, the template file is preceeded by a 'p'
# if the index is >= 0, this should be used in the generation of the output file
def process_file (filename, v_dict, template_folder="", output_folder="", 
                  type="", index=-1):
    """
  Process the generation of a file from a template
    * filename          = file to process
    * v_dict            = dictionary of value to replace
    * template_folder   = path for the templates folder
    * output_folder     = the path to where should be created the output file
    * type              = PARTITION | MAIN | CONFIG | TESTXML
  """
  
    # --------------------------- MAKO PROCESSING -------------------------------
    # get the path for the template file
    if type == "PARTITION":
        if index == -1:
            template_file = os.path.join(template_folder, "p" + filename + ".t")
        else:
            template_file = os.path.join(template_folder, "p" + (filename % "x") + ".t")
    elif type == "MAIN":
        template_file = os.path.join(template_folder, "m" + filename + ".t")    
    elif type == "CONFIG":
        template_file = os.path.join(template_folder, filename + ".t")    
    elif type == "TESTXML":
        template_file = os.path.join(template_folder, "TEST.xml.t")    
    else:
        return False, "Invalid template type"

    # debug: report the template filename
    print template_file.replace(os.getcwd(), "").ljust(40), "=>",
  
    # Mako template load
    try:    
        mytemplate = Template(filename=template_file)
    except IOError:
        return False, \
            "ERROR: Missing template file (%s)" % template_file
    except:
        return False, \
            "ERROR: Error loading template file (%s) : %s" % (template_file, sys.exc_info()[0])
  
    # processing
    file_ctx = mytemplate.render( ** v_dict)
  
    # ------------------------ FILE GENERATION -----------------------------------
  
    # Output file path
    if  type == "PARTITION" and index > -1:
        dstFilePath  = os.path.join(output_folder, filename % str(index))
    else:
        dstFilePath  = os.path.join(output_folder, filename)
  
    # Create the file
    try:
        # delete any previous file that could exist
        if(os.path.exists(dstFilePath)):
            os.remove(dstFilePath);
        # initialise output file
        with open(dstFilePath, 'w') as fdOut:
            # write the context to file
            fdOut.write(file_ctx)
        # Report
        rt = dstFilePath.replace(os.getcwd(), "")
        if rt[0] == "/":
            print rt[1:]
        else:
            print rt
    except:
        return False, sys.exc_info()[1]
    
    return True, "File processed!"



##### XML 2 Datastore Parsing Functions ########################################

# Process each individual test step ########
def process_test_step(node):
    test_step = {}
    # Get test id, name and partition number
    try:
        test_step["id"]         = int(node.getAttribute("id"))
        test_step["partition"]  = int(node.getAttribute("partition"))
        test_step["repeat"]     = int(node.getAttribute("repeat"))
    except:
        error ("XML teststep element does not have all necessary attribute fields")
    # get the test step description
    try:
        for n in node.childNodes:
            if n.nodeType == n.ELEMENT_NODE and n.tagName == "description":
                for e in n.childNodes:
                    if e.nodeType == e.TEXT_NODE and clean_spaces(e.data) > "":
                        test_step_text = align_paragraph(e.data, 70, 1)     
        test_step["description"] = test_step_text
    except:
        error ("XML teststep element does not have a proper description field")
    return test_step
  
# Process the main XML file ################
def process_xml(nodes, datastore):
    # Initialise optional components of the datastore
    datastore["async_traps"] = []
    datastore["sync_traps"] = []
    datastore["virt_traps"] = []
    datastore["ign_err"]    = [False] * 21
    datastore["apex"]       = ""
  
    # Get test element 
    try:
        test_node = nodes.getElementsByTagName("test")[0]
    except:
        error ("XML features no test element")
  
    # Get test id, name and partition number
    try:
        datastore["id"]         = int(test_node.getAttribute("id"))
        datastore["name"] = test_node.getAttribute("name")
        datastore["partitions"] = int(test_node.getAttribute("partitions"))
        datastore["main_part"]  = int(test_node.getAttribute("test_end_partition"))
        datastore["repeat"]     = int(test_node.getAttribute("repeat"))
        datastore["mtf"]        = float(test_node.getAttribute("mtf"))
        datastore["tps"]        = 1000 / int(test_node.getAttribute("ms_per_tick"))
        datastore["type"]       = test_node.getAttribute("type")
        datastore["personality"] = test_node.getAttribute("personality")
    except:
        error ("XML test element does not have all necessary attribute fields")

    # if any, get apex attribute
    apxattr_str = test_node.getAttribute("apex")
    if apxattr_str in VALID_APEXs:
        datastore["apex"] = apxattr_str

    # Get test subtree
    try:
        test_subnodes = test_node.childNodes
    except:
        print "XML features no sub nodes in test node"
        sys.exit(-1)  
  
    # Get test description, requirements and testprocedure
    for node in [n for n in test_subnodes if n.nodeType == n.ELEMENT_NODE]:    
        # Test description
        if    node.tagName == "description":
            datastore["description"] = align_paragraph(node.childNodes[0].data, 76, 1)
        # Test requirements
        elif  node.tagName == "requirements":
            requirements = []
            for n in [n for n in node.childNodes if n.nodeType == n.ELEMENT_NODE and n.tagName  == "req"]:
                requirements.append(n.getAttribute("name"))
            datastore["requirements"] = requirements
        # Test procedure
        elif  node.tagName == "testprocedure":
            test_steps = []
            for ts in [n for n in node.childNodes if n.nodeType == n.ELEMENT_NODE and n.tagName  == "teststep"]:
                test_steps.append(process_test_step(ts))
            datastore["test_steps"] = test_steps
    # Registered Traps
        elif  node.tagName == "registered_traps":   
            # get sync and asyncronous traps
            try:
                datastore["async_traps"] = \
                    map(lambda x: int(x, 16), node.getAttribute("async").split())
                datastore["sync_traps"]  = \
                    map(lambda x: int(x, 16), node.getAttribute("sync").split())
            except:
                print "XML does not specify registered traps (even if empty) : %s" % sys.exc_info()[0]
                sys.exit(-1)
            # now get virtualised traps
            try:
                datastore["virt_traps"] = \
                    map(lambda x: int(x, 16), node.getAttribute("virt").split())
            except:
                pass
            # finally, cycle through the error list to determine which errors to ignore
            # async traps are obsolete ignoring
            #for trap in datastore["async_traps"]:
            #    for i in range(1, len(SPARC_ERROR_MAP)):
            #        if trap in SPARC_ERROR_MAP[i]:
            #            datastore["ign_err"][i] = True
            #for trap in datastore["sync_traps"]:
            #    for i in range(1, len(SPARC_ERROR_MAP)):
            #        if trap in SPARC_ERROR_MAP[i]:
            #            datastore["ign_err"][i] = True


##### Copy file Functions ########################################
def copyFiles(xmlFlag, rootDir, fileName):
   # copy xml files to folder
    try:
        cmdLine = "cp " + TESTGEN_OUTDIR + "/" + fileName[8:][:14] + "/*.xml " + rootDir
        print subprocess.check_output(cmdLine,shell=True,stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))
    
    # copy all files if xmlFlag is false
    if (xmlFlag == False):
        #copy source files
        try:
            cmdLine = "cp -r " + TESTGEN_OUTDIR + "/" + fileName[8:][:14] + "/* " + rootDir
            print subprocess.check_output(cmdLine,shell=True,stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            raise RuntimeError("command '{}' return with error (code {}): {}".format(e.cmd, e.returncode, e.output))
   

################################################################################
## MAIN FUNCTION ###############################################################
################################################################################

def test_generate(xml_filename):  

    # Load XML into DOM ----------------------
    dom = parse(xml_filename)

    # Import into datastore ------------------
    process_xml(dom.documentElement, datastore)


    # Generate Pxtestdef.h files -------------------------------------------------
      
    # get output folder
    output_folder = os.path.join(TESTGEN_OUTDIR, datastore["name"])
    if not os.path.isdir(output_folder):
        os.makedirs(output_folder)

    # determine number of partitions
    numParts = datastore["partitions"]
  
    # Add relevant fields to the datastore
    datastore["total_steps"] = \
        reduce(lambda acc, x: acc + x["repeat"], datastore["test_steps"], 0)


    # Iterate through all defined partitions -------------------------------------
    for p in range(0, numParts):
        # signal current partition
        datastore["p"] = p
    
        # make partition folder
        partFolderPath    = os.path.join(output_folder, "p" + str(p))
        if not os.path.isdir(partFolderPath):
            os.mkdir(partFolderPath)
  
        # Process the Pxtestdef.h file -----------------------
        res, msg = process_file(PX_TESTDEF_H_FILE, datastore, 
                                template_folder=TEMPLATE_PATH,
                                output_folder=partFolderPath,
                                type="PARTITION",
                                index=p)
        if not res:
            print msg
            return res, msg
    
        # Process the Pxtestdef.c file -----------------------
        res, msg = process_file(PX_TESTDEF_C_FILE, datastore, 
                                template_folder=TEMPLATE_PATH,
                                output_folder=partFolderPath,
                                type="PARTITION",
                                index=p)
        if not res:
            print msg
            return res, msg
      
  
    # Process the Pxtestprint.c print partition ------------------
    partFolderPath    = os.path.join(output_folder, "p" + str(p + 1))

    # make partition folder
    if not os.path.isdir(partFolderPath):
        os.mkdir(partFolderPath)  
  
    res, msg = process_file(PX_TESTPRNT_C_FILE, datastore, 
                            template_folder=TEMPLATE_PATH,
                            output_folder=partFolderPath,
                            type="PARTITION",
                            index=p + 1)
    if not res:
        print msg
        return res, msg

    # Generate a test definition XML file  test.xml -------------
    res, msg = process_file(TESTXML_FILE % datastore["name"], datastore, 
                            template_folder=TEMPLATE_PATH,
                            output_folder=output_folder,
                            type="TESTXML", )
    if not res:
        print msg
        return res, msg
  
    #destination_file = os.path.join(output_folder,datastore["name"] + ".xml")
    #shutil.move(  os.path.join(output_folder,TESTXML_FILE), destination_file)
    #print "=>",destination_file
    
  
#===============================================================================
# Main routine execution
#===============================================================================
if __name__ == '__main__':
    """
  Application main function
  """
    xmlFlag = False; # Flag setting if only generates xml
    # Command line processing #####################################################
    
    if(len(sys.argv) == 3) and (sys.argv[1] == '-xml'):
        argname = sys.argv[2]
        print "Generating only xml configurations"
        xmlFlag = True;
    elif len(sys.argv) ==  2:
        argname = sys.argv[1]
    else:
        print Syntax
        error ("Invalid command line!")        
  
    if argname in ["--help", "-?"]:
        print Syntax
        error ("command line arguments")

    # if folder, process all xml files in it
    if os.path.isdir(argname):
        for root, dirs, files in os.walk(argname):
            for filename in files:
                if filename[:7] == "testdef":     
                    print "Processing file: " + os.path.join(root, filename)
                    print "Processing folder: " + root
                    test_generate(os.path.join(root, filename))
                    copyFiles(xmlFlag, root, filename)
                    
    else:
        print Syntax
        error ("command line arguments")
        
    # END OF FILE
