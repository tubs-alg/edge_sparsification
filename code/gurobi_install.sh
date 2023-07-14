!# /bin/bash

wget https://packages.gurobi.com/9.1/gurobi9.1.1_linux64.tar.gz
tar xvfz ~/gurobi9.1.1_linux64.tar.gz -C /opt/tmp/

cd /opt/tmp/gurobi911/linux64/src/build
make
cp libgurobi_c++.a ../../lib/

cd ~/ma-2020-eike-niehs/sparsification
cmake -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles"
cmake --build cmake-build-release --target all -- -j 3