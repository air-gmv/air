#!/usr/bin/python

##### Syntax ###################################################################
Syntax = """
  Requirements parser:
  
    SYNTAX:
    
      ./reqsparse requirements.xml [excel|docx|tests|list] [test|requirement]

        * The requirements xml input parameter is mandatory
        
        * Without further parameters the program will enter the iteractive mode
          in order to analyse requirements or tests
        
        * excel: will generate an excel table with all requirements and tests
        
        * tests: will generate the test definition xml files based on a template
        
        * [test|requirement]: generate tests only for the specified parameter
        
        * docs: Obsolete use generate_docs
        Requires https://github.com/mikemaccana/python-docx.git    
        
        * list: list all requirements and test references
"""



#####  Global constants ######################################################## 
requirement_prefixes = ["RB-", "GEN-FDIR-", "REQ-IO-", "IMA_SEP_"]
test_prefixes     = ["TEST-DEF"]

ReqTests = "ReqTests"
OutputFolder = "output"

# Mandatory imports #############################################################
from globalUtils.definitions import *
from input.scripts.mk_usrfiles import *
from xml.dom.minidom import parse
import os.path
import sys

# Conditional imports ###########################################################
#try:

#    import xlwt
#except:
#    print "Excel functionality will be disabled"

try:
    from colorama import init, Fore, Back, Style
    color = True
except:
    print "Colors disabled"
    color = False

# DISABLED because requires docx github
#try:
#    from docx.docx import *
#except:
#    print "Word functionality will be disabled"

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
def neither(a, b):
    return (not a) and (not b)
  
##### XML Functions ############################################################
  
def find_elem_name(nodes, name):
    for node in nodes:
        if not(node.nodeType in [node.TEXT_NODE, node.COMMENT_NODE]) and node.getAttribute("name") == name:      
            return node
        if node.childNodes:
            possible_node = find_elem_name(node.childNodes, name)
            if possible_node:
                return possible_node
    return None
    
  
def print_dom_tree(nodes, level=0):
    for node in nodes:
        #if node.nodeType != node.TEXT_NODE:
        print '\t' * level, node.nodeName
        if node.childNodes:
            print_dom_tree(node.childNodes, level + 1)


def print_requirements(nodes, level=0, linelength=70, datastore=[], tests={}):
    for node in nodes:
        # Element nodes
        if node.nodeType == node.ELEMENT_NODE:
            name   = node.getAttribute("name")
            test_id = node.getAttribute("test_id")
            ref   = node.getAttribute("ref")    # requirements implemented
            gref  = node.getAttribute("gref")    # requirements implemented (traced by GMV)      
            # 'named' attributes
            if name:
                tab (level)
                print Fore.GREEN + node.getAttribute("name") + Fore.RESET
            # tests
            elif test_id and node.tagName in ["test", "testlink"]:
                tab (level)
                if node.tagName == "test":
                    print   Fore.RED           + \
                        test_id            + \
                        Fore.RESET          + \
                        " "              + \
                        node.getAttribute("fsb")  + \
                        "% feasability "      + \
                        node.getAttribute("rel")  + \
                        "% relevance "              
                else:
                    print   Style.BRIGHT + Fore.RED   + \
                        test_id  + "(link)"      + \
                        Fore.RESET + Style.NORMAL
                    try:
                        print align_paragraph(tests[test_id][0], linelength-(level + 1) * 8, level + 1)
                    except:
                        print align_paragraph("External test", linelength-(level + 1) * 8, level + 1)
            # derived / parent requirements
            elif ref or gref:        
                tab (level)
                format_start = Fore.BLUE  if ref else Style.BRIGHT + Fore.BLUE
                format_end   = Fore.RESET if ref else Fore.RESET + Style.NORMAL
                print format_start + max(ref, gref) + format_end
                print align_paragraph(datastore[max(ref, gref)]["text"], linelength-(level + 1) * 8, level + 1)
        # Text Nodes
        elif node.nodeType == node.TEXT_NODE:
            if node.parentNode.tagName == "comment":
                print Fore.CYAN,
            print align_paragraph(node.data, linelength-level * 8, level) + Fore.RESET
        # recursively walk the tree 
        if node.childNodes:
            print_requirements(node.childNodes, level + 1, linelength, datastore, tests)

