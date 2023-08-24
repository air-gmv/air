#!/bin/bash

# Specify your .gitlab-ci.yml file
CI_YAML_FILE="new.yml"

tests=(
  	"00009:false"
	"00010:false"
	"00012:false"
	"00021:false"
	"00022:false"
	"00023:true"
	"00500:true"
	"00510:true" 
	"00540:false"
	"00560:false"
	"00600:true"
	"01200:true"
	"01210:true"
	"01390:true"
	"01570:false"
	"01590:false"
	"01610:false"
	"01620:true"
	"01630:true"
	"01650:true"
	"01730:true"
	"01740:true"
	"01741:true"
	"01742:true"
	"01750:true"
	"02100:false"
	"80000:false"
	"80010:false"
	"80060:true"
	"80080:false"
	"80300:false"
	"80400:false"
	"80401:false"
	"80420:false"
)

# Read test numbers from the array and generate jobs
for test in "${tests[@]}"; do
	number=$(echo "$test" | cut -d':' -f1)
  	active=$(echo "$test" | cut -d':' -f2)

	if [ "$active" == "true" ]; then
		# Append the generated job definition to the existing YAML file
		echo "" >> $CI_YAML_FILE
		echo "ARM-QEMU-TEST-DEF-$number:" >> $CI_YAML_FILE
		echo "  extends: .ARM-QEMU-validation-template" >> $CI_YAML_FILE
		echo "  variables:" >> $CI_YAML_FILE
		echo "    EXAMPLE: \"TEST-DEF-$number\"" >> $CI_YAML_FILE
		echo "" >> $CI_YAML_FILE
	fi

done
