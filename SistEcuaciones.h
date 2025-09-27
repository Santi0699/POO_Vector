#ifndef SIST_ECUACIONES_H
#define SIST_ECUACIONES_H

#include <string>
#include "MatrizComplejo.h"
#include "VectorComplejo.h"
#include "VectorIncognitas.h"

// -------------------- Carga --------------------
// TXT soportado (formato claro y robusto):
// Bloques en cualquier orden:
//   A:
//   <filas de A, cada celda como "re im">
//   b:
//   <n lineas; cada linea UNA etiqueta, ej: i1>
//   c:
//   <n lineas; cada linea "re im">
// Ejemplo:
//   A:
//   5 0   -2 1
//   -2 -1  7 0
//   b:
//   i1
//   i2
//   c:
//   10 0
//   0 0
//
// BIN:
// [int32 rows][int32 cols]                        // A (rows x cols)
// Luego rows*cols (double re, double im) row-major
// Luego b:
//   [int32 n] == cols
//   Para k en 0..n-1: [int32 len] [len bytes UTF-8 sin '\0']
// Luego c:
//   [int32 n] == rows
//   Para k en 0..n-1: (double re, double im)
bool loadABCLabelsFromTxt(const std::string& path, MatrizComplejo& A, VectorIncognitas& b, VectorComplejo& c);
bool loadABCLabelsFromBin(const std::string& path, MatrizComplejo& A, VectorIncognitas& b, VectorComplejo& c);

// -------------------- Guardado --------------------
// TXT: escribe en bloques A:/b:/c: (un label por línea en b)
bool saveABCLabelsToTxt(const std::string& path, const MatrizComplejo& A, const VectorIncognitas& b, const VectorComplejo& c);
// BIN: exactamente el formato descripto arriba
bool saveABCLabelsToBin(const std::string& path, const MatrizComplejo& A, const VectorIncognitas& b, const VectorComplejo& c);

#endif // SYSTEM_ABC_H