##### Excel Functions ##########################################################


# excel test line reference
excel_test_line = {}
excel_testlink_line = []
        
# check that test tag features bot the 'feasability' and 'relevance' attributes
def check_valid_test_attr(node):
    if   not(node.getAttribute("fsb")) or not(node.getAttribute("rel")):
        raise Exception(node.getAttribute("test_id"), "Test missing attributes")
        
def format_text(elem, level):
  
    easyfx_str = 'align: wrap on, vert top, horiz left, indent %i ;' % level
  
    if elem.parentNode.tagName == "comment":
        easyfx_str += 'font: color_index 4'
        if level > 0:
            easyfx_str += ', italic on;'
    elif elem.parentNode.tagName in ["test", "testlink"]:
        easyfx_str += 'font: color_index 2'
        if level > 0:
            easyfx_str += ', italic on;'
    elif level > 0:
        easyfx_str += 'font: italic on;' 
    return xlwt.easyxf(easyfx_str)
  
def xml2excel(nodes, ws, level=0, line=0, \
              tags=["section", "subsection", "req", "sub", "note", "comment"]):

    # formatting dictionary
    st_wrappoffd = {"section": xlwt.easyxf('align: wrap off, vert top, horiz left , indent 0; font: bold on ; pattern: pattern solid, fore-colour grey50')
        , "subsection": xlwt.easyxf('align: wrap off, vert top, horiz left , indent 2; font: bold on ; pattern: pattern solid, fore-colour grey25; border : top thin , bottom thin')
        , "req": xlwt.easyxf('align: wrap off, vert top, horiz left , indent 5 ; font: bold on, italic on; pattern: pattern solid, fore-colour grey25')
            }

    for node in nodes:    
        line_delta   = 0
        level_delta = 0
    
        if node.nodeType == node.ELEMENT_NODE:
            tag  = node.tagName
            name = node.getAttribute("name")
            if tag == "req" and name:
                ws.write_merge(line, line, level, level + 4, 
                               name, st_wrappoffd[node.tagName])
                line_delta = 1
            elif tag == "testlink" and tag in tags:    
                test_id = node.getAttribute("test_id")
                excel_testlink_line.append([test_id, line, level])
                level_delta = 1          
                line_delta   = 1        
        elif node.nodeType == node.TEXT_NODE:
            show_text = (node.parentNode.tagName in tags) and     \
                (node.parentNode.parentNode.tagName in tags)
        
            text = clean_spaces(node.data)
            # all nodes have text, but in most cases it is empty
            if show_text and text.replace(" ", ""):
                level_delta = 1
                if node.parentNode.tagName == "test":
                    check_valid_test_attr(node.parentNode)
                    test_id = node.parentNode.getAttribute("test_id")
                    text = test_id + ":" + text
                    ws.write(line, 2, node.parentNode.getAttribute("fsb"))
                    ws.write(line, 3, node.parentNode.getAttribute("rel"))
                    excel_test_line[test_id] = line + 1
                ws.write(line, 1, text, format_text(node, level))
                line_delta   = 1
        line  += line_delta
        level += level_delta
        if node.childNodes:
            line = xml2excel(node.childNodes, ws, level + level_delta, line, tags)
    return line

def xml2excel_testlinks(ws, filename):
    for test_id, line, level in excel_testlink_line:
        link_text = ""  
        try:  
            ref = excel_test_line[test_id]
            link_text = 'HYPERLINK("[' + filename + ']' + ReqTests + '!B%i:B%i";"%s")' % \
                (ref, ref, test_id)
        except:
            link_text = '"' + test_id + '"'
        ws.write(line, 1, xlwt.Formula(link_text),
                 xlwt.easyxf("align: wrap on, vert top, horiz left, indent 5; font: color_index 2"))

