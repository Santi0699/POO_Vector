#include <iostream>
#include "complejos.h"
#include "Vector.h"
#include "VectorComplejo.h"
#include "matriz.h"
#include "matrizComplejo.h"
#include "archivos.h"
#include "SistEcuaciones.h"
#include "calcularSist.h"

static void sep(const char* t) { std::cout << "\n==== " << t << " ====\n"; }
/*
int main() 
{
    
    // ===============================
    // 1. Vector<int>
    // ===============================
    vector vi(5);
    vi.vectoraddback(1);
    vi.vectoraddback(2);
    vi.vectoraddback(3);

    std::cout << "vi = " << vi << "\n";
    std::cout << "vi + 5 = " << (vi + 5) << "\n";
    std::cout << "5 - vi = " << (5 - vi) << "\n";
    std::cout << "vi * 3 = " << (vi * 3) << "\n";
    std::cout << "vi / 2 = " << (vi / 2) << "\n";

    // ===============================
    // 2. Vector<int> con complejo
    // ===============================
    complejo c1(2, 3);    // 2 + 3i
    complejo c2(1, -1);   // 1 - i

    VectorComplejo vc1 = vi + c1;
    std::cout << "vi + c1 = " << vc1 << "\n";

    VectorComplejo vc2 = c2 - vi;
    std::cout << "c2 - vi = " << vc2 << "\n";

    VectorComplejo vc3 = vi * c2;
    std::cout << "vi * c2 = " << vc3 << "\n";

    VectorComplejo vc4 = c1 / vi;
    std::cout << "c1 / vi = " << vc4 << "\n";

    // ===============================
    // 3. VectorComplejo con int
    // ===============================
    VectorComplejo vcomp(5);
    vcomp.vectoraddback(complejo(1, 1));
    vcomp.vectoraddback(complejo(2, -1));
    vcomp.vectoraddback(complejo(-3, 0.5));

    std::cout << "vcomp = " << vcomp << "\n";
    std::cout << "vcomp + 2 = " << (vcomp + 2) << "\n";
    std::cout << "2 - vcomp = " << (2 - vcomp) << "\n";
    std::cout << "vcomp * 3 = " << (vcomp * 3) << "\n";
    std::cout << "vcomp / 2 = " << (vcomp / 2) << "\n";

    // ===============================
    // 4. VectorComplejo con complejo
    // ===============================
    std::cout << "vcomp + c1 = " << (vcomp + c1) << "\n";
    std::cout << "c2 - vcomp = " << (c2 - vcomp) << "\n";
    std::cout << "vcomp * c2 = " << (vcomp * c2) << "\n";
    std::cout << "c1 / vcomp = " << (c1 / vcomp) << "\n";

    // ===============================
    // 5. Buscar y borrar en vector<int>
    // ===============================
    std::cout << "\nPrueba de búsqueda y borrado en vi:\n";
    int pos = vi.vectorfind(0, 2);
    std::cout << "Encontrado 2 en pos = " << pos << "\n";
    vi.vectorremove(2);
    std::cout << "vi tras remove(2) = " << vi << "\n";

    // ===============================
    // 6. Buscar y borrar en VectorComplejo
    // ===============================
    std::cout << "\nPrueba de búsqueda y borrado en vcomp:\n";
    int pos2 = vcomp.vectorfind(0, complejo(1, 1));
    std::cout << "Encontrado (1,1) en pos = " << pos2 << "\n";
    vcomp.vectorremove(complejo(1, 1));
    std::cout << "vcomp tras remove((1,1)) = " << vcomp << "\n";

    try {
        // --------------------------
        // 1) Matrices INT
        // --------------------------
        sep("MatrizInt: basicos + - * /, transpuesta/adj/det/inv");
        // A y B con det = 1 (unimodulares) -> inversa entera
        MatrizInt A(2,2); A.set(0,0,1); A.set(0,1,1); A.set(1,0,0); A.set(1,1,1);
        MatrizInt B(2,2); B.set(0,0,1); B.set(0,1,0); B.set(1,0,1); B.set(1,1,1);

        std::cout << "A:\n" << A;
        std::cout << "B:\n" << B;
        std::cout << "A+B:\n" << (A + B);
        std::cout << "A-B:\n" << (A - B);
        std::cout << "A*B:\n" << (A * B);
        std::cout << "det(A) = " << A.determinant() << "\n";
        std::cout << "adj(A):\n" << A.adjugate();
        std::cout << "A^T:\n" << A.transpose();
        std::cout << "A^{-1}:\n" << A.inverse();
        std::cout << "A/B (A * B^{-1}):\n" << (A / B);

        // --------------------------
        // 2) Matrices COMPLEJO
        // --------------------------
        sep("MatrizComplejo: basicos + - * /, transpuesta/adj/det/inv");
        MatrizComplejo C(2,2);
        C.set(0,0, complejo(1, 1)); C.set(0,1, complejo(2, 0));
        C.set(1,0, complejo(0, 1)); C.set(1,1, complejo(1,-1));
        MatrizComplejo D(2,2);
        D.set(0,0, complejo(1, 0)); D.set(0,1, complejo(0, 1));
        D.set(1,0, complejo(1, 0)); D.set(1,1, complejo(1, 1));

        std::cout << "C:\n" << C;
        std::cout << "D:\n" << D;
        std::cout << "C+D:\n" << (C + D);
        std::cout << "C-D:\n" << (C - D);
        std::cout << "C*D:\n" << (C * D);
        std::cout << "det(C) = " << C.determinant() << "\n";
        std::cout << "adj(C):\n" << C.adjugate();
        std::cout << "C^T:\n" << C.transpose();
        std::cout << "C^{-1}:\n" << C.inverse();
        std::cout << "C/D (C * D^{-1}):\n" << (C / D);

        // --------------------------
        // 3) MIXTAS MatrizInt ∘ MatrizComplejo
        // --------------------------
        sep("Mixtas MatrizInt ∘ MatrizComplejo: + - * /");
        std::cout << "A + C:\n" << (A + C);
        std::cout << "C + A:\n" << (C + A);
        std::cout << "A - C:\n" << (A - C);
        std::cout << "C - A:\n" << (C - A);
        std::cout << "A * C:\n" << (A * C);
        std::cout << "C * A:\n" << (C * A);
        std::cout << "A / C:\n" << (A / C); // A * C^{-1}
        std::cout << "C / A:\n" << (C / A); // C * A^{-1} (A^{-1} entera)

        // --------------------------
        // 4) Vector INT con MatrizInt
        // --------------------------
        sep("Vector<int> ∘ MatrizInt: * y /");
        vector xv; xv.vectoraddback(3); xv.vectoraddback(1); // tamaño 2
        std::cout << "x = " << xv << "\n";
        std::cout << "A * x = " << (A * xv) << "\n";
        std::cout << "x^T * A = " << (xv * A) << "\n";
        std::cout << "x / A  = " << (xv / A) << "   (x * A^{-1})\n";
        std::cout << "A / x  = " << (A / xv) << "   (A^{-1} * x)\n";

        // --------------------------
        // 5) Vector COMPLEJO con MatrizComplejo
        // --------------------------
        sep("VectorComplejo ∘ MatrizComplejo: * y /");
        VectorComplejo xc; xc.vectoraddback(complejo(1,0)); xc.vectoraddback(complejo(0,1)); // tamaño 2
        std::cout << "xc = " << xc << "\n";
        std::cout << "C * xc = " << (C * xc) << "\n";
        std::cout << "xc^T * C = " << (xc * C) << "\n";
        std::cout << "xc / C  = " << (xc / C) << "   (xc * C^{-1})\n";
        std::cout << "C / xc  = " << (C / xc) << "   (C^{-1} * xc)\n";

        // --------------------------
        // 6) Mixtas con vectores: (A_cplx ∘ vector_int) y (A_int ∘ VectorComplejo)
        // --------------------------
        sep("Mixtas con vectores");
        std::cout << "C * x(int) = " << (C * xv) << "\n";
        std::cout << "x^T(int) * C = " << (xv * C) << "\n";

        std::cout << "A(int) * xc = " << (A * xc) << "\n";
        std::cout << "xc^T * A(int) = " << (xc * A) << "\n";

        // Todo OK
        sep("OK");
    } catch (const std::exception& e) {
        std::cout << "\n[EXCEPCION] " << e.what() << "\n";
    }

VectorComplejo vec;
    bool ok=loadComplejosFromTxt("complejos.txt",vec);
    vec.print(std::cout);
    saveComplejosToTxt("copia.txt",vec);
    return 0;
}
*/


int main() {
    // 1) Resolver directamente desde TXT y guardar resultados
    bool ok1 = solveFromTxtAndSave("sist.txt",
                                   "solucion.txt",
                                   "solucion.bin");
    std::cout << "TXT→solve→out: " << (ok1 ? "OK" : "ERROR") << "\n";


    // 3) O hacer el flujo manual si querés revisar:
    MatrizComplejo A; VectorIncognitas bLabels; VectorComplejo c, sol;
    if (loadABCLabelsFromTxt("sistema_labels.txt", A, bLabels, c)) {
        if (solveABC(A, c, sol)) {
            saveSolutionToTxt("sol_manual.txt", bLabels, sol);
            saveSolutionToBin("sol_manual.bin", bLabels, sol);
        }
    }
    return 0;
}