#ifndef FASOR_CONV_H
#define FASOR_CONV_H

#include "complejos.h"
#include <utility>

// De (módulo, ángulo°) a complejo
complejo fasorToComplejo(double modulo, double angDeg);

// De complejo a (módulo, ángulo°)
std::pair<double, double> complejoToFasor(const complejo &z);

#endif // FASOR_CONV_H