def create_worksheet_from_xml(wb, dom, filename, ws_name, tags, testlinks=False):
    global excel_test_line
    global excel_testlink_line
    ws  = wb.add_sheet(ws_name)
    excel_test_line = {}
    excel_testlink_line = []  
    xml2excel(dom.documentElement.childNodes, ws, tags=tags)
    ws.col(1).width = 30000
    if (testlinks):
        xml2excel_testlinks(ws, filename)
    return ws

# return a list of keys of a dictionary sorted by a given column (index) of the
# data field. If index is a list, the sum of each of the fields is used to sort.
def sort_by_index(dictionary, index):
    lst = []
    if type(index) is int:
        sort_func = lambda x: x[index]
    elif type(index) is list:
        sort_func = lambda x: reduce(lambda acc, y: acc + int(x[y]), index, 0)

    for key, value in dictionary.items():
        lst.append([int(sort_func(value)), key])
        #print key, int(sort_func(value))
    return [y for x, y in sorted(lst)]
  
def create_worksheet_for_tests(wb, tests, ws_name):
    ws  = wb.add_sheet(ws_name)
    ws.col(0).width = 4000
    ws.col(1).width = 2000
    ws.col(2).width = 2000
    # headers
    ws.write(0, 0, "Test", xlwt.easyxf('align: wrap on, vert top, horiz left ; pattern: pattern solid, fore-colour grey25;'))
    ws.write(0, 1, "Feasibility", xlwt.easyxf('align: wrap on, vert top, horiz left ; pattern: pattern solid, fore-colour grey25;'))
    ws.write(0, 2, "Relevance", xlwt.easyxf('align: wrap on, vert top, horiz left ; pattern: pattern solid, fore-colour grey25;'))
    ws.write(0, 3, "Description", xlwt.easyxf('align: wrap on, vert top, horiz left ; pattern: pattern solid, fore-colour grey25;'))
  
    line = 1
    for test in reversed(sort_by_index(tests, [2, 3])):
        link_text = 'HYPERLINK("[D01-02.xls]' + ReqTests + '!B%i:B%i";"%s")' % \
            (excel_test_line[test], excel_test_line[test], test)
        ws.write(line, 0, xlwt.Formula(link_text), xlwt.easyxf('align: wrap on, vert top, horiz left ;'))
        ws.write(line, 1, tests[test][2], xlwt.easyxf('align: wrap on, vert top, horiz left ;')) # feasibility
        ws.write(line, 2, tests[test][3], xlwt.easyxf('align: wrap on, vert top, horiz left ;')) # relevance
        ws.write(line, 3, tests[test][0], xlwt.easyxf('align: wrap on, vert top, horiz left'))
        line += 1
    ws.col(3).width = 20000

##### Word / .docx Functions ##################################################
def save_word_doc_as(document, filename, title, subject):
    # Default set of relationshipships - these are the minimum components of a document
    relationships = relationshiplist()

    # Create our properties, contenttypes, and other support files
    coreprops = coreproperties(title=title
                               , subject=subject
                               , creator="GMVIS"
                               , keywords=['IMA 4 Space', 'IMA'])
    appprops = appproperties()
    content_types = contenttypes()
    web_settings = websettings()
    word_relationships = wordrelationships(relationships)
    
    # Save our document
    savedocx(document, coreprops, appprops, content_types, web_settings, word_relationships, filename + ".docx")


