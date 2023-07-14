#!/bin/bash

cmake -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles"
cmake --build cmake-build-release --target all -- -j 3
