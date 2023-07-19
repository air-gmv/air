#!/bin/bash

# Access the variables passed as command-line arguments
AIR=$1
UTILS=$2
ARCH=$3
TEST_NUMBER=$4

# Generate the YAML content
cat > test.yml <<EOF
.ARM-QEMU-validation-template:
    stage: validation_tests
    needs: ["ARM-QEMU-hello_world","ARM-QEMU-bare_c"]
    tags:  ["ARM"]
    script: 
        - cd $AIR/examples/private-example/private/validation/TEST-DEF-$TEST_NUMBER
        - *build-executable
        - ($UTILS/rvs_arm_coverage-ci.bash | tee testresult.txt) || true
        - *check-and-publish
    rules:
        - if: "$ARCH == "ARM""

.SPARC-LAYSIM-validation-template:
  stage: validation_tests
  needs: ["SPARC-LAYSIM-hello-world"] # ,"SPARC-LAYSIM-bare_c","SPARC-LAYSIM-hm","SPARC-LAYSIM-ports","SPARC-LAYSIM-shm","SPARC-LAYSIM-paranoia","SPARC-LAYSIM-smp01","SPARC-LAYSIM-smp02"]
  tags:  ["SPARC","LAYSIM"]
  script: 
        - echo "The value of TEST_NUMBER is $TEST_NUMBER"
        - cd $AIR/examples/private-example/private/validation/TEST-DEF-$TEST_NUMBER
        - *build-executable
        - laysim-gr740-mmu-cli -batch ../../../../../rvs_air/scripts/laysim_cmds.txt | tee testresult.txt
        - make distclean -i # Ignore distclean errors
        - *check-and-publish
  rules:
        - if: "$ARCH == "SPARC""

ARM-QEMU-TEST-DEF-$TEST_NUMBER:
    extends: .ARM-QEMU-validation-template  

SPARC-LAYSIM-TEST-DEF-$TEST_NUMBER:
    extends: .SPARC-LAYSIM-validation-template

EOF