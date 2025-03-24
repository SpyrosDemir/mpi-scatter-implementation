# mpi-scatter-implementation

**IMPORTANT: THE PROGRAM ONLY WORKS IF N IS DIVISIBLE BY P**

This MPI program processes a matrix in parallel. Processor 0 gets user input, broadcasts size, and scatters data (MPI_Scatter). It checks strict diagonal dominance, finds max absolute diagonal value, computes matrix B, finds min in B, and terminates. 


**Program Execution:**
Initialization:
Global variables and matrices are allocated.
MPI_Init() starts the MPI environment.

**Matrix Input & Distribution:**
Processor 0 requests matrix size and elements from the user, ensuring valid input.
MPI_Bcast() broadcasts the matrix size to all processors.
Each processor determines its assigned rows, and MPI_Scatter() distributes matrix segments.

**Strictly Diagonally Dominant Check:**
Each processor checks if its portion of the matrix meets strict diagonal dominance.
If a violation occurs, local_flag is set to 1, and the check stops.
MPI_Reduce() aggregates local_flag values, terminating the program if any flag is 1.

**Finding Maximum Absolute Diagonal Element:**
Each processor finds its local max absolute diagonal element.
MPI_Reduce() determines the global max, which is displayed and broadcasted.

**Matrix B Calculation & Gathering:**
A new matrix B is computed using a given formula.
MPI_Gather() assembles B in processor 0, which prints the final matrix.

**Finding Minimum Element & Coordinates in B:**
Each processor finds its local min in B.
Using MPI_MINLOC(), processor 0 receives the minimum value and its coordinates and prints them.

**Termination:**
The program ends with MPI_Finalize().
