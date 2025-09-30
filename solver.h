#ifndef SOLVER_H
#define SOLVER_H

#include "matrizComplejo.h"
#include "vectorComplejo.h"

// Resuelve A·x = c (por inversa). Devuelve false si A no es cuadrada / dims inválidas / no invertible.
bool solveABC(const MatrizComplejo &A, const VectorComplejo &c, VectorComplejo &x);

#endif // SOLVER_H
