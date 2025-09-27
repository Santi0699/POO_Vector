#include "Matriz.h"
#include "MatrizComplejo.h"
#include "Vector.h"
#include "VectorComplejo.h"
#include "complejos.h"

// -------- Regla de 3 --------
MatrizInt::MatrizInt(int r, int c) {
    rows = (r > 0) ? r : 0;
    cols = (c > 0) ? c : 0;
    int total = rows * cols;
    if (total > 0) {
        data = new int[total];
        int i = 0;
        while (i < total) { data[i] = 0; ++i; }
    } else {
        data = nullptr;
    }
}
MatrizInt::MatrizInt(const MatrizInt& o) {
    rows = o.rows; cols = o.cols;
    int total = rows * cols;
    if (total > 0) {
        data = new int[total];
        int i = 0;
        while (i < total) { data[i] = o.data[i]; ++i; }
    } else {
        data = nullptr;
    }
}
MatrizInt& MatrizInt::operator=(const MatrizInt& o) {
    if (this != &o) {
        int total = o.rows * o.cols;
        int* nd = nullptr;
        if (total > 0) {
            nd = new int[total];
            int i = 0;
            while (i < total) { nd[i] = o.data[i]; ++i; }
        }
        delete[] data;
        data = nd; rows = o.rows; cols = o.cols;
    }
    return *this;
}
MatrizInt::~MatrizInt() {
    delete[] data; data = nullptr; rows = 0; cols = 0;
}

// -------- Acceso --------
int MatrizInt::get(int r, int c) const {
    if (r < 0 || c < 0 || r >= rows || c >= cols) { throw std::out_of_range("MatrizInt::get"); }
    return data[idx(r, c)];
}
void MatrizInt::set(int r, int c, int v) {
    if (r < 0 || c < 0 || r >= rows || c >= cols) { throw std::out_of_range("MatrizInt::set"); }
    data[idx(r, c)] = v;
}

// -------- Minor / Determinante --------
MatrizInt MatrizInt::minorMatrix(int cutRow, int cutCol) const {
    if (rows != cols || rows <= 1) { return MatrizInt(); }
    MatrizInt m(rows - 1, cols - 1);
    int r = 0;
    while (r < rows) {
        int c = 0;
        while (c < cols) {
            if (!(r == cutRow || c == cutCol)) {
                int rr = r < cutRow ? r : r - 1;
                int cc = c < cutCol ? c : c - 1;
                m.set(rr, cc, get(r, c));
            }
            ++c;
        }
        ++r;
    }
    return m;
}
long long MatrizInt::detRec() const {
    if (rows != cols) { throw std::runtime_error("determinant: no cuadrada"); }
    if (rows == 0) { return 1; }
    if (rows == 1) { return data[0]; }
    if (rows == 2) {
        long long a11 = get(0,0), a12 = get(0,1), a21 = get(1,0), a22 = get(1,1);
        return a11 * a22 - a12 * a21;
    }
    long long det = 0;
    int j = 0;
    while (j < cols) {
        MatrizInt m = minorMatrix(0, j);
        long long s = ((0 + j) % 2 == 0) ? +1 : -1;
        det += s * static_cast<long long>(get(0, j)) * m.detRec();
        ++j;
    }
    return det;
}
long long MatrizInt::determinant() const { return detRec(); }

// -------- Transpuesta / Adjunta / Inversa --------
MatrizInt MatrizInt::transpose() const {
    MatrizInt t(cols, rows);
    int r = 0;
    while (r < rows) {
        int c = 0;
        while (c < cols) { t.set(c, r, get(r, c)); ++c; }
        ++r;
    }
    return t;
}
MatrizInt MatrizInt::adjugate() const {
    if (rows != cols) { throw std::runtime_error("adjugate: no cuadrada"); }
    MatrizInt cof(rows, cols);
    int r = 0;
    while (r < rows) {
        int c = 0;
        while (c < cols) {
            MatrizInt m = minorMatrix(r, c);
            long long s = ((r + c) % 2 == 0) ? +1 : -1;
            long long val = s * m.detRec();
            cof.set(r, c, static_cast<int>(val));
            ++c;
        }
        ++r;
    }
    return cof.transpose();
}
MatrizInt MatrizInt::inverse() const {
    if (rows != cols) { throw std::runtime_error("inverse: no cuadrada"); }
    long long det = determinant();
    if (det == 0) { throw std::runtime_error("inverse: det=0"); }
    MatrizInt adj = adjugate();
    MatrizInt inv(rows, cols);
    int r = 0;
    while (r < rows) {
        int c = 0;
        while (c < cols) {
            int num = adj.get(r, c);
            if (num % det == 0) {
                inv.set(r, c, static_cast<int>(num / det));
            } else {
                throw std::runtime_error("inverse: no entera");
            }
            ++c;
        }
        ++r;
    }
    return inv;
}

