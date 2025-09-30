#ifndef SAVER_H
#define SAVER_H

#include <string>
#include "vectorIncognitas.h"
#include "vectorComplejo.h"

// Guardar solución en TXT/BIN (rectangular re,im)
bool saveSolutionTxtRect(const std::string &path,
                         const VectorIncognitas &labels,
                         const VectorComplejo &x);

bool saveSolutionBinRect(const std::string &path,
                         const VectorIncognitas &labels,
                         const VectorComplejo &x);

// Guardar solución en TXT/BIN (fasor módulo∠ángulo°)
bool saveSolutionTxtFasor(const std::string &path,
                          const VectorIncognitas &labels,
                          const VectorComplejo &x);

bool saveSolutionBinFasor(const std::string &path,
                          const VectorIncognitas &labels,
                          const VectorComplejo &x);

#endif // SAVER_H
