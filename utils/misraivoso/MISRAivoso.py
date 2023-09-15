import xml.etree.ElementTree as ET
import re
import os
import tkinter as tk
import tkinter.ttk as ttk
import subprocess
from tkinter import filedialog
from tkinter import messagebox
from threading import Thread
import webbrowser
import argparse

# -------------------------------------------------------
# Function to extact the information from the XML report
# -------------------------------------------------------


def extract_code_snippets(xml_report):
    """
    Extracts code snippets from the XML report.

    Args:
        xml_report (str): Path to the XML report file.

    Returns:
        list: List of dictionaries containing code snippets and error information.
    """
    code_snippets = []

    try:
        tree = ET.parse(xml_report)
        root = tree.getroot()

        for error in root.findall('.//error'):
            location = error.find('location')

            if location is not None and all(attr in location.attrib for attr in ['file', 'line', 'column']):
                file_path = location.attrib['file']
                line_number = int(location.attrib['line'])
                column_number = int(location.attrib['column'])
                severity = error.attrib.get('severity', '')
                message = error.attrib.get('msg', '')

                with open(file_path, 'r', encoding='utf-8', errors='replace') as file:
                    lines = file.readlines()

                start_line = max(0, line_number - 1)
                end_line = min(len(lines), line_number)
                code_snippet = ''.join(lines[start_line:end_line])

                code_snippets.append({
                    'file': file_path,
                    'line': line_number,
                    'column': column_number,
                    'severity': severity,
                    'message': message,
                    'snippet': code_snippet,
                    'corrected': code_snippet
                })

    except ET.ParseError as parse_error:
        print(f"Error parsing XML: {parse_error}")

    return code_snippets


# ------------------------------------------------
# Functions to correct code to respect MISRA rules
# ------------------------------------------------

def correct_if_statements(code):
    """
    Corrects 'if' statements to use curly braces.

    Args:
        code (str): The input code.

    Returns:
        str: The corrected code.
    """
    pattern = r"\bif\s*\(([^)]*)\)([^\n]*)\n(.*)"
    matches = re.findall(pattern, code)
    code_lines = code.split("\n")

    for match in matches:
        condition = match[0]
        after_condition = match[1]
        statement = match[2]

        # Skip cases where there's no need for correction
        if (";" in after_condition) or ("{" in after_condition) or not (";" in statement):
            continue

        # Build the complete 'if' condition
        condition = "({}){}".format(condition, after_condition)
        old_if = "if{}".format(condition)

        if "\\" in old_if:
            # Handle 'if' statement spanning multiple lines
            if_statement = "if{}\n{{\\\n\t{}\n}}\\".format(
                condition, statement)
        else:
            if_statement = "if{}\n{{\n{}\n}}\n".format(
                condition, statement)

        for i, line in enumerate(code_lines):
            if (re.sub(r"\s", "", old_if) == re.sub(r"\s", "", line)) and (re.sub(r"\s", "", statement) == re.sub(r"\s", "", code_lines[i + 1])):
                if (not ("{" in statement)):
                    if "\\" in old_if:
                        code_lines[i + 1] = "\\"
                    else:
                        code_lines[i + 1] = ""
                    code_lines[i] = if_statement
                    break

    code = "\n".join(code_lines)
    return code


def correct_else_statements(code):
    """
    Corrects 'else' statements to use curly braces.

    Args:
        code (str): The input code.

    Returns:
        str: The corrected code.
    """
    pattern = r"\belse\b(.*\n)(.*)"
    matches = re.finditer(pattern, code)
    code_lines = code.split("\n")

    for match in matches:
        else_ = match.group(1)
        statement = match.group(2)
        old_else = "else {}".format(else_)

        # Skip cases where there's no need for correction
        if ("else if" in old_else) or ("{" in old_else) or (not ";" in statement):
            continue

        # Handle 'else' statement spanning multiple lines
        if "\\" in old_else:
            else_statement = "else{}{{\\\n{}\\\n}}\\\n".format(else_,
                                                               statement)
        else:
            else_statement = "else{}{{\n{}\n}}\n".format(else_,
                                                         statement)

        for i, line in enumerate(code_lines):
            if (re.sub(r"\s", "", old_else) == re.sub(r"\s", "", line)) and (re.sub(r"\s", "", statement) == re.sub(r"\s", "", code_lines[i + 1])):
                if (not ("{" in statement)):
                    if "\\" in old_else:
                        code_lines[i + 1] = "\\"
                    else:
                        code_lines[i + 1] = ""
                    code_lines[i] = else_statement
                    break

    code = "\n".join(code_lines)
    return code


