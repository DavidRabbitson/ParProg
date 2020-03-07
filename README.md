# Parallel Programming
This repo contains my homework programs for "Parallel Programming" seminars in MIPT.
### Programs
#### Obligatory programs
* MPI
  * Ranks of processes;
  * Work time of collective functions.
#### Bonus programs
* MPI
  * Realization of collective functions;
----
### Ranks of processes
The objective is to make _N_ processes print their ranks in respective order (from _0_ to _N_). There are two realizations.
##### "Train":
Each process prints its rank only after recieving message from previous process (process with rank _0_ invokes first);
##### "Guide":
_0_ process sends message to process _m_ (_m_ < _N_) that has to print its rank at this moment. After printing its rank, process _m_ sends message to _0_ process. When _0_ recieves message from _m_, _0_ sends message to _m + 1_ and waits for response.
