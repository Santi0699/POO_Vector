#include "FasorConv.h"
#include <cmath>

complejo fasorToComplejo(double modulo, double angDeg)
{
    double th = angDeg * M_PI / 180.0;
    return complejo(modulo * std::cos(th), modulo * std::sin(th));
}

std::pair<double, double> complejoToFasor(const complejo &z)
{
    double re = z.GetReal(), im = z.GetImag();
    double mod = std::sqrt(re * re + im * im);
    double ang = std::atan2(im, re) * 180.0 / M_PI;
    return {mod, ang};
}
