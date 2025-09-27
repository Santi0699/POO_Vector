#include "MatrizComplejo.h"
#include "Matriz.h"
#include "Vector.h"
#include "VectorComplejo.h"

// ------- Util -------
static complejo toC(int v) { complejo z(static_cast<double>(v),0.0); return z; }

// ------- Regla de 3 -------
MatrizComplejo::MatrizComplejo(int r, int c) {
    rows = (r>0)?r:0; cols = (c>0)?c:0;
    int total = rows*cols;
    if (total>0) {
        data = new complejo[total];
        int i=0; while (i<total) { complejo z(0.0,0.0); data[i]=z; ++i; }
    } else { data=nullptr; }
}
MatrizComplejo::MatrizComplejo(const MatrizComplejo& o) {
    rows=o.rows; cols=o.cols; int total=rows*cols;
    if (total>0) { data=new complejo[total]; int i=0; while(i<total){ data[i]=o.data[i]; ++i; } }
    else { data=nullptr; }
}
MatrizComplejo& MatrizComplejo::operator=(const MatrizComplejo& o) {
    if (this!=&o) {
        int total=o.rows*o.cols; complejo* nd=nullptr;
        if (total>0) { nd=new complejo[total]; int i=0; while(i<total){ nd[i]=o.data[i]; ++i; } }
        delete[] data; data=nd; rows=o.rows; cols=o.cols;
    }
    return *this;
}
MatrizComplejo::~MatrizComplejo(){ delete[] data; data=nullptr; rows=0; cols=0; }

// ------- Acceso -------
complejo MatrizComplejo::get(int r, int c) const {
    if (r<0 || c<0 || r>=rows || c>=cols) { throw std::out_of_range("MatrizComplejo::get"); }
    return data[idx(r,c)];
}
void MatrizComplejo::set(int r,int c,const complejo& v){
    if (r<0 || c<0 || r>=rows || c>=cols) { throw std::out_of_range("MatrizComplejo::set"); }
    data[idx(r,c)] = v;
}

// ------- Minor / Determinante -------
MatrizComplejo MatrizComplejo::minorMatrix(int cutRow, int cutCol) const {
    if (rows!=cols || rows<=1) { return MatrizComplejo(); }
    MatrizComplejo m(rows-1, cols-1);
    int r=0; while (r<rows) {
        int c=0; while (c<cols) {
            if (!(r==cutRow || c==cutCol)) {
                int rr = r<cutRow ? r : r-1;
                int cc = c<cutCol ? c : c-1;
                m.set(rr, cc, get(r,c));
            }
            ++c;
        }
        ++r;
    }
    return m;
}
complejo MatrizComplejo::detRec() const {
    if (rows!=cols) { throw std::runtime_error("det: no cuadrada"); }
    if (rows==0) { complejo one(1.0,0.0); return one; }
    if (rows==1) { return data[0]; }
    if (rows==2) {
        complejo a11=get(0,0), a12=get(0,1), a21=get(1,0), a22=get(1,1);
        complejo t1=a11*a22; complejo t2=a12*a21; complejo res=t1-t2; return res;
    }
    complejo det(0.0,0.0);
    int j=0; while (j<cols) {
        MatrizComplejo m = minorMatrix(0,j);
        int s = ((0+j)%2==0)?+1:-1;
        complejo term = get(0,j) * m.detRec();
        if (s>0) { det = det + term; } else { det = det - term; }
        ++j;
    }
    return det;
}
complejo MatrizComplejo::determinant() const { return detRec(); }

// ------- Transpuesta / Adjunta / Inversa -------
MatrizComplejo MatrizComplejo::transpose() const {
    MatrizComplejo t(cols, rows);
    int r=0; while (r<rows) { int c=0; while (c<cols) { t.set(c,r,get(r,c)); ++c; } ++r; }
    return t;
}
MatrizComplejo MatrizComplejo::adjugate() const {
    if (rows!=cols) { throw std::runtime_error("adj: no cuadrada"); }
    MatrizComplejo cof(rows, cols);
    int r=0; while (r<rows) {
        int c=0; while (c<cols) {
            MatrizComplejo m=minorMatrix(r,c);
            int s=((r+c)%2==0)?+1:-1;
            complejo val = m.detRec();
            if (s<0) { complejo cero(0.0,0.0); val = cero - val; }
            cof.set(r,c,val); ++c;
        }
        ++r;
    }
    return cof.transpose();
}
MatrizComplejo MatrizComplejo::inverse() const {
    if (rows!=cols) { throw std::runtime_error("inv: no cuadrada"); }
    complejo det = determinant(); complejo cero(0.0,0.0);
    bool isZero = (det == cero);
    if (isZero) { throw std::runtime_error("inv: det=0"); }
    MatrizComplejo adj=adjugate(); MatrizComplejo inv(rows, cols);
    int r=0; while (r<rows) {
        int c=0; while (c<cols) { inv.set(r,c, adj.get(r,c) / det ); ++c; }
        ++r;
    }
    return inv;
}