def correct_while_statements(code):
    """
    Corrects 'while' loops to use curly braces.

    Args:
        code (str): The input code.

    Returns:
        str: The corrected code.
    """
    pattern = r"\bwhile\s*\(([^)]*)\)([^\n]*)\n(.*)"
    matches = re.finditer(pattern, code)
    code_lines = code.split("\n")

    for match in matches:
        condition = match.group(1)
        after_condition = match.group(2)
        loop_body = match.group(3)
        condition = "({}){}".format(condition, after_condition)
        old_while = "while{}".format(condition)
        correct = 0

        # Skip cases where correction is unnecessary
        if not (";" in loop_body):
            continue

        for i, line in enumerate(code_lines):
            if (re.sub(r"\s", "", old_while) == re.sub(r"\s", "", line)) and (re.sub(r"\s", "", loop_body) == re.sub(r"\s", "", code_lines[i + 1])):
                if (not ("{" in loop_body) and not ("{" in code_lines[i + 2])):
                    correct = 1
                    break

        if correct == 1:
            while_loop = "while{}\n{{\n{}\n}}\n".format(condition, loop_body)

            for i, line in enumerate(code_lines):
                if re.sub(r"\s", "", old_while) == re.sub(r"\s", "", line):
                    code_lines[i + 1] = ""
                    code_lines[i] = while_loop
                    break

    code = "\n".join(code_lines)
    return code


def correct_for_statements(code):
    """
    Corrects 'for' loops to use curly braces.

    Args:
        code (str): The input code.

    Returns:
        str: The corrected code.
    """
    pattern = r"\bfor\s*\(([^)]*)\)([^\n]*)\n(.*)"
    matches = re.finditer(pattern, code)
    code_lines = code.split("\n")

    for match in matches:
        condition = match.group(1)
        after_condition = match.group(2)
        loop_body = match.group(3)
        condition = "({}){}".format(condition, after_condition)
        old_for = "for {}".format(condition)

        for_loop = "for{}\n{{\n{}\n}}\n".format(condition, loop_body)

        # Skip cases where correction is unnecessary
        if not (";" in loop_body) or ("{" in condition):
            continue

        for i, line in enumerate(code_lines):
            if (re.sub(r"\s", "", old_for) == re.sub(r"\s", "", line)) and (re.sub(r"\s", "", loop_body) == re.sub(r"\s", "", code_lines[i + 1])):
                if (not ("{" in loop_body)):
                    code_lines[i + 1] = ""
                    code_lines[i] = for_loop
                    break

    code = "\n".join(code_lines)
    return code


def fix_single_variable_conditions(code):
    """
    Fixes 'if' statements with single variable conditions.

    Args:
        code (str): The input code.

    Returns:
        str: The corrected code.
    """
    pattern = r"\bif\s*\((.*?)\)(.*?)\n"
    matches = re.finditer(pattern, code)
    code_lines = code.split("\n")

    for match in matches:
        condition = match.group(1)
        statement = match.group(2)
        condition = "({})".format(condition)
        opening_brackets = 0
        closing_brackets = 0
        run = 0
        complete = 0
        save_index = 0
        skip = 0

        # Detecting if the condition is complete and balanced within parentheses
        while run < len(condition):
            if condition[run] == '(':
                opening_brackets += 1
            elif condition[run] == ')':
                closing_brackets += 1

                if closing_brackets == opening_brackets:
                    complete = 1
                    skip = 1
                    break
                else:
                    complete = 0
            run += 1

        if complete == 0:
            run = 0
            while run < len(statement):
                if statement[run] == '(':
                    opening_brackets += 1
                elif statement[run] == ')':
                    closing_brackets += 1
                    if closing_brackets == opening_brackets:
                        complete = 1
                        save_index = run + 1
                        break
                else:
                    complete = 0
                run += 1

            if (skip == 0):
                # Copy the desired part from statement
                copied_part = statement[0:save_index]
                condition += copied_part
                # Remove the copied part from the original statement
                statement = statement[:0] + statement[save_index:]

        old_if = "if {} {}".format(condition, statement)

        # Check if the condition contains a Boolean comparison operator
        if not (re.search(r"(==|!=|<|>|<=|>=|!)", condition)) or (re.search(r"(->)", condition) and not re.search(r"(==|!=|<=|>=|!)", condition)):
            # If the condition does not contain a comparison operator, assume it's a single variable and compare with 0
            condition = "{} != 0".format(condition)
        else:
            continue

        # Replace the original condition with the updated one
        updated_if = "if({}){}".format(condition, statement)
        for i, line in enumerate(code_lines):
            if re.sub(r"\s", "", old_if) == re.sub(r"\s", "", line):
                code_lines[i] = updated_if
                break

    code = "\n".join(code_lines)
    return code


