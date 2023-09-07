#!/bin/bash

# Access the variable passed as command-line argument
TEST_NUMBER=$1

# Generate the YAML content
cat > test.yml <<EOF

default:
    retry: 1
    before_script:
        - sleep \$((\$RANDOM % 2)).\$RANDOM

        - export STATE_FOLDER=\$HOME/state/\$CI_COMMIT_SHA
        - export AIR=\$STATE_FOLDER/air/
        - export UTILS=\$AIR/../utils/gitlab-runner
            
        - if ! test -d \$STATE_FOLDER; then 
            (mkdir -p \$STATE_FOLDER || true)
          fi;
        - ./utils/gitlab-runner/check_central_server.bash;
            
        - cd \$STATE_FOLDER
        - cd air
    
.build-executable: &build-executable
    - \$AIR/configure
    - make clean -i
    - make -i
    - if ! test -f ./executable/AIRAPP.exe; then echo "Executable does not exist." && exit 1; fi

.check-and-publish: &check-and-publish
    - \$AIR/../utils/gitlab-runner/testcheck.py
    - \$UTILS/publish_example.bash

ARM-QEMU-TEST-DEF-$TEST_NUMBER:
  tags: ["ARM"]
  script: 
      - cd \$AIR/examples/private-example/private/validation/TEST-DEF-$TEST_NUMBER
      - *build-executable
      - (\$UTILS/rvs_arm_coverage-ci.bash | tee testresult.txt) || true
      - *check-and-publish
  rules:
      - if: '\$CI_COMMIT_MESSAGE =~ /^\[ARM\]/'
      - if: '\$CI_COMMIT_MESSAGE !~ /^\[SPARC\]/'

SPARC-LAYSIM-TEST-DEF-$TEST_NUMBER:
  tags: ["SPARC","LAYSIM"]
  script: 
        - cd \$AIR/examples/private-example/private/validation/TEST-DEF-$TEST_NUMBER
        - *build-executable
        - laysim-gr740-mmu-cli -batch \$AIR/rvs_air/scripts/laysim_cmds.txt | tee testresult.txt
        - make distclean -i # Ignore distclean errors
        - *check-and-publish
  rules:
        - if: '\$CI_COMMIT_MESSAGE =~ /^\[SPARC\]/'

EOF