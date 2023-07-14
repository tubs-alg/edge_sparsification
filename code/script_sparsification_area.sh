#!/bin/bash

python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a complete
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a delaunayT
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a mwT
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a greedyT
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a wspd -t 2 -v 3
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a theta -k 6 -c 0
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a theta -k 6 -c 1
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a theta -k 6 -c 2
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a yao -k 6 -c 0
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a yao -k 6 -c 1
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a yao -k 6 -c 2
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a theta -k 12 -c 0
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a theta -k 12 -c 1
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a theta -k 12 -c 2
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a yao -k 12 -c 0
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a yao -k 12 -c 1
python3 run_sparsification.py -d out_instances/area/ -o out_sparsification/area/ -e cmake-build-release/edge_sparsification/edge_sparsification -a yao -k 12 -c 2