def fix_elseif_single_variable_conditions(code):
    """
    Fixes single variable conditions in 'else if' statements.

    Args:
        code (str): The input code.

    Returns:
        str: The corrected code.
    """
    pattern = r"(.*?)\belse if\s*\((.*?)\)(.*\n)"
    matches = re.finditer(pattern, code)
    code_lines = code.split("\n")

    for match in matches:
        before_else = match.group(1)
        condition = match.group(2)
        statement = match.group(3)
        condition = "({})".format(condition)
        opening_brackets = 0
        closing_brackets = 0
        run = 0
        complete = 0
        save_index = 0
        skip = 0

        # Detecting if the condition is complete and balanced within parentheses
        while run < len(condition):
            if condition[run] == '(':
                opening_brackets += 1
            elif condition[run] == ')':
                closing_brackets += 1

                if closing_brackets == opening_brackets:
                    complete = 1
                    skip = 1
                    break
                else:
                    complete = 0
            run += 1

        if complete == 0:
            run = 0
            while run < len(statement):
                if statement[run] == '(':
                    opening_brackets += 1
                elif statement[run] == ')':
                    closing_brackets += 1
                    if closing_brackets == opening_brackets:
                        complete = 1
                        save_index = run + 1
                        break
                else:
                    complete = 0
                run += 1

            if (skip == 0):
                # Copy the desired part from statement
                copied_part = statement[0:save_index]
                condition += copied_part
                # Remove the copied part from the original statement
                statement = statement[:0] + statement[save_index:]

        old_if = "{}else if {} {}".format(before_else, condition, statement)

        # Check if the condition contains a Boolean comparison operator
        if not (re.search(r"(==|!=|<|>|<=|>=|!)", condition)) or (re.search(r"(->)", condition) and not re.search(r"(==|!=|<=|>=|!)", condition)):
            # If the condition does not contain a comparison operator, assume it's a single variable and compare with 0
            condition = "{} != 0".format(condition)
        else:
            continue

        # Replace the original condition with the updated one
        updated_if = "{}else if({}){}".format(
            before_else, condition, statement)
        for i, line in enumerate(code_lines):
            if re.sub(r"\s", "", old_if) == re.sub(r"\s", "", line):
                code_lines[i] = updated_if
                break

    code = "\n".join(code_lines)
    return code


def fix_void_return(code, snippets):
    """
    Fixes 'void' return statements.

    Args:
        code (str): The input code.
        snippets (list): List of dictionaries containing code snippets and error information.

    Returns:
        str: The corrected code.
    """
    code_lines = code.split("\n")

    for snippet in snippets:
        if ('MISRA-C_2012 17.7') in snippet['message']:
            if re.search(r'/\*.*?\*/|//', snippet['snippet'], flags=re.DOTALL):
                # Handle snippet with comments
                # Remove multi-line comments (/* ... */)
                snippet_without_multi_line_comments = re.sub(
                    r'/\*.*?\*/', '', snippet['snippet'], flags=re.DOTALL)
                # Remove single-line comments (// ...)
                void_line = re.sub(
                    r'//.*', '', snippet_without_multi_line_comments)
            else:
                void_line = snippet['snippet']

            # Check if the void_line contains certain keywords
            if not re.search(r"(if|for|else|while|addBitToStream)", void_line):
                if ("(void)" in snippet['snippet']):
                    continue
                updated_line = "(void)" + snippet['snippet']
                for i, line in enumerate(code_lines):
                    if re.sub(r"\s", "", snippet['snippet']) == re.sub(r"\s", "", line):
                        if ("\\" in updated_line):
                            updated_line = updated_line.replace("\n", "")
                        code_lines[i] = updated_line
                        break
            elif not re.search(r"(addBitToStream)", void_line):
                void_line, body = extract_body(snippet['snippet'])
                updated_line = "(void){}".format(body)
                if body is not None:
                    for i, line in enumerate(code_lines):
                        if re.sub(r"\s", "", body) == re.sub(r"\s", "", line):
                            code_lines[i] = updated_line
                            break

    code = "\n".join(code_lines)
    return code


def extract_body(code_snippet):
    """
    Extracts control statement and body content.

    Args:
        code_snippet (str): The code snippet to extract from.

    Returns:
        tuple: Control statement and body content.
    """

    # Regular expression to find the control statement (for/if/while/else) and the body content
    pattern = re.compile(
        r'((?:for|if|while|else)\s*\(.*?\))\s*({.*?}|[^{;]+;)', re.DOTALL)
    match = pattern.search(code_snippet)

    if match:
        control_statement = match.group(1)
        body = match.group(2)
        return control_statement, body
    else:
        return None, None


def fix_unsigned(code, snippets):
    """
    Fixes 'unsigned' types that should be explicitly specified as 'unsigned int'.

    Args:
        code (str): The input code.
        snippets (list): List of dictionaries containing code snippets and error information.

    Returns:
        str: The corrected code.
    """
    code_lines = code.split("\n")

    for snippet in snippets:
        if ('MISRA-C_2012 8.1') in snippet['message']:
            line = snippet['snippet']
            corrected_line = re.sub(
                r'\bunsigned\b(?! (char|int|long|float|double)\s)', 'unsigned int', line)
            for i, orig_line in enumerate(code_lines):
                if re.sub(r"\s", "", line) == re.sub(r"\s", "", orig_line):
                    code_lines[i] = corrected_line
                    break

    code = "\n".join(code_lines)
    return code


