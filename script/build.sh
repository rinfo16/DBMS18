#!/bin/bash

CMAKE_BUILD_TYPE=Debug
COVERAGE=0
CMAKE_ARGS=""

while [ -n "$*" ]
do
  case "$1" in
    -debug)
      CMAKE_ARGS+=" -DCMAKE_BUILD_TYPE=Debug"
      ;;
    -release)
      CMAKE_ARGS+=" -DCMAKE_BUILD_TYPE=Release"
      ;;
    -coverage)
      CMAKE_ARGS+=" -DENABLE_GCOV=1"
      ;;
    -help|*)
      echo "help ... TODO ...."
      exit 1
      ;;
    esac
  shift;
done

cur_dir=$(pwd)
scrip_dir=$(cd `dirname $0`; pwd)
mkdir -p $scrip_dir/../build
cmake -H$scrip_dir/../src -B$scrip_dir/../build ${CMAKE_ARGS}

