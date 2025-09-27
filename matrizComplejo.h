#ifndef MATRIZ_COMPLEJO_H
#define MATRIZ_COMPLEJO_H

#include <iostream>
#include <stdexcept>
#include "complejos.h"

// Forward
class MatrizInt;
class vector;
class VectorComplejo;

class MatrizComplejo {
private:
    int rows;
    int cols;
    complejo* data;

    int  idx(int r, int c) const { return r * cols + c; }
    MatrizComplejo minorMatrix(int cutRow, int cutCol) const;
    complejo detRec() const;

public:
    // Regla de 3
    MatrizComplejo(int r = 0, int c = 0);
    MatrizComplejo(const MatrizComplejo& o);
    MatrizComplejo& operator=(const MatrizComplejo& o);
    ~MatrizComplejo();

    // Acceso
    int       getRows() const { return rows; }
    int       getCols() const { return cols; }
    complejo  get(int r, int c) const;
    void      set(int r, int c, const complejo& v);

    // Básicas
    MatrizComplejo transpose() const;
    MatrizComplejo adjugate() const;
    complejo       determinant() const;
    MatrizComplejo inverse() const;

    // Operadores MatrizComplejo ∘ MatrizComplejo
    MatrizComplejo operator+(const MatrizComplejo& b) const;
    MatrizComplejo operator-(const MatrizComplejo& b) const;
    MatrizComplejo operator*(const MatrizComplejo& b) const;
    MatrizComplejo operator/(const MatrizComplejo& b) const;

    // Mixtas con MatrizInt (promoción a complejo)
    friend MatrizComplejo operator+(const MatrizInt& a, const MatrizComplejo& b);
    friend MatrizComplejo operator+(const MatrizComplejo& a, const MatrizInt& b);
    friend MatrizComplejo operator-(const MatrizInt& a, const MatrizComplejo& b);
    friend MatrizComplejo operator-(const MatrizComplejo& a, const MatrizInt& b);
    friend MatrizComplejo operator*(const MatrizInt& a, const MatrizComplejo& b);
    friend MatrizComplejo operator*(const MatrizComplejo& a, const MatrizInt& b);
    friend MatrizComplejo operator/(const MatrizInt& a, const MatrizComplejo& b);
    friend MatrizComplejo operator/(const MatrizComplejo& a, const MatrizInt& b);

    // Vector ∘ MatrizComplejo
    friend VectorComplejo operator*(const MatrizComplejo& A, const VectorComplejo& x);
    friend VectorComplejo operator*(const VectorComplejo& xT, const MatrizComplejo& A);
    friend VectorComplejo operator/(const VectorComplejo& x, const MatrizComplejo& A);
    friend VectorComplejo operator/(const MatrizComplejo& A, const VectorComplejo& x);

    // Mixtas con vector int
    friend VectorComplejo operator*(const MatrizComplejo& A, const vector& x);
    friend VectorComplejo operator*(const vector& xT, const MatrizComplejo& A);

    // Mixtas con VectorComplejo y MatrizInt
    friend VectorComplejo operator*(const MatrizInt& A, const VectorComplejo& x);
    friend VectorComplejo operator*(const VectorComplejo& xT, const MatrizInt& A);

    friend std::ostream& operator<<(std::ostream& os, const MatrizComplejo& m);
};

#endif // MATRIZ_COMPLEJO_H
