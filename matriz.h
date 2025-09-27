#ifndef MATRIZ_H
#define MATRIZ_H

#include <iostream>
#include <stdexcept>

// Forward declarations para evitar ciclos
class MatrizComplejo;
class vector;
class VectorComplejo;
class complejo;

class MatrizInt {
private:
    int rows;
    int cols;
    int* data;

    int  idx(int r, int c) const { return r * cols + c; }
    MatrizInt minorMatrix(int cutRow, int cutCol) const;
    long long detRec() const;

public:
    // Regla de 3
    MatrizInt(int r = 0, int c = 0);
    MatrizInt(const MatrizInt& o);
    MatrizInt& operator=(const MatrizInt& o);
    ~MatrizInt();

    // Acceso
    int  getRows() const { return rows; }
    int  getCols() const { return cols; }
    int  get(int r, int c) const;
    void set(int r, int c, int v);

    // Básicas
    MatrizInt transpose() const;
    MatrizInt adjugate() const;
    long long determinant() const;
    MatrizInt inverse() const; // inversa ENTERA (throw si no existe)

    // Operadores MatrizInt ∘ MatrizInt
    MatrizInt operator+(const MatrizInt& b) const;
    MatrizInt operator-(const MatrizInt& b) const;
    MatrizInt operator*(const MatrizInt& b) const;
    MatrizInt operator/(const MatrizInt& b) const; // A * inv(B)

    // ---------- Operadores mixtos con MatrizComplejo (definidos en MatrizComplejo.cpp)
    friend MatrizComplejo operator+(const MatrizInt& a, const MatrizComplejo& b);
    friend MatrizComplejo operator+(const MatrizComplejo& a, const MatrizInt& b);
    friend MatrizComplejo operator-(const MatrizInt& a, const MatrizComplejo& b);
    friend MatrizComplejo operator-(const MatrizComplejo& a, const MatrizInt& b);
    friend MatrizComplejo operator*(const MatrizInt& a, const MatrizComplejo& b);
    friend MatrizComplejo operator*(const MatrizComplejo& a, const MatrizInt& b);
    friend MatrizComplejo operator/(const MatrizInt& a, const MatrizComplejo& b);
    friend MatrizComplejo operator/(const MatrizComplejo& a, const MatrizInt& b);

    // ---------- Vector ∘ MatrizInt (definidos en MatrizInt.cpp)
    friend vector         operator*(const MatrizInt& A, const vector& x);  // A*x
    friend vector         operator*(const vector& xT, const MatrizInt& A); // x^T*A
    friend vector         operator/(const vector& x, const MatrizInt& A);  // x*inv(A)
    friend vector         operator/(const MatrizInt& A, const vector& x);  // inv(A)*x

    // ---------- Mixtas con complejos (definidos en MatrizComplejo.cpp)
    friend VectorComplejo operator*(const MatrizComplejo& A, const vector& x);
    friend VectorComplejo operator*(const vector& xT, const MatrizComplejo& A);
    friend VectorComplejo operator*(const MatrizInt& A, const VectorComplejo& x);
    friend VectorComplejo operator*(const VectorComplejo& xT, const MatrizInt& A);

    friend std::ostream& operator<<(std::ostream& os, const MatrizInt& m);
};

#endif // MATRIZ_INT_H
