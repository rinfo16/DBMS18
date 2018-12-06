#!/bin/bash

BUILD_TYPE=Debug
COVERAGE=0
CMAKE_ARGS=""
MAKE=0

while [ -n "$*" ]
do
  case "$1" in
    -debug)
      BUILD_TYPE="Debug"
      CMAKE_ARGS+=" -DCMAKE_BUILD_TYPE=Debug"
      ;;
    -release)
      BUILD_TYPE="Release"
      CMAKE_ARGS+=" -DCMAKE_BUILD_TYPE=Release"
      ;;
    -coverage_debug)
      BUILD_TYPE="CoverageDebug"
      CMAKE_ARGS+=" -DCMAKE_BUILD_TYPE=CoverageDebug"
      ;;
    -coverage_release)
      BUILD_TYPE="CoverageRelease"
      CMAKE_ARGS+=" -DCMAKE_BUILD_TYPE=CoverageRelease"
      ;;
    -make)
      MAKE=1
      ;;
    -help|*)
      echo "help ... "
      echo "  build.sh [ARGS]"
      echo "    -debug"
      echo "    -release"
      echo "    -coverage_debug"
      echo "    -coverage_release"
      echo "    -make"
      exit 1
      ;;
    esac
  shift;
done

CURRENT_DIR=$(pwd)
SCRIPT_DIR=$(cd `dirname $0`; pwd)
cd $SCRIPT_DIR
cd ../
PROJECT_DIR=$(pwd)
cd $CURRENT_DIR
cmake -H$PROJECT_DIR/src -B$PROJECT_DIR/build ${CMAKE_ARGS}
if [[ $MAKE -eq 1 ]] ; then
  cd $PROJECT_DIR/build
  make
  if [[ $BUILD_TYPE = "CoverageDebug" ]]; then
    rm /tmp/owl.data.*
    nohup make owl_coverage &
    sleep 4
    bash $SCRIPT_DIR/test.sh > test_out.txt
    pkill -3 DBMS18
  fi
  cd $CURRENT_DIR
fi
