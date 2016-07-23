#! /bin/sh

set -e

cd ..
   mulle-bootstrap build  -c Debug -k "$@"

cd build
   cmake -DCMAKE_OSX_SYSROOT=macosx -DCMAKE_INSTALL_PREFIX="`pwd`/.." -DCMAKE_BUILD_TYPE=Debug ..
   make install