// ------- Operadores MatrizComplejo ∘ MatrizComplejo -------
MatrizComplejo MatrizComplejo::operator+(const MatrizComplejo& b) const {
    if (!(rows==b.rows && cols==b.cols)) { throw std::runtime_error("sum cplx dims"); }
    MatrizComplejo c(rows, cols);
    int r=0; while (r<rows) { int cc=0; while (cc<cols) { c.set(r,cc, get(r,cc)+b.get(r,cc)); ++cc; } ++r; }
    return c;
}
MatrizComplejo MatrizComplejo::operator-(const MatrizComplejo& b) const {
    if (!(rows==b.rows && cols==b.cols)) { throw std::runtime_error("sub cplx dims"); }
    MatrizComplejo c(rows, cols);
    int r=0; while (r<rows) { int cc=0; while (cc<cols) { c.set(r,cc, get(r,cc)-b.get(r,cc)); ++cc; } ++r; }
    return c;
}
MatrizComplejo MatrizComplejo::operator*(const MatrizComplejo& b) const {
    if (cols!=b.rows) { throw std::runtime_error("mul cplx dims"); }
    MatrizComplejo c(rows, b.cols);
    int i=0; while (i<rows) {
        int j=0; while (j<b.cols) {
            complejo acc(0.0,0.0);
            int k=0; while (k<cols) { acc = acc + (get(i,k)*b.get(k,j)); ++k; }
            c.set(i,j,acc); ++j;
        } ++i;
    }
    return c;
}
MatrizComplejo MatrizComplejo::operator/(const MatrizComplejo& b) const {
    MatrizComplejo binv = b.inverse();
    return (*this) * binv;
}

// ------- Mixtas con MatrizInt (promoción a complejo) -------
MatrizComplejo operator+(const MatrizInt& a, const MatrizComplejo& b) {
    if (!(a.getRows()==b.getRows() && a.getCols()==b.getCols())) { throw std::runtime_error("sum mix dims"); }
    MatrizComplejo c(b.getRows(), b.getCols());
    int r=0; while (r<b.getRows()) { int cc=0; while (cc<b.getCols()) {
        c.set(r,cc, toC(a.get(r,cc)) + b.get(r,cc)); ++cc; } ++r; }
    return c;
}
MatrizComplejo operator+(const MatrizComplejo& a, const MatrizInt& b) { return b + a; }

MatrizComplejo operator-(const MatrizInt& a, const MatrizComplejo& b) {
    if (!(a.getRows()==b.getRows() && a.getCols()==b.getCols())) { throw std::runtime_error("sub mix dims"); }
    MatrizComplejo c(b.getRows(), b.getCols());
    int r=0; while (r<b.getRows()) { int cc=0; while (cc<b.getCols()) {
        c.set(r,cc, toC(a.get(r,cc)) - b.get(r,cc)); ++cc; } ++r; }
    return c;
}
MatrizComplejo operator-(const MatrizComplejo& a, const MatrizInt& b) {
    if (!(a.getRows()==b.getRows() && a.getCols()==b.getCols())) { throw std::runtime_error("sub mix dims"); }
    MatrizComplejo c(a.getRows(), a.getCols());
    int r=0; while (r<a.getRows()) { int cc=0; while (cc<a.getCols()) {
        c.set(r,cc, a.get(r,cc) - toC(b.get(r,cc))); ++cc; } ++r; }
    return c;
}

