#ifndef SOLVE_AND_EXPORT_H
#define SOLVE_AND_EXPORT_H

#include <string>
#include "MatrizComplejo.h"
#include "VectorComplejo.h"
#include "vectorIncognitas.h"

enum class NumFormat : int;

// Resuelve A·b = c  → devuelve en 'sol' el vector de corrientes (mismo orden que 'incog')
// Retorna false si A no es cuadrada/invertible o si dims no coinciden.
bool solveABC(const MatrizComplejo &A, const VectorComplejo &c, VectorComplejo &sol);

// Guarda resultados como pares etiqueta → valor complejo
// TXT: una línea por etiqueta:  <label>   <re> <im>
bool saveSolutionToTxt(const std::string &path, const VectorIncognitas &labels, const VectorComplejo &sol);

// BIN:
// [int32 n]
//  Repito n veces:  [int32 len] [len bytes UTF-8 de label]  [double re] [double im]
bool saveSolutionToBin(const std::string &path, const VectorIncognitas &labels, const VectorComplejo &sol);

// Pipeline directo desde archivo de entrada del sistema (labels):
// Cargan (TXT/BIN), resuelven, y guardan el resultado en (TXT/BIN) de salida.
bool solveFromTxtAndSave(const std::string &inPathTxt, const std::string &outPathTxt, const std::string &outPathBin);

bool solveFromBinAndSave(const std::string &inPathBin, const std::string &outPathTxt, const std::string &outPathBin);

// Nuevos: mismo contenido pero elegís salida
bool saveSolutionToTxtFmt(const std::string &path, const VectorIncognitas &labels, const VectorComplejo &sol, NumFormat fmt);
bool saveSolutionToBinFmt(const std::string &path, const VectorIncognitas &labels, const VectorComplejo &sol, NumFormat fmt);

// Pipelines con formato elegido para la SALIDA
bool solveFromTxtAndSaveFmt(const std::string &inPathTxt, NumFormat outFmt,
                            const std::string &outPathTxt, const std::string &outPathBin);

bool solveFromBinAndSaveFmt(const std::string &inPathBin, NumFormat outFmt,
                            const std::string &outPathTxt, const std::string &outPathBin);

#endif 
