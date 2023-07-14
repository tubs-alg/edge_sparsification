#!/bin/bash

python3 run_sparsification.py -d out_instances/TSPLIB_out/ -o out_sparsification/TSPLIB/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 250 -a complete
python3 run_sparsification.py -d out_instances/TSPLIB_out/ -o out_sparsification/TSPLIB/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 250 -a delaunayT
python3 run_sparsification.py -d out_instances/TSPLIB_out/ -o out_sparsification/TSPLIB/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 250 -a mwT
python3 run_sparsification.py -d out_instances/TSPLIB_out/ -o out_sparsification/TSPLIB/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 250 -a greedyT
