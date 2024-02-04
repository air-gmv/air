#!/usr/bin/env python3
import argparse
import subprocess
import os
    

def list_tests(folder_path):
    """List available tests (subfolders) in the specified folder."""
    tests = [test for test in os.listdir(folder_path) if (os.path.isdir(os.path.join(folder_path, test)) and not "old" in test)]
    return tests

def run_test(test_name, recompile_air=False):
    """Run the bash script for the selected test."""

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
    r = subprocess.run(['configure'], cwd=os.environ['AIR'] + "examples/private-example/" + test_name, capture_output=False)
    if r.returncode != 0:
        print("Configure example failed")
        return r.returncode

    r = subprocess.run(['make', 'clean'], cwd=os.environ['AIR'] + "examples/private-example/" + test_name, capture_output=False)

    r = subprocess.run(['make'], cwd=os.environ['AIR'] + "examples/private-example/" + test_name, capture_output=False)
    if r.returncode != 0:
        print("Example build failed ")
        return r.returncode
            
    # Lock the board file
    if os.system(f"lockfile -r 0 /var/lock/ultrascale.lock || exit 1") != 0:
        print("Board's lockfile is locked. Exiting...")
        return
    
    #Start putty
    putty = subprocess.Popen(["putty", f"/dev/UltraScale_uart", "-serial", "-sercfg", "115200,8,n,1,N"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    os.environ['APP'] = os.environ['AIR'] + "examples/private-example/" + test_name + "/executable/AIRAPP.exe"

    os.system(f"xsct -interactive {os.environ['AIR']}../utils/hw_scripts/Ultrascale/launch_a53.tcl {'1' if test_name == 'T0020_dual' else '0'}")
    if r.returncode != 0:
        print("Example failed")
        return r.returncode

    os.system("pkill putty")

    # Unlock the board file
    try:
        os.remove(f"/var/lock/ultrascale.lock")
    except FileNotFoundError:
        pass

    return 0

def main():
    parser = argparse.ArgumentParser()

    group = parser.add_mutually_exclusive_group(required=False)
    group.add_argument('-t', '--test', help='Run the specified test')
    group.add_argument('-a', '--all_tests', action='store_true', help='Run all tests')
    parser.add_argument('-l', '--list_tests', action='store_true', help='List available tests')

    test_folder = "/home/ansi/AIR_ULTRASCALE/2peruair/air/examples/private-example/AIR_testsuite"  # Update with the actual path to your test folder
    
    # List available tests (subfolders)
    available_tests = list_tests(test_folder)
    available_tests.sort()

    if not available_tests:
        print("No tests found in the specified folder.")
        return

    args = parser.parse_args()

    if args.list_tests:
        for i, test in enumerate(available_tests, start=1):
            print(f"{i}: {test}")
        exit()

    if args.all_tests:
        for t in available_tests:
            print(f"----------Running test {t}----------")
            if run_test(t) != 0:
                print(f"Test {t} failed")
    elif args.test:
        try:
            run_test(available_tests[int(args.test)-1])
        except IndexError:
            print("Invalid test")
    else:
        # Prompt user to choose a test
        print("Available tests:")
        for i, test in enumerate(available_tests, start=1):
            print(f"{i}: {test}")

        while True:
            try:
                selected_index = int(input("Enter the number of the test to run: ")) - 1
                selected_test = available_tests[selected_index]

                # Run the selected test
                run_test(selected_test)

                exit()

            except (ValueError, IndexError):
                print("Invalid selection. Please enter a valid number.")

if __name__ == "__main__":
    main()