def fix_dead_code(code, snippets):
    """
    Fixes dead code violations by removing unnecessary '+0' additions.

    Args:
        code (str): The input code.
        snippets (list): List of dictionaries containing code snippets and error information.

    Returns:
        str: The corrected code.
    """
    code_lines = code.split("\n")

    for snippet in snippets:
        if 'MISRA-C_2012 2.2' in snippet['message']:
            line = snippet['snippet']
            corrected_line = re.sub(r'\+\s*0', '', line)

            for i, orig_line in enumerate(code_lines):
                if re.sub(r"\s", "", line) == re.sub(r"\s", "", orig_line):
                    code_lines[i] = corrected_line
                    break

    code = "\n".join(code_lines)
    return code

# --------------------------------------------------------
# Functions auto ident C code
# --------------------------------------------------------


def auto_indent_c_files(file_path):
    """
    Auto indents a C code file using clang-format with Microsoft style.

    Args:
        file_path (str): Path to the C code file.

    Returns:
        None
    """
    # Run clang-format with Microsoft style and inplace modification of the file
    subprocess.run(['clang-format', '-style=microsoft', '-i', file_path])


# --------------------------------------------------------
# Aux functions to get folders directory
# --------------------------------------------------------

def get_folder_name(directory_path):
    """
    Gets the name of the last folder in a directory path.

    Args:
        directory_path (str): Path to the directory.

    Returns:
        str: Name of the last folder in the directory path.
    """
    # Extract the name of the last folder in the directory path using os.path.basename
    last_folder_name = os.path.basename(directory_path)

    return last_folder_name


# --------------------------------------------------------
# Functions to build the Interface
# --------------------------------------------------------

def choose_directory(directory_entry):
    """
    Opens a directory selection dialog and updates the directory entry widget.

    Args:
        directory_entry (tk.Entry): Entry widget to display the selected directory path.
    """
    try:
        directory_path = filedialog.askdirectory()
        if directory_path:
            relative_path = os.path.relpath(directory_path, os.getcwd())
            print("Selected directory:", relative_path)
            directory_entry.delete(0, tk.END)
            directory_entry.insert(0, relative_path)
    except Exception as e:
        print("Error while choosing directory:", str(e))
        messagebox.showerror(
            "Error", "An error occurred while choosing the directory.")


def choose_report_file(entry):
    try:
        file_path = filedialog.askopenfilename(
            filetypes=[("XML Files", "*.xml")])
        if file_path:
            relative_path = os.path.relpath(file_path, os.getcwd())
            entry.delete(0, tk.END)
            entry.insert(0, relative_path)
        print("Selected XML report:", entry.get())
    except Exception as e:
        print("Error while choosing XML report:", str(e))
        messagebox.showerror(
            "Error", "An error occurred while choosing the XML report file.")


def choose_files(src_files_list_entry):
    try:
        files = filedialog.askopenfilenames(
            title="Select Files or Folder", filetypes=(("All Files", "*.*"), ("H Files", "*.h"), ("CC Files", "*.cc"), ("C Files", "*.c")))
        if files:
            relative_paths = [os.path.relpath(
                file_path, os.getcwd()) for file_path in files]
            src_files_list_entry.delete(0, tk.END)
            src_files_list_entry.insert(0, " ".join(relative_paths))
        print("Selected files/folder:", src_files_list_entry.get())
    except Exception as e:
        print("Error while choosing files:", str(e))
        messagebox.showerror(
            "Error", "An error occurred while choosing the directory.")


def open_readme():
    webbrowser.open(
        "https://$(AIR_GIT_REMOTE_HOSTNAME)/AIR/misraivoso/-/blob/main/README.md")


def open_html_report():
    report_path = "output/html/index.html"  # Update the path to the actual report
    webbrowser.open(report_path)


def add_misra_comments_to_source(code_snippets, progress_bar, flags):
    """
    Adds MISRA violation comments to the source code.

    Args:
        code_snippets (list): List of dictionaries containing code snippets and error information.
    """
    j = 15

    for snippet in code_snippets:
        j = j + (100-15)/len(code_snippets)
        if (flags == 0):
            progress_bar["value"] = j
            progress_bar.update()
        file_path = snippet['file']
        rule_id = snippet['message']  # Assuming message contains the rule ID

        with open(file_path, 'r', encoding='utf-8', errors='replace') as file:
            lines = file.readlines()

        if "\n" in snippet['snippet']:

            violation_comment = f"\n// ----------------------Misraivoso----------------------\n     \
            // Code to fix: {snippet['snippet']}                                                    \
            // MISRA violation: {rule_id}\n                                                         \
            // ------------------------------------------------------\n"                            \
            + snippet['snippet']

        else:
            violation_comment = f"\n// ----------------------Misraivoso----------------------\n \
            // Code to fix: {snippet['snippet']}\n                                              \
            // MISRA violation: {rule_id}\n                                                     \
            // ------------------------------------------------------\n"                        \
            + snippet['snippet']

        for i, line in enumerate(lines):
            if re.sub(r"\s", "", snippet['snippet']) == re.sub(r"\s", "", line):
                if not "MISRA violation:" in lines[i-2]:
                    lines[i] = violation_comment
                    break

        with open(file_path, 'w', encoding='utf-8', errors='replace') as file:
            file.writelines(lines)
        auto_indent_c_files(file_path)
    return


