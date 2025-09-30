#include "Solver.h"

bool solveABC(const MatrizComplejo &A, const VectorComplejo &c, VectorComplejo &x)
{
    if (!(A.getRows() == A.getCols()))
    {
        return false;
    }
    if (!(A.getRows() == c.vectorgetsize()))
    {
        return false;
    }
    try
    {
        MatrizComplejo Ainv = A.inverse();
        x = Ainv * c;
        return true;
    }
    catch (...)
    {
        return false;
    }
}
