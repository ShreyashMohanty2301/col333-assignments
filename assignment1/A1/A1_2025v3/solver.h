#ifndef SOLVER_H
#define SOLVER_H

#include "structures.h"
#include <chrono>
using ll = long long;
/**
 * @brief The main function to implement your search/optimization algorithm.
 * * @param problem A const reference to the parsed problem data.
 * @return A Solution object containing the plan for all helicopters.
 */
Solution solve(const ProblemData& problem);
double h_val(Solution &sol,const ProblemData &prob);
#endif // SOLVER_H