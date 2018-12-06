#!/bin/bash
source_root=$(cd "$(dirname "$0")"; cd ..; pwd)
echo ${source_root}
export PATH=${PATH}:${source_root}/src/third_party/bin:${HOME}/local/bin
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${source_root}/src/third_party/lib/
export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH