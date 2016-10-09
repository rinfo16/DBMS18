#!/bin/bash
cur_dir=$(pwd)
scrip_dir=$(cd `dirname $0`; pwd)
mkdir -p $scrip_dir/../build
cmake -H$scrip_dir/../src -B$scrip_dir/../build

