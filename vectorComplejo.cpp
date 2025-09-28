#include "VectorComplejo.h"
#include <stdexcept>

// ==========================
// Regla de 3
// ==========================
VectorComplejo::VectorComplejo(int cap)
{
    if (cap <= 0)
        cap = 1;
    a = new complejo[cap];
    size = 0;
    capacity = cap;
}
VectorComplejo::VectorComplejo(const VectorComplejo &o)
{
    capacity = o.capacity;
    size = o.size;
    a = new complejo[capacity];
    for (int i = 0; i < size; ++i)
    {
        a[i] = o.a[i];
    }
}
VectorComplejo &VectorComplejo::operator=(const VectorComplejo &o)
{
    if (this != &o)
    {
        complejo *b = new complejo[o.capacity];
        for (int i = 0; i < o.size; ++i)
        {
            b[i] = o.a[i];
        }
        delete[] a;
        a = b;
        size = o.size;
        capacity = o.capacity;
    }
    return *this;
}
VectorComplejo::~VectorComplejo()
{
    delete[] a;
    a = nullptr;
    size = 0;
    capacity = 0;
}

// ==========================
// Acceso / set
// ==========================
const complejo &VectorComplejo::vectorget(int index) const
{
    if (index < 0 || index >= size)
    {
        throw std::out_of_range("vectorget: indice fuera de rango");
    }
    return a[index];
}
void VectorComplejo::vectorset(int index, const complejo &value)
{
    if (index < 0 || index >= size)
    {
        throw std::out_of_range("vectorset: indice fuera de rango");
    }
    a[index] = value;
}

// ==========================
// Inserción / agregado
// ==========================
void VectorComplejo::vectorinsert(int index, const complejo &value)
{
    if (index < 0 || index > size)
    {
        throw std::out_of_range("vectorinsert: indice invalido (0..size)");
    }
    if (size == capacity)
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
void VectorComplejo::vectoraddback(const complejo &value)
{
    if (size == capacity)
    {
        vectorresize();
    }
    a[size++] = value;
}
void VectorComplejo::vectoraddfront(const complejo &value)
{
    if (size == capacity)
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
void VectorComplejo::vectorremove(const complejo &value)
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

int VectorComplejo::vectorfind(int index, const complejo &value) const
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
// Capacidad / tamaños
// ==========================
int VectorComplejo::vectorisfull() const { return size == capacity; }
void VectorComplejo::vectorresize()
{
    int newCap = capacity > 0 ? capacity * 2 : 1;
    complejo *b = new complejo[newCap];
    for (int i = 0; i < size; ++i)
    {
        b[i] = a[i];
    }
    delete[] a;
    a = b;
    capacity = newCap;
}
int VectorComplejo::vectorgetsize() const { return size; }
int VectorComplejo::vectorgetmaxsize() const { return capacity; }

// ==========================
// Operaciones con int
// ==========================
VectorComplejo VectorComplejo::operator+(int s) const
{
    VectorComplejo r(capacity);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] + complejo(s, 0.0);
    }
    return r;
}
VectorComplejo VectorComplejo::operator-(int s) const
{
    VectorComplejo r(capacity);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] - complejo(s, 0.0);
    }
    return r;
}
VectorComplejo VectorComplejo::operator*(int s) const
{
    VectorComplejo r(capacity);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] * complejo(s, 0.0);
    }
    return r;
}
VectorComplejo VectorComplejo::operator/(int s) const
{
    if (s == 0)
    {
        throw std::runtime_error("division por cero");
    }
    VectorComplejo r(capacity);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] / complejo(s, 0.0);
    }
    return r;
}

// ==========================
// Operaciones con complejo
// ==========================
VectorComplejo VectorComplejo::operator+(const complejo &c) const
{
    VectorComplejo r(capacity);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] + c;
    }
    return r;
}
VectorComplejo VectorComplejo::operator-(const complejo &c) const
{
    VectorComplejo r(capacity);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] - c;
    }
    return r;
}
VectorComplejo VectorComplejo::operator*(const complejo &c) const
{
    VectorComplejo r(capacity);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] * c;
    }
    return r;
}
VectorComplejo VectorComplejo::operator/(const complejo &c) const
{
    VectorComplejo r(capacity);
    r.size = size;
    for (int i = 0; i < size; ++i)
    {
        r.a[i] = a[i] / c;
    }
    return r;
}

// ==========================
// Simetría (no-miembro)
// ==========================
VectorComplejo operator-(int s, const VectorComplejo &v)
{
    VectorComplejo r(v.vectorgetmaxsize());
    int i = 0;
    while (i < v.vectorgetsize())
    {
        r.vectoraddback(complejo(s, 0.0) - v.vectorget(i));
        ++i;
    }
    return r;
}
VectorComplejo operator/(int s, const VectorComplejo &v)
{
    VectorComplejo r(v.vectorgetmaxsize());
    int i = 0;
    while (i < v.vectorgetsize())
    {
        r.vectoraddback(complejo(s, 0.0) / v.vectorget(i));
        ++i;
    }
    return r;
}
// complejo - VectorComplejo  (usar copia mutable de c)
VectorComplejo operator-(const complejo &c, const VectorComplejo &v)
{
    VectorComplejo r(v.vectorgetmaxsize());
    int i = 0;
    while (i < v.vectorgetsize())
    {
        complejo lhs = c; // copia mutable
        r.vectoraddback(lhs - v.vectorget(i));
        ++i;
    }
    return r;
}

// complejo / VectorComplejo  (usar copia mutable de c)
VectorComplejo operator/(const complejo &c, const VectorComplejo &v)
{
    VectorComplejo r(v.vectorgetmaxsize());
    int i = 0;
    while (i < v.vectorgetsize())
    {
        complejo lhs = c; // copia mutable
        r.vectoraddback(lhs / v.vectorget(i));
        ++i;
    }
    return r;
}

// ==========================
// Impresión
// ==========================
void VectorComplejo::print(std::ostream &os) const
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
