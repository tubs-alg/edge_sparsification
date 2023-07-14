#!/bin/bash

python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a complete
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a delaunayT
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a mwT
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a greedyT
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a greedy -t 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 1 -k 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 1 -k 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 1 -k 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 2 -k 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 2 -k 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 2 -k 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 3
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a theta -k 6 -c 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a theta -k 6 -c 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a theta -k 6 -c 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a yao -k 6 -c 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a yao -k 6 -c 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a yao -k 6 -c 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 12 -c 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 12 -c 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 12 -c 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 12 -c 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 12 -c 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 12 -c 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 18 -c 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 18 -c 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 18 -c 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 18 -c 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 18 -c 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 18 -c 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 24 -c 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 24 -c 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 24 -c 2
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 24 -c 0
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 24 -c 1
python3 run_sparsification.py -d out_instances/normal/ -o out_sparsification/normal/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 24 -c 2

python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a complete
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a delaunayT
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a mwT
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a greedyT
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a greedy -t 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 1 -k 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 1 -k 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 1 -k 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 2 -k 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 2 -k 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 2 -k 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 3
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a theta -k 6 -c 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a theta -k 6 -c 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a theta -k 6 -c 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a yao -k 6 -c 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a yao -k 6 -c 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a yao -k 6 -c 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 12 -c 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 12 -c 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 12 -c 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 12 -c 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 12 -c 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 12 -c 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 18 -c 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 18 -c 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 18 -c 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 18 -c 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 18 -c 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 18 -c 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 24 -c 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 24 -c 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 24 -c 2
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 24 -c 0
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 24 -c 1
python3 run_sparsification.py -d out_instances/uniform_int/ -o out_sparsification/uniform_int/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 24 -c 2

python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a complete
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a delaunayT
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a mwT
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 600 -a greedyT
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a greedy -t 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 1 -k 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 1 -k 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 1 -k 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 2 -k 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 2 -k 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a onion -v 2 -k 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 4 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 3 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 2 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 0 --sep 1 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 2 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 3 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 4 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 5 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 6 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 7 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a wspd -t 8 -v 3
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a theta -k 6 -c 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a theta -k 6 -c 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a theta -k 6 -c 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a yao -k 6 -c 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a yao -k 6 -c 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 150 -a yao -k 6 -c 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 12 -c 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 12 -c 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 12 -c 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 12 -c 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 12 -c 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 12 -c 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 18 -c 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 18 -c 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 18 -c 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 18 -c 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 18 -c 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 18 -c 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 24 -c 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 24 -c 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a theta -k 24 -c 2
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 24 -c 0
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 24 -c 1
python3 run_sparsification.py -d out_instances/uniform_real/ -o out_sparsification/uniform_real/ -e cmake-build-release/edge_sparsification/edge_sparsification --sizes 1 50 -a yao -k 24 -c 2

