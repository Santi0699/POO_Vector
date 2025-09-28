#include "calcularSist.h"
#include "vectorIncognitas.h" // para cargar A, b(labels), c
#include "SistEcuaciones.h"
#include <fstream>
#include <sstream>
#include <cstdint>
#include "complejos.h"

// ----------- resolver A·x = c -----------
bool solveABC(const MatrizComplejo &A, const VectorComplejo &c, VectorComplejo &sol)
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
        sol = Ainv * c;
        return true;
    }
    catch (...)
    {
        // no invertible
        return false;
    }
}

// ----------- guardar TXT -----------
bool saveSolutionToTxt(const std::string &path, const VectorIncognitas &labels, const VectorComplejo &sol)
{
    if (!(labels.size() == sol.vectorgetsize()))
    {
        return false;
    }

    std::ofstream fo(path, std::ios::trunc);
    if (!fo.good())
    {
        return false;
    }

    int i = 0;
    while (i < labels.size())
    {
        std::string name = labels.get(i);
        complejo z = sol.vectorget(i); // copia mutable para getters no const
        complejo zm = z;
        fo << name << "\t" << zm.GetReal() << " " << zm.GetImag() << "\n";
        ++i;
    }
    return fo.good();
}

// ----------- guardar BIN -----------
bool saveSolutionToBin(const std::string &path, const VectorIncognitas &labels, const VectorComplejo &sol)
{
    if (!(labels.size() == sol.vectorgetsize()))
    {
        return false;
    }

    std::ofstream fb(path, std::ios::binary | std::ios::trunc);
    if (!fb.good())
    {
        return false;
    }

    int32_t n = static_cast<int32_t>(labels.size());
    fb.write(reinterpret_cast<const char *>(&n), sizeof(n));

    int i = 0;
    bool ok = fb.good();
    while (i < n && ok)
    {
        std::string s = labels.get(i);
        int32_t len = static_cast<int32_t>(s.size());
        fb.write(reinterpret_cast<const char *>(&len), sizeof(len));
        if (fb.good())
        {
            if (len > 0)
            {
                fb.write(s.data(), len);
                if (!fb.good())
                {
                    ok = false;
                }
            }
        }
        else
        {
            ok = false;
        }

        complejo z = sol.vectorget(i);
        complejo zm = z;
        double re = zm.GetReal();
        double im = zm.GetImag();
        if (ok)
        {
            fb.write(reinterpret_cast<const char *>(&re), sizeof(re));
        }
        if (fb.good())
        {
            fb.write(reinterpret_cast<const char *>(&im), sizeof(im));
        }
        if (!fb.good())
        {
            ok = false;
        }

        ++i;
    }

    return ok && fb.good();
}

// ----------- pipeline TXT → resolver → TXT/BIN -----------
bool solveFromTxtAndSave(const std::string &inPathTxt, const std::string &outPathTxt, const std::string &outPathBin)
{
    MatrizComplejo A;
    VectorIncognitas labels;
    VectorComplejo c;

    bool okLoad = loadABCLabelsFromTxt(inPathTxt, A, labels, c);
    if (!okLoad)
    {
        return false;
    }

    VectorComplejo sol;
    bool okSolve = solveABC(A, c, sol);
    if (!okSolve)
    {
        return false;
    }

    bool okTxt = saveSolutionToTxt(outPathTxt, labels, sol);
    bool okBin = saveSolutionToBin(outPathBin, labels, sol);

    return okTxt && okBin;
}

// ----------- pipeline BIN → resolver → TXT/BIN -----------
bool solveFromBinAndSave(const std::string &inPathBin, const std::string &outPathTxt, const std::string &outPathBin)
{
    MatrizComplejo A;
    VectorIncognitas labels;
    VectorComplejo c;

    bool okLoad = loadABCLabelsFromBin(inPathBin, A, labels, c);
    if (!okLoad)
    {
        return false;
    }

    VectorComplejo sol;
    bool okSolve = solveABC(A, c, sol);
    if (!okSolve)
    {
        return false;
    }

    bool okTxt = saveSolutionToTxt(outPathTxt, labels, sol);
    bool okBin = saveSolutionToBin(outPathBin, labels, sol);

    return okTxt && okBin;
}
