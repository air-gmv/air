#!/usr/bin/env python3
import argparse
import subprocess
import os
import time 

# Relative to $AIR
# EXAMPLES_FOLDER = "/examples/private-example/AIR_testsuite/Validation_tests/BARE/"
EXAMPLES_FOLDER = "/examples/private-example/AIR_testsuite/Unit_tests/" 

def list_examples(folder_path):
    """List available examples (subfolders) in the specified folder."""
    examples = [example for example in os.listdir(folder_path) if (os.path.isdir(os.path.join(folder_path, example)) and not "old" in example and not example.startswith("T1"))]
    return examples

def run_example(example_name, recompile_air=False):
    """Run the bash script for the selected example."""

    ## Recompile AIR
    if recompile_air:
        r = subprocess.run(['make', 'clean'], cwd=os.environ['AIR'], capture_output=True)
        if r.returncode != 0:
            print("make clean failed")
            return r.returncode
        
        r = subprocess.run(['make'], cwd=os.environ['AIR'], capture_output=True)
        if r.returncode != 0:
            print("make failed")
            return r.returncode
    
    ## Build the example
    r = subprocess.run(['configure'], cwd=os.environ['AIR'] + EXAMPLES_FOLDER + example_name, capture_output=False)
    if r.returncode != 0:
        print("Configure example failed")
        return r.returncode

    r = subprocess.run(['make', 'clean'], cwd=os.environ['AIR'] + EXAMPLES_FOLDER + example_name, capture_output=False)

    r = subprocess.run(['make'], cwd=os.environ['AIR'] + EXAMPLES_FOLDER + example_name, capture_output=False)
    if r.returncode != 0:
        print("Example build failed ")
        return r.returncode
            
    # Lock the board file
    if os.system(f"lockfile -r 0 /var/lock/ultrascale.lock || exit 1") != 0:
        print("Board's lockfile is locked. Exiting...")
        return
    
    #Start putty
    #remove putty log before starting
    try:
        os.remove(f"putty_{example_name}.log")
    except FileNotFoundError:
        pass

    putty = subprocess.Popen(["putty", f"/dev/UltraScale_uart", "-serial", "-sercfg", "115200,8,n,1,N", "-sessionlog", f"putty_{example_name}.log"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    os.environ['APP'] = os.environ['AIR'] + EXAMPLES_FOLDER + example_name + "/executable/AIRAPP.exe"

    # Core number for this test
    core_number = 1
    if example_name.startswith("T002"):
        if "quad_core" in example_name:
            core_number = 4
        else:
            core_number = 2

    os.system(f"xsct -interactive {os.environ['AIR']}../utils/hw_scripts/Ultrascale/launch_a53.tcl {core_number}")
    
    rc = -1
    try:
        with open(f"putty_{example_name}.log", "r") as f:
            log = f.read()
            if "END OF TEST ALL PASS" in log:
                rc = 0
            else:
                rc = 1

    except FileNotFoundError:
        while(1):
            pass
        pass
    os.system("pkill putty")

    # Unlock the board file
    try:
        os.remove(f"/var/lock/ultrascale.lock")
    except FileNotFoundError:
        pass

    return rc

def main():
    parser = argparse.ArgumentParser()

    group = parser.add_mutually_exclusive_group(required=False)
    group.add_argument('-e', '--example', help='Run the specified test')
    group.add_argument('-a', '--all_examples', action='store_true', help='Run all examples')
    parser.add_argument('-l', '--list_examples', action='store_true', help='List available examples')

    test_folder = f"{os.environ['AIR']}{EXAMPLES_FOLDER}"
    
    # List available examples (subfolders)
    available_examples= list_examples(test_folder)
    available_examples.sort()

    if not available_examples:
        print("No tests found in the specified folder.")
        return

    args = parser.parse_args()

    if args.list_examples:
        for i, test in enumerate(available_examples, start=1):
            print(f"{i}: {test}")
        exit()

    if args.all_examples:
        success = []
        failed = []
        start = time.time() 
        for t in available_examples:
            print(f"----------Running example {t}----------")

            rc = run_example(t)

            if rc != 0:
                failed.append(t)
            else:
                success.append(t)
        print(f"Took {time.time() - start} seconds")
        print(f"Passed ({len(success)}): {success}")
        print(f"Failed ({len(failed)}): {failed}")
    elif args.example:
        try:
            run_example(available_examples[int(args.example)-1])
        except IndexError:
            print("Invalid test")
    else:
        # Prompt user to choose a test
        print("Available tests:")
        for i, ex in enumerate(available_examples, start=1):
            print(f"{i}: {ex}")

        while True:
            try:
                selected_index = int(input("Enter the number of the test to run: ")) - 1
                selected_example = available_examples[selected_index]

                run_example(selected_example)

                exit()

            except (ValueError, IndexError):
                print("Invalid selection. Please enter a valid number.")

if __name__ == "__main__":
    main()
