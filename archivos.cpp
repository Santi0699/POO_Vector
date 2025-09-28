#include "archivos.h"
#include <fstream>
#include <sstream>
#include <cstdint>

// ------------------------------
// Helpers internos (sin break/return dentro de bucles)
// ------------------------------
static void assignOrAppend(vector &dst, vector &src, bool append)
{
    if (append)
    {
        int i = 0;
        while (i < src.vectorgetsize())
        {
            dst.vectoraddback(src.vectorget(i));
            ++i;
        }
    }
    else
    {
        dst = src; // tu operator= de vector
    }
}

static void assignOrAppend(VectorComplejo &dst, VectorComplejo &src, bool append)
{
    if (append)
    {
        int i = 0;
        while (i < src.vectorgetsize())
        {
            dst.vectoraddback(src.vectorget(i));
            ++i;
        }
    }
    else
    {
        dst = src; // tu operator= de VectorComplejo
    }
}

// ==============================
// TXT: enteros
// ==============================
bool loadIntsFromTxt(const std::string &path, vector &out, bool append)
{
    std::ifstream fi(path);
    if (!fi.good())
    {
        return false;
    }

    vector tmp(16);
    std::string line;
    bool leer = true;

    while (leer)
    {
        if (!std::getline(fi, line))
        {
            leer = false;
        }
        else
        {
            std::istringstream iss(line);
            long long v = 0;
            bool seguir = true;
            while (seguir)
            {
                if (iss >> v)
                {
                    tmp.vectoraddback(static_cast<int>(v));
                }
                else
                {
                    seguir = false;
                }
            }
        }
    }

    assignOrAppend(out, tmp, append);
    return true;
}

// ==============================
// TXT: complejos (pares real imag)
// ==============================
bool loadComplejosFromTxt(const std::string &path, VectorComplejo &out, bool append)
{
    std::ifstream fi(path);
    if (!fi.good())
    {
        return false;
    }

    VectorComplejo tmp(16);
    std::string line;
    bool leer = true;

    while (leer)
    {
        if (!std::getline(fi, line))
        {
            leer = false;
        }
        else
        {
            std::istringstream iss(line);
            double re = 0.0, im = 0.0;
            bool seguir = true;
            while (seguir)
            {
                if (iss >> re)
                {
                    if (iss >> im)
                    {
                        tmp.vectoraddback(complejo(re, im));
                    }
                    else
                    {
                        seguir = false; // impar de números en la línea: se ignora el último
                    }
                }
                else
                {
                    seguir = false;
                }
            }
        }
    }

    assignOrAppend(out, tmp, append);
    return true;
}

// ==============================
// BIN: enteros  [count][val...]
// ==============================
bool loadIntsFromBin(const std::string &path, vector &out, bool append)
{
    std::ifstream fb(path, std::ios::binary);
    if (!fb.good())
    {
        return false;
    }

    int32_t count = 0;
    fb.read(reinterpret_cast<char *>(&count), sizeof(count));
    if (!fb.good() || count < 0)
    {
        return false;
    }

    vector tmp(count > 0 ? count : 1);
    int i = 0;
    bool seguir = true;

    while (seguir)
    {
        if (i < count)
        {
            int32_t v = 0;
            fb.read(reinterpret_cast<char *>(&v), sizeof(v));
            if (fb.good())
            {
                tmp.vectoraddback(static_cast<int>(v));
                ++i;
            }
            else
            {
                seguir = false;
            }
        }
        else
        {
            seguir = false;
        }
    }

    if (i != count)
    {
        return false;
    }
    assignOrAppend(out, tmp, append);
    return true;
}

// ==============================
// BIN: complejos  [count][re,im]...
// ==============================
bool loadComplejosFromBin(const std::string &path, VectorComplejo &out, bool append)
{
    std::ifstream fb(path, std::ios::binary);
    if (!fb.good())
    {
        return false;
    }

    int32_t count = 0;
    fb.read(reinterpret_cast<char *>(&count), sizeof(count));
    if (!fb.good() || count < 0)
    {
        return false;
    }

    VectorComplejo tmp(count > 0 ? count : 1);
    int i = 0;
    bool seguir = true;

    while (seguir)
    {
        if (i < count)
        {
            double re = 0.0, im = 0.0;
            fb.read(reinterpret_cast<char *>(&re), sizeof(re));
            if (fb.good())
            {
                fb.read(reinterpret_cast<char *>(&im), sizeof(im));
                if (fb.good())
                {
                    tmp.vectoraddback(complejo(re, im));
                    ++i;
                }
                else
                {
                    seguir = false;
                }
            }
            else
            {
                seguir = false;
            }
        }
        else
        {
            seguir = false;
        }
    }

    if (i != count)
    {
        return false;
    }
    assignOrAppend(out, tmp, append);
    return true;
}

// ==============================
// SAVE: enteros -> TXT (un valor por línea)
// ==============================
bool saveIntsToTxt(const std::string &path, const vector &src)
{
    std::ofstream fo(path, std::ios::trunc);
    if (!fo.good())
    {
        return false;
    }

    int i = 0;
    while (i < src.vectorgetsize())
    {
        fo << src.vectorget(i) << "\n";
        ++i;
    }
    return fo.good();
}

// ==============================
// SAVE: complejos -> TXT ("re im" por línea)
// ==============================
bool saveComplejosToTxt(const std::string &path, const VectorComplejo &src)
{
    std::ofstream fo(path, std::ios::trunc);
    if (!fo.good())
    {
        return false;
    }

    int i = 0;
    while (i < src.vectorgetsize())
    {
        // COPIA mutable
        complejo c = src.vectorget(i);
        fo << c.GetReal() << " " << c.GetImag() << "\n";
        ++i;
    }
    return fo.good();
}

// ==============================
// SAVE: enteros -> BIN  [count][val...]
// ==============================
bool saveIntsToBin(const std::string &path, const vector &src)
{
    std::ofstream fb(path, std::ios::binary | std::ios::trunc);
    if (!fb.good())
    {
        return false;
    }

    const int32_t count = static_cast<int32_t>(src.vectorgetsize());
    fb.write(reinterpret_cast<const char *>(&count), sizeof(count));

    int i = 0;
    bool ok = fb.good();
    while (i < src.vectorgetsize() && ok)
    {
        int32_t v = static_cast<int32_t>(src.vectorget(i));
        fb.write(reinterpret_cast<const char *>(&v), sizeof(v));
        if (fb.good())
        {
            ++i;
        }
        else
        {
            ok = false;
        }
    }
    return ok && fb.good();
}

// ==============================
// SAVE: complejos -> BIN  [count][re,im]...
// ==============================
bool saveComplejosToBin(const std::string &path, const VectorComplejo &src)
{
    std::ofstream fb(path, std::ios::binary | std::ios::trunc);
    if (!fb.good())
    {
        return false;
    }

    const int32_t count = static_cast<int32_t>(src.vectorgetsize());
    fb.write(reinterpret_cast<const char *>(&count), sizeof(count));

    int i = 0;
    bool ok = fb.good();
    while (i < src.vectorgetsize() && ok)
    {
        // COPIA mutable
        complejo c = src.vectorget(i);
        double re = c.GetReal();
        double im = c.GetImag();

        fb.write(reinterpret_cast<const char *>(&re), sizeof(re));
        if (fb.good())
        {
            fb.write(reinterpret_cast<const char *>(&im), sizeof(im));
            if (fb.good())
            {
                ++i;
            }
            else
            {
                ok = false;
            }
        }
        else
        {
            ok = false;
        }
    }
    return ok && fb.good();
}
