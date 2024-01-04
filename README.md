# Edge Sparsification for Geometric Tour Problems

*Sándor P. Fekete, Phillip Keldenich, [Dominik Krupke](https://github.com/d-krupke), and Eike Niehs (TU Braunschweig, Algorithms Group, 2023)*

This repository contains the source code and the data for the (not yet published) paper "Edge Sparsification for Geometric Tour Problems" by Sándor P. Fekete, Phillip Keldenich, Dominik Krupke, and Eike Niehs (TU Braunschweig, Algorithms Group, 2023).

**Abstract:** *We study a variety of sparsification approaches for a spectrum of
geometric optimization problems related to tour problems, such as the Angular
TSP, the Minimum Perimeter Problem, and the Minimum/Maximum Area Polygon Problem.  To
this end, we conduct a thorough study that compares the solution quality and
runtime achieved by integer programming solvers on geometrically reduced edge
sets to the exact solution on the full edge set; considered
sparsification techniques include a variety of triangulations (Delaunay, Greedy,
Minimum Weight), Theta and Yao graphs, Well-Separated Pair Decomposition, and Onion graphs.*

*We demonstrate that edge sparsification often leads to significantly reduced runtimes.
For several of the considered problems, we can compute within a few seconds solutions that are
very close to being optimal for instances that could not be solved to provable optimality 
within an hour; for other problems, we encounter a significant loss in solution quality.
However, for almost all problems we considered,
we find good solutions much earlier in the search process than for the complete edge set;
thus, our methods can at least be used to provide initial bounds for the exact solution,
demonstrating their usefulness even if optimality cannot be established.*

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