def remove_misra_comments_from_source(code_snippets, progress_bar, flags):
    """
    Removes MISRA violation comments from the source code.

    Args:
        file_path (str): Path to the source code file.
    """
    file_map_source = {}
    for snippet in code_snippets:
        file_path = snippet['file']
        if file_path not in file_map_source:
            file_map_source[file_path] = None
    j = 15
    for file_path in file_map_source:
        j = j + (100-15) / len(file_map_source)
        if (flags == 0):
            progress_bar["value"] = j
            progress_bar.update()
        with open(file_path, 'r', encoding='utf-8', errors='replace') as file:
            lines = file.readlines()

        updated_lines = []
        skip_lines = False
        skip = 0
        for i, line in enumerate(lines):

            if skip_lines == False:
                if "// ----------------------Misraivoso----------------------" in lines[i]:
                    if "// Code to fix:" in lines[i+1]:
                        k = i
                        while not "// -----------------------------------------------" in lines[k]:
                            k = k+1
                        skip = k+1
                        skip_lines = True

            if i == skip:
                skip_lines = False

            if skip_lines == True:
                continue
            updated_lines.append(line)

        with open(file_path, 'w', encoding='utf-8', errors='replace') as file:
            file.writelines(updated_lines)

        auto_indent_c_files(file_path)


