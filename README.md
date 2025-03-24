# mpi-scatter-implementation
This MPI program processes a matrix in parallel. Processor 0 gets user input, broadcasts size, and scatters data (MPI_Scatter). It checks strict diagonal dominance, finds max absolute diagonal value, computes matrix B, finds min in B, and terminates. 
