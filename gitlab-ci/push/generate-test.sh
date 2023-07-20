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
        - export GIT_SSL_NO_VERIFY=1

        - git config --global credential.helper store
        - echo "https://huca:\${CI_JOB_TOKEN}@$(AIR_GIT_REMOTE_HOSTNAME)" > ~/.git-credentials

        - if ! test -f git-clean.lock; 
            then 
                touch git-clean.lock; 
            fi

        - if ! \$(grep -Fxq \$CI_COMMIT_SHORT_SHA git-clean.lock); 
            then 
                git clean -ffdx && git submodule foreach --recursive git clean -ffdx && echo \$CI_COMMIT_SHORT_SHA > git-clean.lock; 
                git submodule update --init --recursive; 
                git submodule sync --recursive; 
            fi
            
        - if ! test -d \$STATE_FOLDER; 
            then 
                (mkdir -p \$STATE_FOLDER || true) && ./utils/gitlab-runner/check_central_server.bash; 
            else 
                ./utils/gitlab-runner/check_central_server.bash;
            fi;

        - while test -e /tmp/rsync-"\$CI_COMMIT_SHORT_SHA".lock; 
            do sleep 1; 
            done;        

        - cd \$STATE_FOLDER
        - cd air
        
        - export AIR=\$STATE_FOLDER/air/
        - export PATH=\$PATH:\$AIR
        - export AIR_INSTALL=\$AIR/install
        - export AIR_PMK=\$AIR_INSTALL/pmk
        - export AIR_POS=\$AIR_INSTALL/pos

        - export RTEMS410=/opt/rtems-4.10/bin
        - export PATH=\$PATH:\$RTEMS410
        - export RTEMS_MAKEFILE_PATH=\$AIR_POS/rtems5/rtems5-install/sparc-rtems5/gr740
        - export PATH=\$PATH:/opt/gcc-arm-9.2-2019.12-x86_64-arm-none-eabi/bin

        - export DISPLAY=:99
        - export PATH=/opt/rtems/5/bin:\$PATH
        - export PATH=/opt/rtems-5.1-2019.07.25/bin:\$PATH
        - export PATH=/opt/laysim-gr740:\$PATH

        - export UTILS=\$AIR/../utils/gitlab-runner

.build-executable: &build-executable
    - \$AIR/configure
    - make clean
    - make
    - if ! test -f ./executable/AIRAPP.exe; then echo "Executable doens\'t exist." && exit 1; fi

.check-and-publish: &check-and-publish
    - \$AIR/../utils/gitlab-runner/testcheck.py
    - \$UTILS/publish_example.bash

.ARM-QEMU-validation-template:
  tags:  ["ARM"]
  script: 
      - cd \$AIR/examples/private-example/private/validation/TEST-DEF-$TEST_NUMBER
      - *build-executable
      - (\$UTILS/rvs_arm_coverage-ci.bash | tee testresult.txt) || true
      - *check-and-publish
  rules:
      - if: '\$CI_COMMIT_MESSAGE =~ /^\[ARM\]/'
      - if: '\$CI_COMMIT_MESSAGE !~ /^\[SPARC\]/'

.SPARC-LAYSIM-validation-template:
  tags:  ["SPARC","LAYSIM"]
  script: 
        - echo "The value of TEST_NUMBER is $TEST_NUMBER"
        - cd \$AIR/examples/private-example/private/validation/TEST-DEF-$TEST_NUMBER
        - *build-executable
        - laysim-gr740-mmu-cli -batch \$AIR/rvs_air/scripts/laysim_cmds.txt | tee testresult.txt
        - make distclean -i # Ignore distclean errors
        - *check-and-publish
  rules:
        - if: '\$CI_COMMIT_MESSAGE =~ /^\[SPARC\]/'

ARM-QEMU-TEST-DEF-$TEST_NUMBER:
    extends: .ARM-QEMU-validation-template  

SPARC-LAYSIM-TEST-DEF-$TEST_NUMBER:
    extends: .SPARC-LAYSIM-validation-template

EOF