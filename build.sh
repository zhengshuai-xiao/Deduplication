#!/bin/bash

export MY_INSTALL_DIR=$HOME/.local
export PATH="$MY_INSTALL_DIR/bin:$PATH"
export LD_LIBRARY_PATH=$MY_INSTALL_DIR/lib:$LD_LIBRARY_PATH

mkdir -p cmake/build && cd cmake/build

cmake   -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release ../..

cmake --build . --config Release

#ctest -V