#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>
#include <iostream>
#include "complejos.h"   // para interoperar con complejo

class VectorComplejo;    // forward decl para firmas que retornan VectorComplejo

class vector
{
private:
    int* a;
    int size;
    int capacity;

public:
    explicit vector(int capacity = 10);
    vector(const vector& other);
    vector& operator=(const vector& other);
    ~vector();

    // Acceso / modificación
    int  vectorget(int index) const;
    void vectorset(int index, int value);

    // Inserción y agregado
    void vectorinsert(int index, int value);
    void vectoraddback(int value);
    void vectoraddfront(int value);

    // Borrado / búsqueda
    void vectorremove(int value);            // elimina 1ra ocurrencia
    int  vectorfind(int index, int value) const;

    // Capacidad
    int  vectorisfull() const;
    void vectorresize();

    // Tamaños
    int  vectorgetsize() const;
    int  vectorgetmaxsize() const;

    // --- Operadores con ESCALAR int -> devuelve vector (int) ---
    vector operator+(int s) const;
    vector operator-(int s) const;
    vector operator*(int s) const;
    vector operator/(int s) const;  // división entera

    // Permitir int + vector (amigos no-miembro)
    friend vector operator+(int s, const vector& v) { return v + s; }
    friend vector operator-(int s, const vector& v);
    friend vector operator*(int s, const vector& v) { return v * s; }

    // --- Operadores con ESCALAR complejo -> devuelve VectorComplejo ---
    VectorComplejo operator+(const complejo& c) const;
    VectorComplejo operator-(const complejo& c) const;
    VectorComplejo operator*(const complejo& c) const;
    VectorComplejo operator/(const complejo& c) const;

    // Permitir complejo + vector (no-miembro)
    friend VectorComplejo operator+(const complejo& c, const vector& v);
    friend VectorComplejo operator-(const complejo& c, const vector& v);
    friend VectorComplejo operator*(const complejo& c, const vector& v);
    friend VectorComplejo operator/(const complejo& c, const vector& v);

    // Impresión
    void print(std::ostream& os = std::cout) const;
    friend std::ostream& operator<<(std::ostream& os, const vector& v) {
        v.print(os);
        return os;
    }
};

#endif // VECTOR_H
