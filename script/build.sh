#!/bin/bash
cur_dir=$(pwd)
scrip_dir=$(cd `dirname $0`; pwd)
cmake -H$scrip_dir/../src -B$scrip_dir/../build