// -------- Operadores MatrizInt ∘ MatrizInt --------
MatrizInt MatrizInt::operator+(const MatrizInt& b) const {
    if (!(rows==b.rows && cols==b.cols)) { throw std::runtime_error("sum: dims"); }
    MatrizInt c(rows, cols);
    int r=0; while (r<rows) { int cc=0; while (cc<cols) { c.set(r,cc, get(r,cc)+b.get(r,cc)); ++cc; } ++r; }
    return c;
}
MatrizInt MatrizInt::operator-(const MatrizInt& b) const {
    if (!(rows==b.rows && cols==b.cols)) { throw std::runtime_error("sub: dims"); }
    MatrizInt c(rows, cols);
    int r=0; while (r<rows) { int cc=0; while (cc<cols) { c.set(r,cc, get(r,cc)-b.get(r,cc)); ++cc; } ++r; }
    return c;
}
MatrizInt MatrizInt::operator*(const MatrizInt& b) const {
    if (cols != b.rows) { throw std::runtime_error("mul: dims"); }
    MatrizInt c(rows, b.cols);
    int i=0;
    while (i<rows) {
        int j=0;
        while (j<b.cols) {
            long long acc=0; int k=0;
            while (k<cols) { acc += static_cast<long long>(get(i,k))*b.get(k,j); ++k; }
            c.set(i,j, static_cast<int>(acc));
            ++j;
        }
        ++i;
    }
    return c;
}
MatrizInt MatrizInt::operator/(const MatrizInt& b) const {
    MatrizInt binv = b.inverse();
    return (*this) * binv;
}

// -------- Vector ∘ MatrizInt --------
vector operator*(const MatrizInt& A, const vector& x) {
    extern int /* dummy to force declaration after include */ _unused_vector_symbol;
    (void)_unused_vector_symbol;
    if (A.getCols() != x.vectorgetsize()) { throw std::runtime_error("A*x dims"); }
    vector y(A.getRows());
    int i=0; while (i<A.getRows()) {
        long long acc=0; int k=0;
        while (k<A.getCols()) { acc += static_cast<long long>(A.get(i,k))*x.vectorget(k); ++k; }
        y.vectoraddback(static_cast<int>(acc)); ++i;
    }
    return y;
}
vector operator*(const vector& xT, const MatrizInt& A) {
    if (xT.vectorgetsize() != A.getRows()) { throw std::runtime_error("x^T*A dims"); }
    vector y(A.getCols());
    int j=0; while (j<A.getCols()) {
        long long acc=0; int k=0;
        while (k<A.getRows()) { acc += static_cast<long long>(xT.vectorget(k))*A.get(k,j); ++k; }
        y.vectoraddback(static_cast<int>(acc)); ++j;
    }
    return y;
}
vector operator/(const vector& x, const MatrizInt& A) {
    MatrizInt Ainv = A.inverse();
    return x * Ainv;
}
vector operator/(const MatrizInt& A, const vector& x) {
    MatrizInt Ainv = A.inverse();
    return Ainv * x;
}

// -------- Stream --------
std::ostream& operator<<(std::ostream& os, const MatrizInt& m) {
    int r=0; while (r<m.getRows()) {
        int c=0; while (c<m.getCols()) {
            os << m.get(r,c); if (c+1<m.getCols()) { os << " "; } ++c;
        }
        os << "\n"; ++r;
    }
    return os;
}

// Hack para forzar el linker a incluir Vector.h (sólo declaración)
int _unused_vector_symbol = 0;
