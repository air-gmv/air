#!/bin/bash

# Specify the file containing test numbers
TEST_NUMBERS_FILE="test_numbers.yml"

# Specify your .gitlab-ci.yml file
CI_YAML_FILE="../.gitlab-ci-validation.yml"

line_number=0
increment=3

# Read test numbers from the YAML file and generate jobs
while IFS= read -r line; do
	((line_number++))
	
	if [ "$((line_number % increment))" -eq 1 ]; then
		number="$(echo "$line" | grep -oE 'number: "([0-9]+)"' | grep -oE '[0-9]+')"
	fi

	if [ "$((line_number % increment))" -eq 2 ]; then
		active="$(echo "$line" | grep -oE 'active: (true|false)' | grep -oE '(true|false)')"

		if [ "$active" == "true" ]; then
			# Append the generated job definition to the existing YAML file
			echo "" >> $CI_YAML_FILE
			echo "ARM-QEMU-TEST-DEF-$number:" >> $CI_YAML_FILE
			echo "  extends: .ARM-QEMU-validation-template" >> $CI_YAML_FILE
			echo "  variables:" >> $CI_YAML_FILE
			echo "    EXAMPLE: \"TEST-DEF-$number\"" >> $CI_YAML_FILE
			echo "" >> $CI_YAML_FILE
		fi

	fi

done < "$TEST_NUMBERS_FILE"