def xml2docx_reqs(nodes, docbody,
                  tags=["section", "subsection", "req", "sub", "note", "comment", "test", "testlink"],
                  sections=[]):
    # formatting dictionary
    for node in nodes:    
        skip_section = False 
        if node.nodeType == node.ELEMENT_NODE:
            tag  = node.tagName
            name = node.getAttribute("name")
            if tag == "req" and name:
                docbody.append(paragraph(name, style="ListBullet"))
                parents = datastore[name]['parents']
                if parents:
                    parents_text = ", ".join(parents)
                    docbody.append(paragraph(parents_text, "NormalIndentSubHeading"))
            elif tag == "testlink" and tag in tags:    
                test_id = node.getAttribute("test_id")
                docbody.append(paragraph(test_id))
            elif tag == "section" and tag in tags:
                section_id = node.getAttribute("name")
                if sections and section_id not in sections:
                    skip_section = True
                else:
                    docbody.append(paragraph(section_id, "Heading2"))
            elif tag == "subsection" and tag in tags:
                subsection_id = node.getAttribute("name")
                docbody.append(paragraph(subsection_id, "Heading3"))
        elif node.nodeType == node.TEXT_NODE:
            show_text = (node.parentNode.tagName in tags) and     \
                (node.parentNode.parentNode.tagName in tags)
        
            text = clean_spaces(node.data)
            # all nodes have text, but in most cases it is empty
            if show_text and text.replace(" ", ""):
                if node.parentNode.tagName == "test":
                    check_valid_test_attr(node.parentNode)
                    test_id = node.parentNode.getAttribute("test_id")
                    docbody.append(paragraph("[" + test_id + "] fsb=" + node.parentNode.getAttribute("fsb")
                                   , "NormalIndent"))
                if    node.parentNode.tagName == "comment":
                    docbody.append(paragraph(text, style="NormalIndentSubHeading"))
                elif  node.parentNode.tagName == "note":
                    docbody.append(paragraph(text, style="NormalIndent2"))
                else:
                    docbody.append(paragraph(text, style="NormalIndent"))
        if node.childNodes and not skip_section:
            xml2docx_reqs(node.childNodes, docbody, tags)
    return 


def xml2docx_tests(nodes, docbody,
                   tags=["section", "subsection", "req", "test", "testlink"],
                   sections=[]):
    # test 2 requirements mapping
    mapping_txt = []
    # moving through the tree to keep xml order
    for node in nodes:    
        skip_section = False 
        if node.nodeType == node.ELEMENT_NODE:
            tag  = node.tagName
            name = node.getAttribute("name")
            if tag == "req" and name:
                docbody.append(paragraph(name, style="ListBullet"))
            elif tag == "testlink" and tag in tags:    
                test_id = node.getAttribute("test_id")
                docbody.append(paragraph("(" + test_id + ")", style="NormalTestId"))
            elif tag == "section" and tag in tags:
                section_id = node.getAttribute("name")
                if sections and section_id not in sections:
                    skip_section = True
                else:
                    docbody.append(paragraph(section_id, "Heading2"))
            elif tag == "subsection" and tag in tags:
                subsection_id = node.getAttribute("name")
                docbody.append(paragraph(subsection_id, "Heading3"))
        elif node.nodeType == node.TEXT_NODE:
            show_text = (node.parentNode.tagName in tags) and     \
                (node.parentNode.parentNode.tagName in tags)
        
            text = clean_spaces(node.data)
            # all nodes have text, but in most cases it is empty
            if show_text and text.replace(" ", ""):
                if node.parentNode.tagName == "test":
                    check_valid_test_attr(node.parentNode)
                    test_id = node.parentNode.getAttribute("test_id")
                    #  + node.parentNode.getAttribute("fsb")
                    docbody.append(paragraph("[" + test_id + "]", "NormalTestId"))
                    docbody.append(paragraph(text, style="NormalIndent"))
                else:
                    docbody.append(paragraph(text, style="NormalIndent"))
        if node.childNodes and not skip_section:
            xml2docx_tests(node.childNodes, docbody, tags)
    return 

