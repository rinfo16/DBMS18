#!/bin/bash
source_root=$(cd "$(dirname "$0")"; cd ..; pwd)
thirdparty_path=${source_root}/src/third_party

#install boost
cd ${thirdparty_path}
boost_package_name=boost_1_68_0
boost_tarball=${boost_package_name}.tar.gz
boost_download_url=https://dl.bintray.com/boostorg/release/1.68.0/source/${boost_tarball}
wget ${boost_download_url}
tar -xvf ${boost_tarball}
cd ${boost_package_name}
./bootstrap.sh --prefix=${thirdparty_path}
./b2
./b2 install