def print_progress_bar(iteration, total, length=50):
    percent = ("{0:.1f}").format(100 * (iteration / float(total)))
    filled_length = int(length * iteration // total)
    bar = '#' * filled_length + '-' * (length - filled_length)
    print(f'\r[{bar}] {percent}% Complete', end='', flush=True)


def _configure_button(button, bg_color):
    """
    Configure the appearance of a button.
    """
    button.config(
        borderwidth=4,
        bg=bg_color,
        relief=tk.RAISED,
        highlightthickness=2,
        highlightbackground="red",
        highlightcolor="red",
        width=20,
        fg="white"
    )


def block_button(gui_correction, run_button, directory_entry, xml_report_entry, specific_directory_entry, include_comments_var, delete_comments_var, src_files_list_entry, progress_bar, progress, flags):

    if gui_correction == 1:
        run_correction(directory_entry.get(),
                       xml_report_entry.get(), progress, flags)
        run_button.config(state='disabled')
    elif gui_correction == 0:
        run_shell_script(directory_entry.get(), specific_directory_entry.get(), progress_bar,
                         include_comments_var.get(), delete_comments_var.get(), src_files_list_entry.get(), flags)
        run_button.config(state='normal')


def run_gui(flags):
    """
    Create and display the main graphical user interface (GUI) window for the Code Correction Tool.
    """
    # Create the GUI window
    root = tk.Tk()
    root.title("MISRAivoso")
    root.configure(bg="#e6e6e6")
    window_width = 1550
    window_height = 850
    root.geometry(f"{window_width}x{window_height}")
    # Set the minimum size for the window
    root.minsize(1500, 850)  # Adjust the minimum dimensions as needed

    # Configure row and column properties to center the content
    for i in range(13):  # Update the range based on the number of rows
        root.grid_rowconfigure(i, weight=1)
    for i in range(3):  # Update the range based on the number of column
        root.grid_columnconfigure(i, weight=1)
    gui_correction = 0
    # Load and set the GUI icon
    icon_file_name = "misra_fixer_icon.gif"
    script_directory = os.path.dirname(os.path.abspath(__name__))
    icon_path = os.path.join(script_directory, icon_file_name)
    img = tk.PhotoImage(file=icon_path)
    root.tk.call("wm", "iconphoto", root._w, img)

    # Custom color for button backgrounds
    custom_color = "#bc1313"  # Custom color (replace with desired color)

    # Directory Selection Section
    directory_label = tk.Label(
        root, text="Select Source Directory:", pady=10, bg="#e6e6e6")
    directory_entry = tk.Entry(root, width=50, bg="#ffe2e2")
    choose_directory_button = tk.Button(
        root, text="Choose Directory", command=lambda: choose_directory(directory_entry))
    _configure_button(choose_directory_button, custom_color)

    # File Selection Section
    src_files_list_label = tk.Label(
        root, text="Select Source Directory or Files:", pady=10, bg="#e6e6e6")
    src_files_list_entry = tk.Entry(root, width=50, bg="#ffe2e2")
    src_files_list_button = tk.Button(
        root, text="Choose Files", command=lambda: choose_files(src_files_list_entry))
    _configure_button(src_files_list_button, custom_color)

    # Directory Selection Section
    specific_directory_label = tk.Label(
        root, text="Select Source Directory:", pady=10, bg="#e6e6e6")
    specific_directory_entry = tk.Entry(root, width=50, bg="#ffe2e2")
    choose_specific_directory_button = tk.Button(
        root, text="Choose Directory 2", command=lambda: choose_directory(specific_directory_entry))
    _configure_button(choose_specific_directory_button, custom_color)

    progress_bar = ttk.Progressbar(
        root, orient="horizontal", length=200, mode="determinate")

    run_shell_script_button = tk.Button(
        root, text="Run Check and Report", command=lambda: block_button(0, run_button, directory_entry, xml_report_entry, specific_directory_entry, include_comments_var, delete_comments_var, src_files_list_entry, progress_bar, progress, flags))
    _configure_button(run_shell_script_button, "#c81010")

    # Create a label as a separation title
    separation_label_1 = tk.Label(
        root, text="Check and Report Section", bg="#e6e6e6", font=("Helvetica", 12, "bold"))
    separation_label_1.grid(row=0, column=1, padx=20, pady=10)

    run_shell_script_button.grid(
        row=4, column=1, padx=20, pady=10)
    progress_bar.grid(row=6, column=1, padx=20, pady=10)

    # XML Report File Selection Section
    xml_report_label = tk.Label(
        root, text="Select XML Report:", pady=10, bg="#e6e6e6")
    xml_report_entry = tk.Entry(root, width=50, bg="#ffe2e2")
    choose_xml_report_button = tk.Button(
        root, text="Choose XML Report", command=lambda: choose_report_file(xml_report_entry))
    _configure_button(choose_xml_report_button, custom_color)

    # Create a progress bar
    progress = ttk.Progressbar(
        root, orient="horizontal", length=300, mode="determinate")
    progress.grid(row=12, column=1,
                  padx=20, pady=20)  # Placed at the end

    # Run Correction Button Section
    run_button = tk.Button(root, text="Run Correction", command=lambda: block_button(1, run_button, directory_entry, xml_report_entry,
                           specific_directory_entry, include_comments_var, delete_comments_var, src_files_list_entry, progress_bar, progress, flags))
    _configure_button(run_button, "#c81010")

    # Open HTML Report Button Section
    open_report_button = tk.Button(
        root, text="Open HTML Report", command=open_html_report)
    _configure_button(open_report_button, "#c81010")
    open_report_button.grid(row=5, column=1, padx=20, pady=10)

    # Create a label as a separation title
    separation_label = tk.Label(
        root, text="Correction Section", bg="#e6e6e6", font=("Helvetica", 12, "bold"))
    separation_label.grid(row=7, column=1, padx=20, pady=10)

    # Organize widgets using grid layout
    directory_label.grid(row=1, column=0, sticky="nsew", padx=20, pady=10)
    directory_entry.grid(row=1, column=1, padx=20, pady=10)
    choose_directory_button.grid(row=1, column=2, padx=20, pady=10)

    src_files_list_label.grid(row=2, column=0, sticky="nsew", padx=20, pady=10)
    src_files_list_entry.grid(row=2, column=1, padx=20, pady=10)
    src_files_list_button.grid(row=2, column=2, padx=20, pady=10)

    specific_directory_label.grid(
        row=3, column=0, sticky="nsew", padx=20, pady=10)
    specific_directory_entry.grid(row=3, column=1, padx=20, pady=10)
    choose_specific_directory_button.grid(row=3, column=2, padx=20, pady=10)

    xml_report_label.grid(row=9, column=0, sticky="nsew", padx=20, pady=10)
    xml_report_entry.grid(row=9, column=1, padx=20, pady=10)
    choose_xml_report_button.grid(row=9, column=2, padx=20, pady=10)

    run_button.grid(row=10, column=1, padx=20, pady=20)

    # Check box to include comments
    include_comments_var = tk.IntVar()
    include_comments_checkbox = tk.Checkbutton(
        root, text="Include MISRA Comments", variable=include_comments_var, bg="#ffe2e2", font=("Helvetica", 12, "bold"))
    include_comments_checkbox.grid(
        row=5, column=2, padx=20, pady=20, sticky="nsew")

    # Check box to include comments
    delete_comments_var = tk.IntVar()
    delete_comments_checkbox = tk.Checkbutton(
        root, text="Delete MISRA Comments", variable=delete_comments_var, bg="#ffe2e2", font=("Helvetica", 12, "bold"))
    delete_comments_checkbox.grid(
        row=4, column=2, padx=20, pady=20, sticky="nsew")

    # Help Button Section
    help_button = tk.Button(root, text="Help", command=open_readme)
    _configure_button(help_button, "#c81010")  # Use a color of your choice
    help_button.grid(row=11, column=1, padx=20,
                     pady=10)  # Adjust row and column as needed
    # Define the steps
    steps = ["Step 1: Choose Directory to run check and report",
             "Step 2: Click in Choose Files if only specific files to check",
             "Step 3: Click in Choose Directory 2 if only specific directory to check",
             "Step 4: Run Check and Report",
             "Step 5: Choose XML Report, saved in output folder",
             "Step 6: Run Correction",
             "Step 7: Choose Corrected Directory to run check and report again",
             "Step 8: Run Check and Report on corrected directory",
             "Finish"]

    # Create a frame to hold the step buttons and information
    step_buttons_frame = tk.Frame(root, bg="#e6e6e6")
    step_buttons_frame.grid(
        row=14, column=1, padx=20, pady=10, sticky="nsew")
    step_buttons_frame.grid_rowconfigure(3, weight=1)
    step_buttons_frame.grid_columnconfigure(1, weight=1)
    # Create a frame for each step
    step_frames = []
    for step in steps:
        frame = tk.Frame(root)
        step_frames.append(frame)

    # Configure row and column properties for step frames
    for i, frame in enumerate(step_frames):
        frame.grid(row=0, column=1,
                   padx=20, pady=10, sticky="nsew")
        frame.grid_rowconfigure(0, weight=1)
        frame.grid_columnconfigure(3, weight=1)

        frame.grid_forget()  # Hide all frames initially

    def show_step(step_index):
        for i, frame in enumerate(step_frames):
            if i == step_index:
                frame.grid()
                current_step_label.config(text=steps[i])
            else:
                frame.grid_forget()

    def prev_step():
        current_step = steps.index(current_step_label.cget("text"))
        if current_step > 0:
            show_step(current_step - 1)

    def next_step():
        current_step = steps.index(current_step_label.cget("text"))
        if current_step < len(steps) - 1:
            show_step(current_step + 1)

    # Create navigation buttons and step information label
    prev_button = tk.Button(
        step_buttons_frame, text="Previous Step", command=prev_step)
    next_button = tk.Button(
        step_buttons_frame, text="Next Step", command=next_step)
    current_step_label = tk.Label(
        step_buttons_frame, text="", width=80, bg="#e6e6e6", font=("Helvetica", 12, "bold"))
    # Configure row and column properties for buttonsand label
    prev_button.grid(row=2, column=1,
                     padx=20, pady=10, sticky="nsew")
    current_step_label.grid(row=0, column=1, padx=30, pady=10, sticky="nsew")
    next_button.grid(row=1, column=1, padx=20, pady=10, sticky="nsew")
    # Show the initial step
    show_step(0)
    # Start the main event loop to display the GUI
    root.minsize(1500, 850)  # Adjust the minimum dimensions as needed
    root.mainloop()


# --------------------------------------------------------
# Main functions
# --------------------------------------------------------


def run_shell_script(directory_to_check, specific_directory, progress_bar, include_comments, delete_comments, src_file_list, flags):
    if not directory_to_check:
        messagebox.showerror("Error", "Please choose a directory to check.")
        return

    shell_script_path = "utils/misraivoso/run_cppcheck.py"

    if (flags == 0):
        # Update progress bar
        progress_bar["value"] = 15
        progress_bar.update()
        directory = directory_to_check

        if (src_file_list and specific_directory):
            shell_script_command = f"python3 {shell_script_path} {directory} --src-file-list '{src_file_list} {specific_directory}'"
        elif (src_file_list):
            shell_script_command = f"python3 {shell_script_path} {directory} --src-file-list '{src_file_list}'"
        elif (specific_directory):
            shell_script_command = f"python3 {shell_script_path} {directory} --src-file-list '{specific_directory}'"
        else:
            shell_script_command = f"python3 {shell_script_path} {directory} --src-file-list '{directory}'"
    else:
        directory = directory_to_check
        shell_script_command = f"python3 {shell_script_path} {directory} --src-file-list '{src_file_list}'"

    try:
        subprocess.run(shell_script_command, shell=True, check=True)
        if delete_comments == 1:
            xml_path = "utils/misraivoso/output/cppcheck_out.xml"
            snippets = extract_code_snippets(xml_path)
            if (flags == 0):
                progress_bar["value"] = 15
                progress_bar.update()
            remove_misra_comments_from_source(snippets, progress_bar, flags)
            if (flags == 0):
                messagebox.showinfo(
                    "Success", "Check and report executed successfully.")
            else:
                print("\nCheck and report executed successfully.\n")
        elif include_comments == 1:
            xml_path = "utils/misraivoso/output/cppcheck_out.xml"
            snippets = extract_code_snippets(xml_path)
            if (flags == 0):
                progress_bar["value"] = 15
                progress_bar.update()
            add_misra_comments_to_source(snippets, progress_bar, flags)
            if (flags == 0):
                messagebox.showinfo(
                    "Success", "\nCheck and report executed successfully.")
            else:
                print("\nCheck and report executed successfully.\n")
        else:
            if (flags == 0):
                progress_bar["value"] = 100
                progress_bar.update()
                messagebox.showinfo(
                    "Success", "Check and report executed successfully.")
            else:
                print("\nCheck and report executed successfully.\n")

    except subprocess.CalledProcessError as e:
        messagebox.showerror(
            "Error", f"An error occurred while running the shell script: {e}")


def run_correction(source_directory, report_file, progress, flags):
    """
    Run the code correction process based on user input.

    Args:
        directory_entry (tk.Entry): Entry widget containing the selected input directory.
        output_directory_entry (tk.Entry): Entry widget containing the selected output directory.
        xml_report_entry (tk.Entry): Entry widget containing the selected XML report file.
        root (tk.Tk): The root GUI window.

    Returns:
        None
    """

    # Check if required fields are provided
    if not source_directory:
        messagebox.showerror("Error", "Please choose a directory first.")
        return

    if not os.path.exists(source_directory):
        messagebox.showerror(
            "Error", "The selected source directory does not exist.")
        return

    if not report_file:
        messagebox.showerror("Error", "Please choose the XML report file.")
        return
    # Extract code snippets from the XML report
    snippets = extract_code_snippets(report_file)
    file_map_source = {}
    for snippet in snippets:
        file_path = snippet['file']
        if file_path not in file_map_source:
            file_map_source[file_path] = None
    # Get the total number of files to process
    total_files = len(file_map_source)

    if (flags == 0):

        progress["maximum"] = total_files
        progress.update()

    # Process each file
    for i, file in enumerate(file_map_source):
        if flags != 0:
            print_progress_bar(i + 1, total_files)
        auto_indent_c_files(os.path.join(file))
        with open(os.path.join(file), 'r', encoding='utf-8', errors='replace') as f:
            code = f.read()

        #####################################################################################
        # Functions to correct MISRA rules:
        # - To add more functions to correct MISRA is in this section, just pass the variable
        # "code" to the function and return it to the "corrected_code".
        # - "snippets" is a dictionary that have the contents of the XML report
        #####################################################################################
        corrected_code = fix_unsigned(code, snippets)
        corrected_code = correct_if_statements(corrected_code)
        corrected_code = correct_for_statements(corrected_code)
        corrected_code = correct_while_statements(corrected_code)
        corrected_code = correct_else_statements(corrected_code)
        corrected_code = fix_single_variable_conditions(corrected_code)
        corrected_code = fix_elseif_single_variable_conditions(corrected_code)
        corrected_code = fix_void_return(corrected_code, snippets)
        corrected_code = fix_dead_code(corrected_code, snippets)
        #####################################################################################

        # Modify the output directory in the path
        # directory = file.replace(input_folder_name, output_folder_name)
        directory = file
        # Write corrected code to output directory
        with open(os.path.join(directory), "w", encoding='utf-8') as f:
            f.write(corrected_code)

        auto_indent_c_files(os.path.join(directory))

        if (flags == 0):
            # Update progress bar
            progress["value"] = i + 1
            progress.update()

    if (flags == 0):
        # Display completion message
        messagebox.showinfo(
            "Success", "Correction completed for all files in the directory!")
    else:
        print("\nCorrection completed for all files in the directory!\n")


def run_with_flags(source_directory, include_comments, delete_comments, cppcheck, correction, src_file_list, flags):

    progress_bar = 0
    specific_directory = None
    xml_path = "utils/misraivoso/output/cppcheck_out.xml"
    # xml_path = "utils/misraivoso/output/cppcheck_out.xml"
    if cppcheck or correction:
        run_shell_script(source_directory, specific_directory, progress_bar,
                         include_comments, delete_comments, src_file_list, flags)

    if correction:
        print("MISRA Correction:\n")
        run_correction(source_directory,
                       xml_path, progress_bar, flags)


def main():

    parser = argparse.ArgumentParser(
        description="MISRAivoso Command Line Tool")
    parser.add_argument("--cppcheck", help="Run cppcheck", action="store_true")
    parser.add_argument(
        "--correction", help="Run correction", action="store_true")
    parser.add_argument("--src", help="Source directory")
    parser.add_argument('--src-file-list', default=None, type=str,
                        help='Folder or list of individual files inside the source directory to be checked')
    parser.add_argument("--include-comments",
                        help="Include MISRA Comments", action="store_true")
    parser.add_argument("--delete-comments",
                        help="Delete MISRA Comments", action="store_true")
    args = parser.parse_args()
    flags = 0
    if not args.src or not args.src_file_list:
        if args.include_comments or args.delete_comments or args.cppcheck or args.correction:
            print("Error: --src and --src-file-list flags are mandatory.")
            return
        else:
            # No flags provided, run without flags
            # Start the GUI in a separate thread
            gui_thread = Thread(target=run_gui(flags))
            gui_thread.start()

    else:
        # Flags provided, run with flags
        if not args.src or not args.src_file_list:
            print("Error: --src and --src-file-list flags are mandatory.")
            return

        if args.cppcheck and args.correction:
            print(
                "Error: Choose one of the options: --cppecheck or --correction, can't be both")
            return

        if not (args.cppcheck or args.correction):
            print(
                "Error: Choose one of the options: --cppecheck or --correction, can't be both")
            return

        if args.correction and (args.include_comments or args.delete_comments):
            print("Error: Include and delete comments only work with: --cppcheck")
            return

        flags = 1
        run_with_flags(args.src, args.include_comments, args.delete_comments,
                       args.cppcheck, args.correction, args.src_file_list, flags)


if __name__ == "__main__":
    main()
