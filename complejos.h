// fichero complejos.h
// declaraci�n de la clase complejo

#ifndef COMPLEJOS_H_INCLUDED
#define COMPLEJOS_H_INCLUDED

#include <iostream>
#include <cmath>

using namespace std;

class complejo
{
protected:
    double m_real;
    double m_imag;

public:
    // Constructores
    // complejo(void);
    complejo(double re = 0, double im = 0.0);
    // complejo(const complejo&);     // constructor de copia proporcionado por el sistema

    // sets
    void SetData(void);
    void SetReal(double);
    void SetImag(double);

    // gets
    double GetReal()const;
    double GetImag()const;

    double Modulo() const;          // |z| = hypot(re, im)
    double FaseRad() const;         // arg(z) en radianes = atan2(im, re)
    double FaseDeg() const;         // arg(z) en grados

    // Sobrecarga de operadores aritmeticos
    complejo &operator+(const complejo &);
    complejo operator-(const complejo &);
    complejo operator*(const complejo &);
    complejo operator/(const complejo &);

    complejo operator+=(const complejo &);

    complejo &operator++();
    complejo &operator++(int);

    // Sobrecarga del operador de asignacion
    complejo operator=(const complejo &); // retorna una referencia para poder hacer a = b = c

    // Sobrecarga de operadores de comparacion
    //    friend int operator== (const complejo&, const complejo&);
    friend bool operator!=(const complejo &, const complejo &); // mas amigable al utilizar 2 operandos en la comparacion
    bool operator==(const complejo &);                          // sin utilizar el modificador friend
    friend complejo operator+(double, complejo);

    // Sobrecarga del operador de insercion en el stream de salida
    friend ostream &operator<<(ostream &, const complejo &);
};

#endif
