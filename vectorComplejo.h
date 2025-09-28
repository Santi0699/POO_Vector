#ifndef VECTOR_COMPLEJO_H
#define VECTOR_COMPLEJO_H

#include <stdexcept>
#include <iostream>
#include "complejos.h"

class VectorComplejo
{
private:
    complejo *a;
    int size;
    int sizemax;

public:
    explicit VectorComplejo(int sizemax = 10);
    VectorComplejo(const VectorComplejo &other);
    VectorComplejo &operator=(const VectorComplejo &other);
    ~VectorComplejo();

    // Acceso / modificación
    const complejo &vectorget(int index) const;
    void vectorset(int index, const complejo &value);

    // Inserción y agregado
    void vectorinsert(int index, const complejo &value);
    void vectoraddback(const complejo &value);
    void vectoraddfront(const complejo &value);

    // Borrado / búsqueda
    void vectorremove(const complejo &value); // usa operator== de complejo
    int vectorfind(int index, const complejo &value) const;

    // Capacidad
    int vectorisfull() const;
    void vectorresize();

    // Tamaños
    int vectorgetsize() const;
    int vectorgetmaxsize() const;

    // --- Operadores con ESCALAR int y complejo -> devuelve VectorComplejo ---
    VectorComplejo operator+(int num) const;
    VectorComplejo operator-(int num) const;
    VectorComplejo operator*(int num) const;
    VectorComplejo operator/(int num) const; // divide cada complejo por s

    VectorComplejo operator+(const complejo &comp) const;
    VectorComplejo operator-(const complejo &comp) const;
    VectorComplejo operator*(const complejo &comp) const;
    VectorComplejo operator/(const complejo &comp) const;

    // Simetría: escalar op VectorComplejo (no-miembro)
    friend VectorComplejo operator+(int num, const VectorComplejo &vector) 
    { 
        return vector + num; 
    }
    friend VectorComplejo operator*(int s, const VectorComplejo &v) 
    { 
        return v * s; 
    }
    friend VectorComplejo operator+(const complejo &c, const VectorComplejo &v) 
    { 
        return v + c; 
    }
    friend VectorComplejo operator*(const complejo &c, const VectorComplejo &v) 
    { 
        return v * c; 
    }

    friend VectorComplejo operator-(int num, const VectorComplejo &v);
    friend VectorComplejo operator/(int num, const VectorComplejo &v);
    friend VectorComplejo operator-(const complejo &comp, const VectorComplejo &vector);
    friend VectorComplejo operator/(const complejo &comp, const VectorComplejo &vecotr);

    // Impresión
    void print(std::ostream &os = std::cout) const;
    friend std::ostream &operator<<(std::ostream &os, const VectorComplejo &v)
    {
        v.print(os);
        return os;
    }
};

#endif // VECTOR_COMPLEJO_H
