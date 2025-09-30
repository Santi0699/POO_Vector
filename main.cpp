#include <iostream>
#include "complejos.h"
#include "Vector.h"
#include "VectorComplejo.h"
#include "matriz.h"
#include "matrizComplejo.h"
//#include "archivos.h"
#include "Saver.h"
#include "solver.h"
#include "Loader.h"
#include "FasorConv.h"
static void sep(const char *t) { std::cout << "\n==== " << t << " ====\n"; }

int main()
{
    MatrizComplejo A;
    VectorIncognitas b;
    VectorComplejo c, x;

    // Ejemplo: leer TXT en FASOR (bloques o pipes), resolver y guardar en FASOR
    if (loadFromTxtFasor("sist.txt", A, b, c))
    {
        if (solveABC(A, c, x))
        {
            saveSolutionTxtFasor("solucion.txt", b, x);
            saveSolutionBinFasor("solucion.bin", b, x);
        }
    }

    // Ejemplo: leer TXT en RECT (bloques o pipes), resolver y guardar en RECT
    if (loadFromTxtRect("sist.txt", A, b, c))
    {
        if (solveABC(A, c, x))
        {
            saveSolutionTxtRect("solucion1.txt", b, x);
            saveSolutionBinRect("solucion1.bin", b, x);
        }
    }

    // Ejemplo alternativo: leer BIN en RECT, resolver y guardar en RECT
    if (loadFromBinRect("sistema_rect.bin", A, b, c))
    {
        if (solveABC(A, c, x))
        {
            saveSolutionTxtRect("sol_rect.txt", b, x);
            saveSolutionBinRect("sol_rect.bin", b, x);
        }
    }
}

/* MAIN EJEMPLO 
#include <iostream>
#include <string>
#include "Loader.h"   // loadFrom..., load3...
#include "Solver.h"   // solveABC
#include "Saver.h"    // saveSolution...

static void printOk(const std::string& tag, bool ok){
    if(ok){ std::cout << "[OK] " << tag << "\n"; }
    else  { std::cout << "[FAIL] " << tag << "\n"; }
}

int main() {
    // ================== Caso 1: Un solo archivo (FASOR, TXT) ==================
    {
        MatrizComplejo A;
        VectorIncognitas b;
        VectorComplejo c, x;

        bool lk = loadFromTxtFasor("sistema_fasor.txt", A, b, c);
        printOk("loadFromTxtFasor", lk);

        bool sk = false;
        if(lk){
            sk = solveABC(A, c, x);
            printOk("solveABC (fasor/txt)", sk);
        }

        bool sx1=false, sx2=false;
        if(sk){
            sx1 = saveSolutionTxtFasor("sol_fasor.txt", b, x);
            sx2 = saveSolutionBinFasor("sol_fasor.bin", b, x);
            printOk("saveSolutionTxtFasor", sx1);
            printOk("saveSolutionBinFasor", sx2);
        }
    }

    // =========== Caso 2: Un solo archivo (RECTANGULAR, TXT) ReIm/ImRe =========
    {
        MatrizComplejo A;
        VectorIncognitas b;
        VectorComplejo c, x;

        // Cambiá PairOrder::ReIm por PairOrder::ImRe si tu TXT viene como (imag real)
        bool lk = loadFromTxtRect("sistema_rect.txt", A, b, c, PairOrder::ReIm);
        printOk("loadFromTxtRect (ReIm)", lk);

        bool sk=false;
        if(lk){
            sk = solveABC(A, c, x);
            printOk("solveABC (rect/txt)", sk);
        }

        bool sx1=false, sx2=false;
        if(sk){
            sx1 = saveSolutionTxtRect("sol_rect.txt", b, x);
            sx2 = saveSolutionBinRect("sol_rect.bin", b, x);
            printOk("saveSolutionTxtRect", sx1);
            printOk("saveSolutionBinRect", sx2);
        }
    }

    // ======= Caso 3: Tres archivos (A,b,c separados) – FASOR, TXT =============
    {
        MatrizComplejo A;
        VectorIncognitas b;
        VectorComplejo c, x;

        bool lk = load3FasorTxt("A_fasor.txt", "b.txt", "c_fasor.txt", A, b, c);
        printOk("load3FasorTxt", lk);

        bool sk=false;
        if(lk){
            sk = solveABC(A, c, x);
            printOk("solveABC (3-txt fasor)", sk);
        }

        bool sx1=false, sx2=false;
        if(sk){
            sx1 = saveSolutionTxtFasor("sol3_fasor.txt", b, x);
            sx2 = saveSolutionBinFasor("sol3_fasor.bin", b, x);
            printOk("saveSolutionTxtFasor (3)", sx1);
            printOk("saveSolutionBinFasor (3)", sx2);
        }
    }

    // ===== Caso 4: Tres archivos (A,b,c separados) – RECTANGULAR, BIN =========
    {
        MatrizComplejo A;
        VectorIncognitas b;
        VectorComplejo c, x;

        // Cambiá PairOrder::ReIm por PairOrder::ImRe si tus BIN tienen (imag, real)
        bool lk = load3RectBin("A_rect.bin", "b.bin", "c_rect.bin", A, b, c, PairOrder::ReIm);
        printOk("load3RectBin (ReIm)", lk);

        bool sk=false;
        if(lk){
            sk = solveABC(A, c, x);
            printOk("solveABC (3-bin rect)", sk);
        }

        bool sx1=false, sx2=false;
        if(sk){
            sx1 = saveSolutionTxtRect("sol3_rect.txt", b, x);
            sx2 = saveSolutionBinRect("sol3_rect.bin", b, x);
            printOk("saveSolutionTxtRect (3)", sx1);
            printOk("saveSolutionBinRect (3)", sx2);
        }
    }

    std::cout << "Listo.\n";
    return 0;
}

*/