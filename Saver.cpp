#include "Saver.h"
#include <fstream>
#include <cstdint>
#include "FasorConv.h"
#include "complejos.h"

bool saveSolutionTxtRect(const std::string &path,
                         const VectorIncognitas &labels,
                         const VectorComplejo &x)
{
    if (labels.size() != x.vectorgetsize())
        return false;
    std::ofstream fo(path, std::ios::trunc);
    if (!fo.good())
        return false;
    int i = 0;
    while (i < labels.size())
    {
        complejo z = x.vectorget(i);
        fo << labels.get(i) << "\t" << z.GetReal() << " " << z.GetImag() << "\n";
        ++i;
    }
    return fo.good();
}

bool saveSolutionBinRect(const std::string &path,
                         const VectorIncognitas &labels,
                         const VectorComplejo &x)
{
    if (labels.size() != x.vectorgetsize())
        return false;
    std::ofstream fb(path, std::ios::binary | std::ios::trunc);
    if (!fb.good())
        return false;
    int32_t n = (int32_t)labels.size();
    fb.write((const char *)&n, sizeof(n));
    int i = 0;
    bool ok = fb.good();
    while (i < n && ok)
    {
        std::string s = labels.get(i);
        int32_t len = (int32_t)s.size();
        fb.write((const char *)&len, sizeof(len));
        if (fb.good())
        {
            if (len > 0)
            {
                fb.write(s.data(), len);
                if (!fb.good())
                    ok = false;
            }
        }
        else
            ok = false;

        complejo z = x.vectorget(i);
        double re = z.GetReal(), im = z.GetImag();
        if (ok)
            fb.write((const char *)&re, sizeof(re));
        if (fb.good())
            fb.write((const char *)&im, sizeof(im));
        if (!fb.good())
            ok = false;
        ++i;
    }
    return ok && fb.good();
}

bool saveSolutionTxtFasor(const std::string &path,
                          const VectorIncognitas &labels,
                          const VectorComplejo &x)
{
    if (labels.size() != x.vectorgetsize())
        return false;
    std::ofstream fo(path, std::ios::trunc);
    if (!fo.good())
        return false;
    int i = 0;
    while (i < labels.size())
    {
        auto fa = complejoToFasor(x.vectorget(i));
        fo << labels.get(i) << "\t" << fa.first << " ∠ " << fa.second << "°\n";
        ++i;
    }
    return fo.good();
}

bool saveSolutionBinFasor(const std::string &path,
                          const VectorIncognitas &labels,
                          const VectorComplejo &x)
{
    if (labels.size() != x.vectorgetsize())
        return false;
    std::ofstream fb(path, std::ios::binary | std::ios::trunc);
    if (!fb.good())
        return false;
    int32_t n = (int32_t)labels.size();
    fb.write((const char *)&n, sizeof(n));
    int i = 0;
    bool ok = fb.good();
    while (i < n && ok)
    {
        std::string s = labels.get(i);
        int32_t len = (int32_t)s.size();
        fb.write((const char *)&len, sizeof(len));
        if (fb.good())
        {
            if (len > 0)
            {
                fb.write(s.data(), len);
                if (!fb.good())
                    ok = false;
            }
        }
        else
            ok = false;

        auto fa = complejoToFasor(x.vectorget(i));
        double mod = fa.first, ang = fa.second;
        if (ok)
            fb.write((const char *)&mod, sizeof(mod));
        if (fb.good())
            fb.write((const char *)&ang, sizeof(ang));
        if (!fb.good())
            ok = false;
        ++i;
    }
    return ok && fb.good();
}
