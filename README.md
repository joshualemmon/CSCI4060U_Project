# CSCI4060U_Project
Various parallel implementations of matrix multiplication.

Argument list for sequential version:  
  output M K N

Argument list for parallelized implementations:  
  output num_threads M K N
  
Output can be 0 or 1, hides or shows matrices.  
num_threads is the number of threads to be used. For simplicity, can't be larger than M.  
M is the number or rows for matrix A/C.  
K is the number of columns for A and rows for B.  
N is number of columns for B/C.  
