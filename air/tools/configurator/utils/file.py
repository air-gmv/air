## @package utils.file
#  @author pfnf,$(AIR_RUNNER_USER)
#  @brief File manipulation methods

import os
import shutil
import fnmatch
import re
import utils
import utils.terminal as terminalutils
import logging

from hashlib import sha256
from shutil import copy2, rmtree
from subprocess import Popen, PIPE

FILE_UTILS_MERGETOOL 	= 'vimdiff'
FILE_UTILS_DIFFTOOL  	= 'diff'
FILE_UTILS_EDITOR    	= 'vim'
FILE_UTILS_FORCE	 	= False
FILE_UTILS_COPY_DEFAULT = 2
keep_files = False

## Finds files with an certain extension and header within a directory
#  @param pwd directory to search
#  @param extension extension of the files to search
#  @param header header to look for
#  @param n number of lines to search
#  @return list of files
def findFiles(pwd, extension, header, n = 10000):

    files = []
    for f in os.listdir(pwd):
        if f.endswith(extension):
            try:
                fd = open(os.path.join(pwd, f))
                fdHeader = ' '.join(fd.readlines(n))
                fd.close()
                if fdHeader.find(header) >= 0: files.append(os.path.join(pwd, f))

            except:
                logging.exception("Exception, See details below")
                pass

    return files

## Safely creates a directory tree without raising errors if removal isn't possible
#  @param dir_path directory do crate
def safeMakedirs(dir_path, logger = None):

    try:
        os.makedirs(dir_path)
    except Exception as why:
        if logger is not None:
            logger.error(str(why))

## Safely os.removes a file from the filesystem without raising errors if removal isn't possible
#  @param file_path file to os.remove
def safeRemoveFile(file_path, logger = None):

    try:
        os.remove(file_path)
    except Exception as why:
        if logger is not None:
            logger.error(str(why))

def safeRemoveDirectory(dir_path, logger = None):

    try:
        from shutil import rmtree
        rmtree(dir_path)
    except Exception as why:
        if logger is not None:
            logger.error(str(why))

## Safe file copy, with user interactivity
# @param src Source file
# @param dst Destination file
# @param[out] errors list of errors that occurred during the execution
def safeFileCopy(src, dst, errors = None):

    src = os.path.normpath(src)
    # define filename is user decides to do hardcoded
    hc = os.path.normpath(dst + ".hc")
    dst = os.path.normpath(dst)

    # Copy the file interactively
    safeMakedirs(os.path.dirname(dst))
    if not FILE_UTILS_FORCE and os.path.exists(dst) and genFileS256Hash(src) != genFileS256Hash(dst):
        while True:
            # if already exist and hardcoded define then ignore
            # Also ignore if --keep-files-silent
            if os.path.exists(hc) or keep_files:
                result = 1
            else:
                FILE_COPY_EXISTS = "   File '{0}' already exists and they are different:"
                FILE_COPY_OPTS = ["&Replace old file", "&Keep old file", "&Merge both files", "View files &diff"]
                result = terminalutils.promptActions(
                    FILE_COPY_EXISTS.format(os.path.relpath(dst, utils.WORKING_DIRECTORY)),
                    FILE_COPY_OPTS,
                    FILE_UTILS_COPY_DEFAULT)

            # Replace file
            if result == 0:
                break

            # Keep file
            if result == 1: return True

            # Merge files
            if result == 2:
                proc = Popen(utils.flatten([FILE_UTILS_MERGETOOL.split(), src, dst]), stderr = PIPE)
                proc.wait()
                return False

            # View file diff
            if result == 3:
                proc = Popen(utils.flatten([FILE_UTILS_DIFFTOOL.split(), src, dst]), stderr = PIPE)
                proc.wait()

    # Just try to copy the file
    try:
        copy2(src, dst)
        return True
    except Exception as why:
        if errors: errors.append(str(why))
        return False

## Safe multi file copy, with user interactivity
# @param src Source file
# @param dst Destination file
# @param[out] errors list of errors that occurred during the execution
def safeMultiFileCopy(src, dst, errors = None):

    # Copy files
    for p, ds, fs in os.walk(src):
        rp = os.path.relpath(p, src)

        # Create destination directory
        if rp != '.':
            safeMakedirs(os.path.join(dst, rp), errors)

        # Process files in source directory
        for f in fs:
            fsrc = os.path.join(src, rp, f)
            fdst = os.path.join(dst, rp, f)
            safeFileCopy(fsrc, fdst, errors)

    # os.remove the source directory
    #if not logger.haveErrors() and os.removeSrc:
    #	rmtree(src, ignore_errors = True)

## Gens a Hexadecimal SHA256 hash string of a file
# @param file_path File os.path
# @return Hexadecimal SHA256 hash string
def genFileS256Hash(file_path):
    
    # Open file
    try:
        fd = open(file_path, 'rb+')
    except:
        logging.exception("Exception, See details below")
        return ''
    # Create hash
    hasher = sha256()
    buf = fd.read(65536)
    while len(buf) > 0:
        #update function receives binary objects
        hasher.update(buf)
        buf = fd.read(65536)
    fd.close()
    return hasher.hexdigest()

