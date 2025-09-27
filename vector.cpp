#include "vector.h"

#include "Vector.h"
#include <stdexcept> // std::out_of_range, std::runtime_error

// ---------------------------
// Constructor / Destructor
// ---------------------------
vector::vector(int initial)
{
    if (initial <= 0) initial = 1;
    a = new int[initial];
    size = 0;
    maxsize = initial;
}

vector::~vector()
{
    delete[] a;
    a = nullptr;
    size = 0;
    maxsize = 0;
}

// ---------------------------
// Acceso / set (sin expandir)
// ---------------------------
int vector::vectorget(int index)
{
    if (index < 0 || index >= size) {
        throw std::out_of_range("vectorget: indice fuera de rango");
    }
    return a[index];
}

void vector::vectorset(int index, int value)
{
    if (index < 0 || index >= size) {
        throw std::out_of_range("vectorset: indice fuera de rango (no expande)");
    }
    a[index] = value;
}

// ---------------------------
// Inserción (desplaza a der.)
// ---------------------------
void vector::vectorinsert(int index, int value)
{
    if (index < 0 || index > size) {
        throw std::out_of_range("vectorinsert: indice invalido (permitido 0..size)");
    }
    if (size == maxsize) {
        vectorresize();
    }
    // desplazar [index..size-1] una posición a la derecha
    for (int i = size; i > index; --i) {
        a[i] = a[i - 1];
    }
    a[index] = value;
    ++size;
}

// ---------------------------
// Capacidad
// ---------------------------
int vector::vectorisfull()
{
    return (size == maxsize) ? 1 : 0;
}

void vector::vectorresize()
{
    int newCap = (maxsize > 0) ? (maxsize * 2) : 1;
    int* b = new int[newCap];

    for (int i = 0; i < size; ++i) {
        b[i] = a[i];
    }

    delete[] a;
    a = b;
    maxsize = newCap;
}

// ---------------------------
// Eliminación por valor
// ---------------------------
void vector::vectorremove(int value)
{
    // elimina la PRIMERA ocurrencia de 'value' si existe
    int pos = -1;
    for (int i = 0; i < size; ++i) {
        if (a[i] == value) { pos = i; break; }
    }
    if (pos == -1) return; // no encontrado, no hace nada

    for (int i = pos; i < size - 1; ++i) {
        a[i] = a[i + 1];
    }
    --size;
}

// ---------------------------
// Búsqueda
// ---------------------------
int vector::vectorfind(int index, int value)
{
    if (index < 0 || index >= size) return -1;
    for (int i = index; i < size; ++i) {
        if (a[i] == value) return i;
    }
    return -1;
}

// ---------------------------
// Agregar atrás / adelante
// ---------------------------
void vector::vectoraddback(int value)
{
    if (size == maxsize) {
        vectorresize();
    }
    a[size++] = value;
}

void vector::vectoraddfront(int value)
{
    if (size == maxsize) {
        vectorresize();
    }
    for (int i = size; i > 0; --i) {
        a[i] = a[i - 1];
    }
    a[0] = value;
    ++size;
}

// ---------------------------
// Size / MaxSize
// ---------------------------
int vector::vectorgetsize()
{
    return size;
}

int vector::vectorgetmaxsize()
{
    return maxsize;
}

// ---------------------------
// Operadores unarios
// ---------------------------
vector vector::operator+() const
{
    // copia idéntica
    vector r(maxsize);
    r.size = size;
    for (int i = 0; i < size; ++i) r.a[i] = +a[i];
    return r;
}

vector vector::operator-() const
{
    // copia con elementos negados
    vector r(maxsize);
    r.size = size;
    for (int i = 0; i < size; ++i) r.a[i] = -a[i];
    return r;
}

vector vector::operator*() const
{
    // ELECCIÓN DOCUMENTADA:
    // devuelve copia con cada elemento al cuadrado (a[i] * a[i])
    vector r(maxsize);
    r.size = size;
    for (int i = 0; i < size; ++i) r.a[i] = a[i] * a[i];
    return r;
}
