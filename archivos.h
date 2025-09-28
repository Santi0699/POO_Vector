#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <string>
#include "vector.h"
#include "vectorComplejo.h"
#include "complejos.h"

// -------- CARGA DESDE TXT --------
// enteros: números separados por espacios o saltos de línea
bool loadIntsFromTxt(const std::string &path, vector &out, bool append = false);
// complejos: pares "real imag" (double double) separados por espacios o saltos de línea
bool loadComplejosFromTxt(const std::string &path, VectorComplejo &out, bool append = false);

// -------- CARGA DESDE BIN --------
// enteros (bin): [int32_t count] [int32_t x count]
bool loadIntsFromBin(const std::string &path, vector &out, bool append = false);
// complejos (bin): [int32_t count] [(double real, double imag) x count]
bool loadComplejosFromBin(const std::string &path, VectorComplejo &out, bool append = false);

// -------- GUARDADO A TXT --------
// enteros en texto (un valor por línea)
bool saveIntsToTxt(const std::string &path, const vector &src);
// complejos en texto ("real imag" por línea)
bool saveComplejosToTxt(const std::string &path, const VectorComplejo &src);

// -------- GUARDADO A BIN --------
// enteros (bin): [int32_t count] [int32_t x count]
bool saveIntsToBin(const std::string &path, const vector &src);
// complejos (bin): [int32_t count] [(double real, double imag) x count]
bool saveComplejosToBin(const std::string &path, const VectorComplejo &src);

#endif // FILE_IO_H