##### Test Suite Functions #####################################################
def create_test_xml_files(teststore, ttest=None, rreq=None):

    if not os.path.isdir(TESTSDEFS_FOLDER):
        os.mkdir(TESTSDEFS_FOLDER)

    for test in sorted(teststore.keys()):
        res = True
    
        # if specific test requested, process only that!
        if ttest and (test != ttest):
            continue
        # if specific requirement requested, process only relevant tests
        if rreq and not (rreq in teststore[test][1]):
            continue
    
        description = align_paragraph(teststore[test][0], 70, 3)
    
        test_dict = dict (name=test
                          , id=int(test[-5:])
                          , partitions=2
                          , description=description
                          , requirements=teststore[test][1])
    
        out_folder = TESTSDEFS_FOLDER + '/' + test
        if not os.path.isdir(out_folder):
            os.mkdir(out_folder)
    
        res, msg = process_file(TEST_DEF_FILE, test_dict, 
                                template_folder=TEMP_FLDER_PATH,
                                output_folder=out_folder,
                                type="TESTDEF",
                                index=test)                            
        if not res:
            print msg
            return res, msg

  
##### Datastore Functions ######################################################
def requirements_to_datastore(nodes, datastore, teststore, req="",
                              tags=["section", "subsection", "req", "sub", "note", "comment", "test", "testlink"], line=0):
    
    for node in nodes:
        line_delta = 0
        # ELEMENT NODES
        if node.nodeType == node.ELEMENT_NODE:
            tag  = node.tagName
            name = node.getAttribute("name")
            if tag == "req":
                print "nodes req"
                req = name
                line_delta = 1
                # duplicate requirement, complain
                if datastore.get(req):
                    print "Duplicate requirement: %s" % req
                    sys.exit(-1)
                datastore[req] = {
                    'index': line,
                    'text': '',
                    'sub': [],
                    'note': [],
                    'comment': '',
                    'tests': {},
                    'testlinks': [],
                    'parents': []}
            elif tag == "testlink":
                print "node testlink"
                test_id = node.getAttribute("test_id")
                datastore[req]['testlinks'].append(test_id)
                try:
                    teststore[test_id][1].append(req)
                    print "222222222222222222222APPEND REQ"
                except:
                    #if not(test_id[:5] in ["T-API-"]) or not( test_id[:4] in "val_02_"):
                    print "ignoring append req"
                    if ("T-API-" not in test_id[:6]) and ("val_" not in test_id[:4]):
                        print "Linking to undefined test : %s @ %s" % (test_id, req)
                        sys.exit(-1)
            elif tag == "parent":
                datastore[req]['parents'].append(node.getAttribute("ref"))
        # TEXT NODES
        elif node.nodeType == node.TEXT_NODE:   
            print "nodes text"
            show_text = (node.parentNode.tagName in tags) and     \
                (node.parentNode.parentNode.tagName in tags)
            text = clean_spaces(node.data)
            # all nodes have text, but in most cases it is empty
            if show_text and text.replace(" ", ""):
                tag  = node.parentNode.tagName        
                if tag == "req":
                    datastore[req]['text'] = text
                elif tag == "sub":
                    datastore[req]['sub'].append(text)
                elif tag == "note":
                    datastore[req]['note'].append(text)
                elif tag == "comment":
                    datastore[req]['comment'] = text
                elif tag == "test":
                    test_id = node.parentNode.getAttribute("test_id")
                    fsb = int(node.parentNode.getAttribute("fsb"))
                    rel = int(node.parentNode.getAttribute("rel"))
                    datastore[req]['tests'][test_id] = text
                    if not teststore.get(test_id):
                        teststore[test_id] = [text, [req], fsb, rel]
                        print "1111111111111111111111set test_id"
                    else:
                        teststore[test_id][1].append(req)
                        print "XXXXXXXXXXXXXXXXXXXXXset append testid"
        line += line_delta  
        if node.childNodes:
            line = requirements_to_datastore(node.childNodes, datastore, teststore, req, line=line)
    return line


##### Global Data #############################################################
      
# Associative Datastore (key = requirement, value = dictionary of properties)