## Gets a valid file name from a string
# @param file_name string to validate
# @return valid file name
def getValidFilename(file_name):
    valid_chars = '-_.() abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
    return ''.join(c for c in file_name if c in valid_chars)

def getFilesByExtensions(lookup_dir, extensions, traverse=True):

    files_found = []
    if traverse:
        for root, dirs, files in os.walk(lookup_dir):
            for f in files:
                for ext in extensions:
                    if f.endswith(ext):
                        files_found.append(os.path.join(root, f))
    else:
        for f in os.listdir(lookup_dir):
            for ext in extensions:
                if f.endswith(ext):
                    files_found.append(os.path.join(lookup_dir, f))

    return sorted(files_found)

def createFileRecord(directory):

    record = dict()
    for p, dirs, files in os.walk(directory):
        rp = os.path.relpath(p, directory)
        for f in files:
            record[os.path.join(rp, f)] = genFileS256Hash(os.path.join(p, f))

    return record

def saveFileRecord(file_name, record):

    # Create record file
    try:
        fd = open(file_name, 'w')
    except Exception as why:
        return False

    for key in record.keys():
        fd.write('[{0}]{1}\n'.format(record[key], key))

    fd.close()
    return True

def loadFileRecord(file_name):

    # open clean record file
    try:
        with open(file_name) as fd: contents = fd.readlines()
    except:
        logging.debug("Exception, See details below")
        return None

    # Parse the cleaning record
    record = dict()
    for l in contents:
        match = re.match('\[(?P<hash>.*?)\]', l)
        if match:
            # Get hash and file path
            f_hash = match.group('hash')
            f_name = l.replace('[{0}]'.format(f_hash),'').replace('\n', '')
            record[f_name] = f_hash

    return record

## Safe multi file copy, with user interactivity
# @param src Source file
# @param dst Destination file
# @param[out] errors list of errors that occurred during the execution
def safeMultiFileCopyWithRecord(src, dst, record, errors = None):

    # check if record is valid
    if record is None:
        safeMultiFileCopy(src, dst, errors)
        return

    # copy files
    for p, ds, fs in os.walk(src):
        rp = os.path.relpath(p, src)

        # create destination directory
        if rp != '.': safeMakedirs(os.path.join(dst, rp), errors)

        # process files in source directory
        for f in fs:

            f_src = os.path.join(src, rp, f)
            f_dst = os.path.join(dst, rp, f)

            # record key
            key = os.path.join(rp, f)

            # remove file if it match the record
            if key in record and os.path.isfile(f_dst) and record[key] == genFileS256Hash(f_dst):
                safeRemoveFile(f_dst, errors)

            # copy file
            safeFileCopy(f_src, f_dst, errors)


## Creates a cleaning record to clean the configurations
# @param directory Directory for the record
# @param record Record filename
# @param logger Logger to register the events
def createCleaningRecord(directory, record, logger):

    # Create record file
    try:
        fd = open(record, 'w')
    except Exception as why:
        return False

    # Write record data
    for p, dirs, files in os.walk(directory):
        rp = os.path.relpath(p, directory)
        for f in files:
            fd.write('[{0}]{1}\n'.format(genFileS256Hash(os.path.join(p, f)), os.path.join(rp, f)))

    fd.close()
    return True

## Runs a cleaning record to clean the configurations
# @param record Record filename
# @param logger Logger to register the events
def runCleaningRecord(record, logger, followDefault = False):

    res = True
    # Parse the cleaning record
    try:
        with open(record) as f:
            contents = f.readlines()
    except Exception as why:
        return False

    directories = []
    for l in contents:
        m = re.match('\[(?P<hash>.*?)\]', l)
        if m:
            # Get hash and file path
            originalHash = m.group('hash')
            file = os.path.join(os.getcwd(), l.replace('[{0}]'.format(originalHash), '').replace('\n', ''))

            # Store the paths for removal
            directory = os.path.dirname(file)
            if directory not in directories and directory[-1] != '.':
                directories.append(directory)

            # Check if the files are the same
            currentHash = genFileS256Hash(file)
            logger.information(1,"Removing file '{0}'...", file)
            if currentHash and currentHash != originalHash and \
               terminalutils.promptActions(
                       "File '{0}', was modified, do you still want remove it?".format(file),
                       ["Remove file", "Keep file"], 1):
                res = False
                continue
            else:
                safeRemoveFile(file, logger)

    # Remove directories
    for d in directories:
        if os.path.isdir(d) and not os.listdir(d):
            try:
                rmtree(d)
            except:
                logging.exception("Exception, See details below")
                continue

    # remove configuration record
    if res: safeRemoveFile(record)


## @brief Search hardcoded files (extension .hc), replace the generated file (.c) with it. 
# Also replaces the file in .temp folder so the file signature is correctly stored in .config.
def setHardcodedFiles(app=0):
    for root, dirnames, filenames in os.walk('.'):
        for filename in fnmatch.filter(filenames, '*.hc'):
            #matches.append(os.path.join(root, filename))

            # Replace the file in the partition folder
            old_name = os.path.join(root, filename)
            new_name = old_name[:-3]
            shutil.copy(old_name, new_name)
                
            # Replace in .temp
            if app : 
                temp_folder_path = ".temp/" + new_name
                shutil.copy(new_name,temp_folder_path )
