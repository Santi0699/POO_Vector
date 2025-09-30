#ifndef LOADER_H
#define LOADER_H

#include <string>
#include "matrizComplejo.h"
#include "vectorComplejo.h"
#include "vectorIncognitas.h"

// Orden para pares rectangulares
enum class PairOrder { ReIm, ImRe };  // real,imag  ó  imag,real

// =================== CARGA INTEGRADA (1 archivo) ===================
// Rectangular (re, im) — ahora con orden configurable
bool loadFromTxtRect(const std::string& path,
                     MatrizComplejo& A,
                     VectorIncognitas& b,
                     VectorComplejo& c,
                     PairOrder order = PairOrder::ReIm);

bool loadFromBinRect(const std::string& path,
                     MatrizComplejo& A,
                     VectorIncognitas& b,
                     VectorComplejo& c,
                     PairOrder order = PairOrder::ReIm);

// Fasor (módulo, ángulo°) — sin cambios
bool loadFromTxtFasor(const std::string& path,
                      MatrizComplejo& A,
                      VectorIncognitas& b,
                      VectorComplejo& c);
bool loadFromBinFasor(const std::string& path,
                      MatrizComplejo& A,
                      VectorIncognitas& b,
                      VectorComplejo& c);

// =================== CARGA POR ARCHIVO SEPARADO ====================
// --- TXT RECT ---
bool loadATxtRect (const std::string& pathA, MatrizComplejo& A, PairOrder order = PairOrder::ReIm);
bool loadBTxt     (const std::string& pathB, VectorIncognitas& b);
bool loadCTxtRect (const std::string& pathC, VectorComplejo& c, PairOrder order = PairOrder::ReIm);

// --- TXT FASOR ---
bool loadATxtFasor(const std::string& pathA, MatrizComplejo& A);
bool loadCTxtFasor(const std::string& pathC, VectorComplejo& c);

// --- BIN RECT ---
bool loadABinRect (const std::string& pathA, MatrizComplejo& A, PairOrder order = PairOrder::ReIm);
bool loadBBin     (const std::string& pathB, VectorIncognitas& b);
bool loadCBinRect (const std::string& pathC, VectorComplejo& c, PairOrder order = PairOrder::ReIm);

// --- BIN FASOR ---
bool loadABinFasor(const std::string& pathA, MatrizComplejo& A);
bool loadCBinFasor(const std::string& pathC, VectorComplejo& c);

// =================== WRAPPERS 3-RUTAS ==============================
// Rect
bool load3RectTxt(const std::string& pathA,
                  const std::string& pathB,
                  const std::string& pathC,
                  MatrizComplejo& A, VectorIncognitas& b, VectorComplejo& c,
                  PairOrder order = PairOrder::ReIm);

bool load3RectBin(const std::string& pathA,
                  const std::string& pathB,
                  const std::string& pathC,
                  MatrizComplejo& A, VectorIncognitas& b, VectorComplejo& c,
                  PairOrder order = PairOrder::ReIm);

// Fasor (sin cambios)
bool load3FasorTxt(const std::string& pathA,
                   const std::string& pathB,
                   const std::string& pathC,
                   MatrizComplejo& A, VectorIncognitas& b, VectorComplejo& c);
bool load3FasorBin(const std::string& pathA,
                   const std::string& pathB,
                   const std::string& pathC,
                   MatrizComplejo& A, VectorIncognitas& b, VectorComplejo& c);

#endif // LOADER_H