def main():
    datastore = {}
    requirements = [] # sorted datastore, same order as file
    # Associative reverse test lookup (key = test, value = requirements)
    tests = {}

    # Parse command line ##########################################################  
    reqs_file = ""
    gen_xml   = False
    gen_docx  = False
    gen_tests = False
    iter      = True
    ttest     = None
    rreq      = None
    if len(sys.argv) < 2:
        print Syntax 
        sys.exit(-1)
    elif len(sys.argv) >= 3:
        if    sys.argv[2] in ["excel", "xls"]:
            gen_xml   = True
            iter      = False
        elif sys.argv[2] in ["tests", "TESTS", "Tests"]:
            gen_tests = True
            iter      = False
        elif sys.argv[2] in ["docx", "doc", "word"]:
            gen_docx  = True
            iter      = False

    reqs_file = sys.argv[1]
    if reqs_file in ["--help", "-?"]:
        print Syntax
        sys.exit(-1)

    if len(sys.argv) == 4:
        if "TEST-DEF" in sys.argv[3]:
            ttest = sys.argv[3]
        else:
            rreq  = sys.argv[3]

    # Parse XML ###################################################################
    dom = parse(reqs_file)
    #print_requirements(dom.documentElement.childNodes,linelength=140)
    #print_dom_tree(dom.documentElement.childNodes)


    # Import into datastore ######################################################
    req_count = requirements_to_datastore(dom.documentElement.childNodes, datastore, tests)
    for req in sorted(datastore.items(), key=lambda x: x[1]["index"]):
        requirements.append(req[0])

    # Generate tests ###############################################################
    if gen_tests:
        print "Generating test XML files..."
        create_test_xml_files(tests, ttest, rreq)

    # Export to Excel ############################################################
    if gen_xml:

        excel_filename = os.path.basename(reqs_file.split(".")[0] + ".xls")
        excel_filename = os.path.join(OutputFolder, excel_filename)

        # Open and setup excel file
        wb = xlwt.Workbook(style_compression=2)

        create_worksheet_from_xml(wb, dom, os.path.basename(excel_filename),
                                  'All Requirements',
                                  tags=["section", "subsection", "req", "sub", "note"])

        create_worksheet_from_xml(wb, dom, os.path.basename(excel_filename), 
                                  ReqTests,
                                  tags=["section", "subsection", "req", "sub", "note", "comment", "test", "testlink"],
                                  testlinks=True)
        create_worksheet_for_tests(wb, tests, 'Tests')
        wb.save(excel_filename)
        print "Generated %s file." % excel_filename 


    # Export to Word / .docx ######################################################
    if gen_docx:
        print "Generating Requirements.docx document...",
        # Make a new document tree - this is the main part of a Word document
        document = newdocument()

        # This xpath location is where most interesting content lives 
        docbody = document.xpath('/w:document/w:body', namespaces=nsprefixes)[0]

        # create Requirements docx based on the structure of the XML file
        xml2docx_reqs (dom.documentElement.childNodes
                       , docbody
                       , tags=["section", "subsection", "req", "sub", "note", "comment", "parent"]
                       , sections=["AIR SEP Requirements"])

        doc_name = "Requirements"
        save_word_doc_as(document, doc_name, doc_name, doc_name);
        if color:
            print Fore.GREEN, "[OK]", Fore.RESET
        else:
            print "[OK]"


        print "Generating Tests.docx document...",
        # Make a new document tree - this is the main part of a Word document
        document = newdocument()

        # This xpath location is where most interesting content lives 
        docbody = document.xpath('/w:document/w:body', namespaces=nsprefixes)[0]

        # create Requirements docx based on the structure of the XML file
        xml2docx_tests (dom.documentElement.childNodes
                        , docbody
                        , sections=["AIR SEP Requirements"])

        # Requirements vs test traceability matrix
        list_of_lists = [["Requirements", "Tests"]]
        for r in requirements:
            tsts = []
            if "AIR_SEP_" in r and r < "AIR_SEP_1090":
                if (datastore[r]['testlinks']):
                    tsts = datastore[r]['testlinks'] + []
                if (datastore[r]['tests']):
                    tsts += datastore[r]['tests'].keys() + []
                if not tests:
                    tsts = [datastore[r]['comment']]
                list_of_lists.append([r, " ".join(sorted(tsts))])
        # write table
        docbody.append(paragraph('Requirements vs Test Trace:'))
        docbody.append(table(list_of_lists))


        # Requirements vs test traceability inverse matrix
        list_of_lists = [["Tests", "Requirements"]]
        for t in sorted(tests):
            list_of_lists.append([t, " ".join(sorted(tests[t][1]))])

        # write table
        docbody.append(paragraph('Requirements vs Test Inverse Trace:'))
        docbody.append(table(list_of_lists))

        # process inverse traceability
        list_of_lists = [["Requirements", "Derived Requirements"]]
        for r in requirements:
            if "AIR_SEP_" not in r:
                tr_reqs = []
                for k, v in datastore.items():
                    if r in v['parents']:
                        if "AIR_SEP_" not in k:
                            tr_reqs.append(k)
                        elif k < "AIR_SEP_1090":
                            tr_reqs.append(k)
                # we either trace the derived requirements, or present 
                # a relevant comment
                if tr_reqs:
                    list_of_lists.append([r, " ".join(sorted(tr_reqs))])
                else:
                    list_of_lists.append([r, datastore[r]["comment"]])
        # now, write another table
        docbody.append(paragraph('Inverse Requirements Trace:'))
        docbody.append(table(list_of_lists))

        # finally, save and report
        doc_name = "Tests"
        save_word_doc_as(document, doc_name, doc_name, doc_name);
        if color:
            print Fore.GREEN, "[OK]", Fore.RESET
        else:
            print "[OK]"


    # Interactive mode #############################################################
    if iter:
        print("Entering interactive mode")
        print "".ljust(120, '=')
        query = ""
        num = 0

        while not query in ['q', 'quit']:
            last_query = query
            query = raw_input("Enter your query: ")
            # increase / decrease number
            if query == "+" or not(query):
                num = num + 1 if num < req_count - 1 else 0    
                query = str(num) 
            elif query == "-":
                num = num - 1 if num > 0 else 0
                query = str(num)

            # last command
            if query == " ":
                query = last_query

            # sequencial number
            elif query.isdigit():
                query, ds_item = filter(lambda (x, y): y['index'] == int(query), datastore.items())[0]
                num = int(ds_item['index'])

            # Requirements (check if valid requirement prefix)
            if reduce(lambda acc, x: acc or (x in query), requirement_prefixes):
                if query in datastore.keys():
                    num = datastore[query]["index"]
                    req_dom = [find_elem_name(dom.documentElement.childNodes, query)]
                    if req_dom:
                        print_requirements(req_dom, level=0, linelength=120, datastore=datastore, tests=tests)

            # Test ID
            test = query
            if reduce(lambda acc, x: acc or (x in test), test_prefixes):
                if test in tests.keys():
                    # print requirement that lists this test
                    query = tests[test][1][0] # [1] : requirements list 
                        # [0] : first element should be original test definition
                    num = datastore[query]["index"]
                    req_dom = [find_elem_name(dom.documentElement.childNodes, query)]
                    if req_dom:
                        print_requirements(req_dom, level=0, linelength=120, datastore=datastore, tests=tests)
                    # print all requirements that either define or link to this test      
                    tab (1)
                    print "All requirements that are checked by this test: "  + Fore.GREEN,
                    for rq in tests[test][1]:        
                        print rq,
                    print Fore.RESET
                    print


            print "== [%i|%i]%s ==============================" % (num, req_count, query),
            print "".ljust(70, '=')
            print

    ## Detection of duplicate test IDs ###########
    #for test, value in sorted(tests.items()):
    #  print test,value[1]

    # Detect tests without the relevance field
if __name__ == "__main__":
    main()