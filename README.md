# Edge Sparsification for Geometric Tour Problems

*Sándor P. Fekete, Phillip Keldenich, Dominik Krupke, and Eike Niehs (TU Braunschweig, Algorithms Group, 2023)*

This repository contains the source code and the data for the (not yet published) paper "Edge Sparsification for Geometric Tour Problems" by Sándor P. Fekete, Phillip Keldenich, Dominik Krupke, and Eike Niehs (TU Braunschweig, Algorithms Group, 2023).

The data is stored in compressed pandas dataframes, individually for each problem.
Due to the size of the data (7 GB), we do not provide the concrete sparsified graphs or the computed solutions, but only the metadata.
The instances are provided in relatively simple json files with the x and y coordinates of the points.
You can find them compressed in the `./evaluation/00_instances.tar.gz` file.
The evaluations have been performed in documented jupyter notebooks, which are also provided in the `./evaluation` folder.
There is one jupyter notebook for each problem.

The code is written in C++ and requires CPLEX, Gurobi, and CGAL to be compiled as a whole.
We recommend to just copy the code snippets you need and adapt them to your own code.
This way you are not required to set up all dependencies, which can be quite cumbersome.
The code is barely documented, as its primary purpose was to create the data for the paper.
The evaluation of the data, on the other hand, is documented in the jupyter notebooks.