MatrizComplejo operator*(const MatrizInt& a, const MatrizComplejo& b) {
    if (a.getCols()!=b.getRows()) { throw std::runtime_error("mul mix dims"); }
    MatrizComplejo c(a.getRows(), b.getCols());
    int i=0; while (i<a.getRows()) {
        int j=0; while (j<b.getCols()) {
            complejo acc(0.0,0.0);
            int k=0; while (k<a.getCols()) { acc = acc + (toC(a.get(i,k)) * b.get(k,j)); ++k; }
            c.set(i,j,acc); ++j;
        } ++i;
    }
    return c;
}
MatrizComplejo operator*(const MatrizComplejo& a, const MatrizInt& b) {
    if (a.getCols()!=b.getRows()) { throw std::runtime_error("mul mix dims"); }
    MatrizComplejo c(a.getRows(), b.getCols());
    int i=0; while (i<a.getRows()) {
        int j=0; while (j<b.getCols()) {
            complejo acc(0.0,0.0);
            int k=0; while (k<a.getCols()) { acc = acc + (a.get(i,k) * toC(b.get(k,j))); ++k; }
            c.set(i,j,acc); ++j;
        } ++i;
    }
    return c;
}
MatrizComplejo operator/(const MatrizInt& a, const MatrizComplejo& b) {
    MatrizComplejo binv = b.inverse();
    MatrizComplejo ac(a.getRows(), a.getCols());
    int i=0; while (i<a.getRows()) { int j=0; while (j<a.getCols()) {
        ac.set(i,j, toC(a.get(i,j))); ++j; } ++i; }
    return ac * binv;
}
MatrizComplejo operator/(const MatrizComplejo& a, const MatrizInt& b) {
    MatrizInt binvI = b.inverse(); // puede lanzar si no es entera
    MatrizComplejo binv(binvI.getRows(), binvI.getCols());
    int i=0; while (i<binvI.getRows()) { int j=0; while (j<binvI.getCols()) {
        binv.set(i,j, toC(binvI.get(i,j))); ++j; } ++i; }
    return a * binv;
}

// ------- Vector ∘ MatrizComplejo -------
VectorComplejo operator*(const MatrizComplejo& A, const VectorComplejo& x) {
    if (A.getCols()!=x.vectorgetsize()) { throw std::runtime_error("A*x cplx dims"); }
    VectorComplejo y(A.getRows());
    int i=0; while (i<A.getRows()) {
        complejo acc(0.0,0.0);
        int k=0; while (k<A.getCols()) { acc = acc + (A.get(i,k) * x.vectorget(k)); ++k; }
        y.vectoraddback(acc); ++i;
    }
    return y;
}
VectorComplejo operator*(const VectorComplejo& xT, const MatrizComplejo& A) {
    if (xT.vectorgetsize() != A.getRows()) { throw std::runtime_error("x^T*A cplx dims"); }
    VectorComplejo y(A.getCols());

    int j = 0;
    while (j < A.getCols()) {
        complejo acc(0.0, 0.0);

        int k = 0;
        while (k < A.getRows()) {
            complejo lhs = xT.vectorget(k);   // copia mutable del const&
            complejo rhs = A.get(k, j);       // copia mutable del temporal
            complejo prod = lhs * rhs;
            complejo sum  = acc + prod;
            acc = sum;
            ++k;
        }

        y.vectoraddback(acc);
        ++j;
    }
    return y;
}
VectorComplejo operator/(const VectorComplejo& x, const MatrizComplejo& A) {
    MatrizComplejo Ainv = A.inverse();
    return x * Ainv;
}
VectorComplejo operator/(const MatrizComplejo& A, const VectorComplejo& x) {
    MatrizComplejo Ainv = A.inverse();
    return Ainv * x;
}

// Mixtas con vector int
VectorComplejo operator*(const MatrizComplejo& A, const vector& x) {
    if (A.getCols()!=x.vectorgetsize()) { throw std::runtime_error("A*x mix dims"); }
    VectorComplejo xc(A.getCols());
    int i0=0; while (i0<x.vectorgetsize()) { xc.vectoraddback( complejo(x.vectorget(i0),0.0) ); ++i0; }
    return A * xc;
}
VectorComplejo operator*(const vector& xT, const MatrizComplejo& A) {
    VectorComplejo xTc(xT.vectorgetsize());
    int i0=0; while (i0<xT.vectorgetsize()) { xTc.vectoraddback( complejo(xT.vectorget(i0),0.0) ); ++i0; }
    return xTc * A;
}

// Mixtas con MatrizInt y VectorComplejo
VectorComplejo operator*(const MatrizInt& A, const VectorComplejo& x) {
    MatrizComplejo Ac(A.getRows(), A.getCols());
    int i=0; while (i<A.getRows()) { int j=0; while (j<A.getCols()) { Ac.set(i,j, toC(A.get(i,j))); ++j; } ++i; }
    return Ac * x;
}
VectorComplejo operator*(const VectorComplejo& xT, const MatrizInt& A) {
    MatrizComplejo Ac(A.getRows(), A.getCols());
    int i=0; while (i<A.getRows()) { int j=0; while (j<A.getCols()) { Ac.set(i,j, toC(A.get(i,j))); ++j; } ++i; }
    return xT * Ac;
}

// ------- Stream -------
std::ostream& operator<<(std::ostream& os, const MatrizComplejo& m) {
    int r=0; while (r<m.getRows()) {
        int c=0; while (c<m.getCols()) { os << m.get(r,c); if (c+1<m.getCols()) { os << " "; } ++c; }
        os << "\n"; ++r;
    }
    return os;
}
