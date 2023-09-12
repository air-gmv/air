#!/usr/bin/env python3.6

"""
This modules provides an API to execute a coding rules analysis (cppcheck with
its misra addon) and parse the results.

It relies heavily on subprocesses and regexes.
The run and analysis phases can be run separately.
"""

import os
import sys
import hjson
import subprocess
import argparse
import xml.etree.ElementTree as ET

CPPCHECK_EXE = ['cppcheck']

# Misra rules - relative to this script
MISRA_RULES = os.path.dirname(
    os.path.abspath(__file__)) + '/misra-rules-all.txt'
MISRA_CONFIG = os.path.dirname(os.path.abspath(__file__)) + '/misra.json'

# Out files, relative to an output folder passed as parameter
OUT_INCLUDES = 'includes.txt'
OUT_CPPCHECK = 'cppcheck_out.xml'


def runCmd(cmd, cwd=".") -> bool:
    try:
        output = subprocess.check_output(
            cmd, stderr=subprocess.STDOUT, shell=True, timeout=3,
            universal_newlines=True, cwd=cwd)
    except subprocess.CalledProcessError as exc:
        print(
            f'Status : FAIL, returncode: {exc.returncode}, output: {exc.output}')
    else:
        if output != "":
            print(f'Out: {output}')


class CodingRulesRunner(object):
    def __init__(self, project_folder, output_folder, config_file, exclude_file, src_file_list):
        """
        project_folder: root folder of the source code, containing ASW and EPSW folders.
        output_folder: folder were to store reports.
        """
        self.project_folder = project_folder
        self.output_folder = output_folder
        self.excluded = []
        self.src_file_list = None

        if exclude_file:
            with open(exclude_file, "r") as f:
                for line in f.readlines():
                    self.excluded.append(f'-i')
                    self.excluded.append(line.strip())

        if src_file_list:
            self.src_file_list = src_file_list.split(" ")

        with open(config_file, "r") as f:
            config = hjson.load(f)
            self.CPPCHECK_FLAGS = config["CPPCHECK_FLAGS"]
            self.PREPROCESSOR_FLAGS = config["PREPROCESSOR_FLAGS"]

    def run(self):
        """
        By default, run cppcheck on all files, and then run the misra addon on
        each file. check_modules can be a list of modules to check.
        """

        # make sure the out folder exists and clean old reports
        subprocess.check_call(['mkdir', '-p', self.output_folder])
        subprocess.check_call(['rm', '-f', OUT_INCLUDES],
                              cwd=self.output_folder)
        subprocess.check_call(['rm', '-f', OUT_CPPCHECK],
                              cwd=self.output_folder)

        with open('%s/%s' % (self.output_folder, OUT_INCLUDES), 'a') as f:
            subprocess.check_call(
                'find "%s/" -name "*.[h]" | xargs -I{} dirname {} | uniq' % self.project_folder, shell=True, stdout=f)

        if self.src_file_list:
            with open('%s/%s' % (self.output_folder, OUT_CPPCHECK), 'a') as f:
                subprocess.check_call(CPPCHECK_EXE + self.CPPCHECK_FLAGS + self.PREPROCESSOR_FLAGS + self.excluded + [
                                      '--xml', f'--addon={MISRA_CONFIG}', f'--includes-file={self.output_folder}/{OUT_INCLUDES}'] + self.src_file_list, stderr=f)
        else:
            with open('%s/%s' % (self.output_folder, OUT_CPPCHECK), 'a') as f:
                subprocess.check_call(CPPCHECK_EXE + self.CPPCHECK_FLAGS + self.PREPROCESSOR_FLAGS + self.excluded + [
                                      '--xml', f'--addon={MISRA_CONFIG}', f'--includes-file={self.output_folder}/{OUT_INCLUDES}', self.project_folder], stderr=f)

        runCmd(f'python3 cppcheck-htmlreport.py --file {self.output_folder}/{OUT_CPPCHECK} --report-dir {self.output_folder}/html --source-dir {self.project_folder}',
               cwd=os.path.dirname(os.path.abspath(__file__)))


class CodingRulesAnalyzer(object):
    def __init__(self, output_folder):
        """
        filepath: cppcheck report to analyse
        """
        self.output_folder = output_folder
        self.filepath = f'{output_folder}/{OUT_CPPCHECK}'

        self.results = dict()

    def analyse(self):
        tree = ET.parse(self.filepath)
        errors = tree.getroot().find("errors")

        for err in errors:
            try:
                self.results[err.attrib["id"]] += 1
            except:
                self.results[err.attrib["id"]] = 1

    def print_summary(self):
        """
        Print summary results, sorted by number of occurences
        """
        with open(f"{self.output_folder}/summary.txt", "w") as f:
            f.write("CPPCHECK defects\n")
            print("CPPCHECK defects")
            for violation, rep in sorted(self.results.items()):
                if "misra" not in violation:
                    f.write(f"\t- {violation}: {rep}\n")
                    print(f"\t- {violation}: {rep}")

            f.write("\nMISRA-C_2012 violations\n")
            print("MISRA-C_2012 violations")
            for violation, rep in sorted(self.results.items()):
                if "misra" in violation:
                    f.write(f"\t- {violation}: {rep}\n")
                    print(f"\t- {violation}: {rep}")

            f.write(f"\n")
            f.write("-"*10 + " DETAILED INFO " + "-"*10)
            f.write(f"\n")
            f.write(f"\n")

            with open(self.filepath, "r") as f_all:
                f.writelines(f_all.read())


def run(project_folder, output_folder, config_file, exclude_file, src_file_list):
    CodingRulesRunner(project_folder, output_folder,
                      config_file, exclude_file, src_file_list).run()


def analyse(output_folder):
    cppcheck_std = CodingRulesAnalyzer(output_folder)

    cppcheck_std.analyse()
    cppcheck_std.print_summary()


parser = argparse.ArgumentParser(description='Run cppcheck.')
parser.add_argument('source', type=str, help='main source code folder')
parser.add_argument('--configuration', default="utils/misraivoso/configuration.hjson",
                    type=str, help='configuration file')
parser.add_argument('--output', default="utils/misraivoso/output", type=str, help='output path')
# parser.add_argument('--configuration', default="utils/misraivoso/configuration.hjson", type=str, help='configuration file')
# parser.add_argument('--output', default="utils/misraivoso/output", type=str, help='output path')
parser.add_argument('--exclude', default=None, type=str,
                    help='exclude file (files or path to exclude), one file path per line')
parser.add_argument('--src-file-list', default=None, type=str,
                    help='source file list to be checked individually instead of full main source folder')
args = parser.parse_args()

if __name__ == "__main__":
    SOURCE_FOLDER = args.source
    OUTPUT_FOLDER = args.output

    if not os.path.exists(args.configuration):
        print(
            f"Error, configuration file {args.configuration} does not exists")
        parser.print_help()
        sys.exit(-1)

    os.makedirs(OUTPUT_FOLDER, exist_ok=True)

    run(SOURCE_FOLDER, OUTPUT_FOLDER, args.configuration,
        args.exclude, args.src_file_list)
    analyse(OUTPUT_FOLDER)
