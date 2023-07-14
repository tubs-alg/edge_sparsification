#!/bin/bash

python3 run_sparsification.py -d out_instances/image_instances/brightness -o out_sparsification/image_instances/brightness -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a complete
python3 run_sparsification.py -d out_instances/image_instances/brightness -o out_sparsification/image_instances/brightness -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a delaunayT
python3 run_sparsification.py -d out_instances/image_instances/brightness -o out_sparsification/image_instances/brightness -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a mwT
python3 run_sparsification.py -d out_instances/image_instances/brightness -o out_sparsification/image_instances/brightness -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a greedyT

python3 run_sparsification.py -d out_instances/image_instances/edge -o out_sparsification/image_instances/edge -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a complete
python3 run_sparsification.py -d out_instances/image_instances/edge -o out_sparsification/image_instances/edge -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a delaunayT
python3 run_sparsification.py -d out_instances/image_instances/edge -o out_sparsification/image_instances/edge -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a mwT
python3 run_sparsification.py -d out_instances/image_instances/edge -o out_sparsification/image_instances/edge -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a greedyT
