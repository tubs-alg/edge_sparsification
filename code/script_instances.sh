#!/bin/bash

./cmake-build-release/instance_generator/instance_random -c 10 --distribution normal --json -o out_instances/normal/ --batch 155 600 5
./cmake-build-release/instance_generator/instance_random -c 10 --distribution uniform_int --json -o out_instances/uniform_int/ --batch 155 600 5
./cmake-build-release/instance_generator/instance_random -c 10 --distribution uniform_real --json -o out_instances/uniform_real/ --batch 155 600 5
