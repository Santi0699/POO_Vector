#include "vector.h"
#include "vectorComplejo.h"
#include <stdexcept>

// ==========================
// Regla de 3
// ==========================
vector::vector(int cap)
{
    if (cap <= 0)
        cap = 1;
    a = new int[cap];
    size = 0;
    sizemax = cap;
}

vector::vector(const vector &other)
{
    sizemax = other.sizemax;
    size = other.size;
    a = new int[sizemax];
    for (int i = 0; i < size; ++i)
    {
        a[i] = other.a[i];
    }
}

vector &vector::operator=(const vector &other)
{
    if (this != &other)
    {
        int *nuevo = new int[other.sizemax];
        for (int i = 0; i < other.size; ++i)
        {
            nuevo[i] = other.a[i];
        }
        delete[] a;
        a = nuevo;
        size = other.size;
        sizemax = other.sizemax;
    }
    return *this;
}

vector::~vector()
{
    delete[] a;
    a = nullptr;
    size = 0;
    sizemax = 0;
}

// ==========================
// Acceso / set
// ==========================
int vector::vectorget(int index) const
{
    if (index < 0 || index >= size)
    {
        throw std::out_of_range("vectorget: indice fuera de rango");
    }
    return a[index];
}

void vector::vectorset(int index, int value)
{
    if (index < 0 || index >= size)
    {
        throw std::out_of_range("vectorset: indice fuera de rango");
    }
    a[index] = value;
}

// ==========================
// Inserciones
// ==========================
void vector::vectorinsert(int index, int value)
{
    if (index < 0 || index > size)
    {
        throw std::out_of_range("vectorinsert: indice invalido (0..size)");
    }
    if (size == sizemax)
    {
        vectorresize();
    }
    for (int i = size; i > index; --i)
    {
        a[i] = a[i - 1];
    }
    a[index] = value;
    ++size;
}

void vector::vectoraddback(int value)
{
    if (size == sizemax)
    {
        vectorresize();
    }
    a[size++] = value;
}

void vector::vectoraddfront(int value)
{
    if (size == sizemax)
    {
        vectorresize();
    }
    for (int i = size; i > 0; --i)
    {
        a[i] = a[i - 1];
    }
    a[0] = value;
    ++size;
}

// ==========================
// Borrado / búsqueda
// ==========================
void vector::vectorremove(int value)
{
    int pos = -1;
    int i = 0;
    bool found = false;
    while (i < size && !found)
    {
        if (a[i] == value)
        {
            pos = i;
            found = true;
        }
        else
        {
            ++i;
        }
    }
    if (found)
    {
        for (int j = pos; j < size - 1; ++j)
        {
            a[j] = a[j + 1];
        }
        --size;
    }
}

int vector::vectorfind(int index, int value) const
{
    if (index < 0 || index >= size)
    {
        return -1;
    }
    int res = -1;
    int i = index;
    bool found = false;
    while (i < size && !found)
    {
        if (a[i] == value)
        {
            res = i;
            found = true;
        }
        else
        {
            ++i;
        }
    }
    return res;
}

// ==========================
// Capacidad
// ==========================
int vector::vectorisfull() const { return size == sizemax; }

void vector::vectorresize()
{
    int newCap = sizemax > 0 ? sizemax * 2 : 1;
    int *b = new int[newCap];
    for (int i = 0; i < size; ++i)
    {
        b[i] = a[i];
    }
    delete[] a;
    a = b;
    sizemax = newCap;
}

// ==========================
// Tamaños
// ==========================
int vector::vectorgetsize() const { return size; }
int vector::vectorgetmaxsize() const { return sizemax; }

// ==========================
// Operadores con escalar int -> vector
// ==========================
vector vector::operator+(int s) const
{
    vector r(sizemax);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] + s;
    }
    return r;
}
vector vector::operator-(int s) const
{
    vector r(sizemax);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] - s;
    }
    return r;
}
vector vector::operator*(int s) const
{
    vector r(sizemax);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] * s;
    }
    return r;
}
vector vector::operator/(int s) const
{
    if (s == 0)
    {
        throw std::runtime_error("division por cero");
    }
    vector r(sizemax);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] / s;
    }
    return r;
}

vector operator-(int s, const vector &v)
{
    vector r(v.vectorgetmaxsize());
    int i = 0;
    while (i < v.vectorgetsize())
    {
        r.vectoraddback(s - v.vectorget(i));
        ++i;
    }
    return r;
}

// ==========================
// Operadores con complejo -> VectorComplejo
// ==========================
VectorComplejo vector::operator+(const complejo &c) const
{
    VectorComplejo r(sizemax);
    int i = 0;
    while (i < size)
    {
        complejo tmp(a[i], 0.0);
        r.vectoraddback(tmp + c);
        ++i;
    }
    return r;
}
VectorComplejo vector::operator-(const complejo &c) const
{
    VectorComplejo r(sizemax);
    int i = 0;
    while (i < size)
    {
        complejo tmp(a[i], 0.0);
        r.vectoraddback(tmp - c);
        ++i;
    }
    return r;
}
VectorComplejo vector::operator*(const complejo &c) const
{
    VectorComplejo r(sizemax);
    int i = 0;
    while (i < size)
    {
        complejo tmp(a[i], 0.0);
        r.vectoraddback(tmp * c);
        ++i;
    }
    return r;
}
VectorComplejo vector::operator/(const complejo &c) const
{
    VectorComplejo r(sizemax);
    int i = 0;
    while (i < size)
    {
        complejo tmp(a[i], 0.0);
        r.vectoraddback(tmp / c);
        ++i;
    }
    return r;
}

// no-miembro
// complejo + vector
VectorComplejo operator+(const complejo &c, const vector &v)
{
    VectorComplejo r(v.vectorgetmaxsize());
    int i = 0;
    while (i < v.vectorgetsize())
    {
        complejo lhs = c; // copia mutable
        complejo rhs(v.vectorget(i), 0.0);
        r.vectoraddback(lhs + rhs); // OK: lhs no es const
        ++i;
    }
    return r;
}

// complejo - vector
VectorComplejo operator-(const complejo &c, const vector &v)
{
    VectorComplejo r(v.vectorgetmaxsize());
    int i = 0;
    while (i < v.vectorgetsize())
    {
        complejo lhs = c; // copia mutable
        complejo rhs(v.vectorget(i), 0.0);
        r.vectoraddback(lhs - rhs); // evita const ∘ non-const
        ++i;
    }
    return r;
}

// complejo * vector
VectorComplejo operator*(const complejo &c, const vector &v)
{
    VectorComplejo r(v.vectorgetmaxsize());
    int i = 0;
    while (i < v.vectorgetsize())
    {
        complejo lhs = c; // copia mutable
        complejo rhs(v.vectorget(i), 0.0);
        r.vectoraddback(lhs * rhs);
        ++i;
    }
    return r;
}

// complejo / vector
VectorComplejo operator/(const complejo &c, const vector &v)
{
    VectorComplejo r(v.vectorgetmaxsize());
    int i = 0;
    while (i < v.vectorgetsize())
    {
        complejo lhs = c; // copia mutable
        complejo rhs(v.vectorget(i), 0.0);
        r.vectoraddback(lhs / rhs);
        ++i;
    }
    return r;
}

// ==========================
// Impresión
// ==========================
void vector::print(std::ostream &os) const
{
    os << "[";
    for (int i = 0; i < size; ++i)
    {
        if (i)
        {
            os << ", ";
        }
        os << a[i];
    }
    os << "]";
}
