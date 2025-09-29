#include "SistEcuaciones.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <cmath>        // <- nuevo: cos/sin para polar
#include "complejos.h"

// ======================================================
// Helpers (sin break/return en bucles; estilo consistente)
// ======================================================
static bool isBlank(const std::string &s)
{
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r'))
    {
        ++i;
    }
    return i == s.size();
}

static bool parseDoublesLine(const std::string &text, std::vector<double> &out)
{
    std::istringstream iss(text);
    double x = 0.0;
    bool any = false;
    bool cont = true;
    while (cont)
    {
        if (iss >> x)
        {
            out.push_back(x);
            any = true;
        }
        else
        {
            cont = false;
        }
    }
    return any;
}

static complejo makeC(double re, double im)
{
    complejo z(re, im);
    return z;
}

constexpr double PI = 3.14159265358979323846;

// Construye complejo desde (módulo, ángulo) en grados o radianes
static complejo makeFromPolar(double mag, double ang, bool degrees)
{
    const double theta = degrees ? (ang * PI / 180.0) : ang;
    return complejo(mag * std::cos(theta), mag * std::sin(theta));
}

// Heurística: decide si una tabla de pares (a,b) parece Rect, PolarDeg o PolarRad
// Criterio: si la mayoría de los "b" caen en +/-360 o +/-2pi y los "a" raramente son negativos → polar.
//enum class NumFormat : int { Rect = 0, PolarDeg = 1, PolarRad = 2 };

static NumFormat detectFmtPairs(const std::vector<std::vector<double>>& rows /*cada fila: a b a b ...*/)
{
    long totalPairs = 0, negMag = 0, within360 = 0, within2pi = 0;
    int r = 0;
    while (r < (int)rows.size())
    {
        const auto& line = rows[r];
        int j = 0;
        while (j + 1 < (int)line.size())
        {
            double a = line[j], b = line[j + 1];
            ++totalPairs;
            if (a < 0) ++negMag;
            if (std::isfinite(b))
            {
                if (std::fabs(b) <= 360.0 + 1e-9) ++within360;
                if (std::fabs(b) <= 2.0 * PI + 1e-9) ++within2pi;
            }
            j += 2;
        }
        ++r;
    }
    if (totalPairs == 0) return NumFormat::Rect;

    // si >10% de módulos negativos, preferimos rectangular
    if ((double)negMag / (double)totalPairs > 0.10) return NumFormat::Rect;

    double f360 = (double)within360 / (double)totalPairs;
    double f2pi = (double)within2pi / (double)totalPairs;

    if (f360 >= 0.80 || f2pi >= 0.80)
    {
        if (f360 >= f2pi) return NumFormat::PolarDeg;
        else return NumFormat::PolarRad;
    }
    return NumFormat::Rect;
}

// ======================================================
// TXT
// Formato soportado:
//  A:            (obligatorio)
//  [polar_deg|polar_rad|rect]   (opcional, línea de control)
//  filas con pares "a b"  (rect: re im) (polar: mag ang)
//  b:
//  n líneas con etiquetas
//  c:
//  m líneas con pares "a b" (rect o polar; admite línea de control igual que A)
// Si no hay línea de control, se aplica heurística.
// ======================================================
bool loadABCLabelsFromTxt(const std::string &path, MatrizComplejo &A, VectorIncognitas &b, VectorComplejo &c)
{
    std::ifstream fi(path);
    if (!fi.good())
    {
        return false;
    }

    std::vector<std::vector<double>> rowsA; // cada fila: ... a b a b ...
    std::vector<std::string> rowsB;         // cada fila: UNA etiqueta
    std::vector<std::vector<double>> rowsC; // cada fila: a b

    bool inA = false, inB = false, inC = false;
    // flags de formato por bloque (con "línea de control" opcional)
    NumFormat aFmt = NumFormat::Rect, cFmt = NumFormat::Rect;
    bool aFmtForced = false, cFmtForced = false;

    std::string line;
    bool reading = true;
    while (reading)
    {
        if (!std::getline(fi, line))
        {
            reading = false;
        }
        else
        {
            if (isBlank(line))
            {
                // nada
            }
            else if (line == "A:" || line == "a:" || line == "A" || line == "a")
            {
                inA = true; inB = false; inC = false;
            }
            else if (line == "b:" || line == "B:" || line == "b" || line == "B")
            {
                inA = false; inB = true; inC = false;
            }
            else if (line == "c:" || line == "C:" || line == "c" || line == "C")
            {
                inA = false; inB = false; inC = true;
            }
            else
            {
                // detección de líneas de control dentro de A o C
                if (inA || inC)
                {
                    if (line == "polar" || line == "polar_deg" || line == "format=polar")
                    {
                        if (inA) { aFmt = NumFormat::PolarDeg; aFmtForced = true; }
                        else     { cFmt = NumFormat::PolarDeg; cFmtForced = true; }
                        continue;
                    }
                    if (line == "polar_rad" || line == "rad")
                    {
                        if (inA) { aFmt = NumFormat::PolarRad; aFmtForced = true; }
                        else     { cFmt = NumFormat::PolarRad; cFmtForced = true; }
                        continue;
                    }
                    if (line == "rect" || line == "cart" || line == "complejo")
                    {
                        if (inA) { aFmt = NumFormat::Rect; aFmtForced = true; }
                        else     { cFmt = NumFormat::Rect; cFmtForced = true; }
                        continue;
                    }
                }

                // Parseo de datos
                if (inA)
                {
                    std::vector<double> buf;
                    parseDoublesLine(line, buf);
                    if ((int)buf.size() > 0)
                    {
                        rowsA.push_back(buf);
                    }
                }
                else if (inB)
                {
                    rowsB.push_back(line);
                }
                else if (inC)
                {
                    std::vector<double> buf;
                    parseDoublesLine(line, buf);
                    if ((int)buf.size() > 0)
                    {
                        rowsC.push_back(buf);
                    }
                }
                else
                {
                    // si no hay bloque activo, lo tratamos como A
                    std::vector<double> buf;
                    parseDoublesLine(line, buf);
                    if ((int)buf.size() > 0)
                    {
                        rowsA.push_back(buf);
                    }
                }
            }
        }
    }

    // ---------- Validaciones y dimensiones ----------
    int m = (int)rowsA.size();
    if (m == 0) return false;

    int doublesPerRow = (int)rowsA[0].size();
    if (doublesPerRow % 2 != 0) return false;

    int i = 1;
    bool same = true;
    while (i < m && same)
    {
        if ((int)rowsA[i].size() != doublesPerRow) same = false;
        ++i;
    }
    if (!same) return false;

    int n = doublesPerRow / 2;

    // ---------- Detección de formato (si no hubo línea de control) ----------
    if (!aFmtForced) aFmt = detectFmtPairs(rowsA);
    if (!cFmtForced) cFmt = detectFmtPairs(rowsC);

    // ---------- Construcción de A ----------
    A = MatrizComplejo(m, n);
    int r = 0;
    while (r < m)
    {
        int j = 0;
        while (j < n)
        {
            int idx = 2 * j;
            double a = rowsA[r][idx];
            double bval = rowsA[r][idx + 1];
            complejo z = (aFmt == NumFormat::Rect)
                             ? makeC(a, bval)
                             : (aFmt == NumFormat::PolarDeg)
                                   ? makeFromPolar(a, bval, true)
                                   : makeFromPolar(a, bval, false);
            A.set(r, j, z);
            ++j;
        }
        ++r;
    }

    // ---------- Construcción de b ----------
    b = VectorIncognitas(n);
    if ((int)rowsB.size() == n)
    {
        int k = 0;
        while (k < n)
        {
            b.set(k, rowsB[k]);
            ++k;
        }
    }
    else
    {
        int k = 0;
        while (k < n)
        {
            std::ostringstream oss;
            oss << "i" << (k + 1);
            b.set(k, oss.str());
            ++k;
        }
    }

    // ---------- Construcción de c ----------
    c = VectorComplejo(m);
    if ((int)rowsC.size() == m)
    {
        int t = 0;
        bool okpairs = true;
        while (t < m && okpairs)
        {
            if ((int)rowsC[t].size() == 2)
            {
                double a = rowsC[t][0];
                double bval = rowsC[t][1];
                complejo z = (cFmt == NumFormat::Rect)
                                 ? makeC(a, bval)
                                 : (cFmt == NumFormat::PolarDeg)
                                       ? makeFromPolar(a, bval, true)
                                       : makeFromPolar(a, bval, false);
                c.vectoraddback(z);
            }
            else
            {
                okpairs = false;
            }
            ++t;
        }
        if (!okpairs) return false;
    }
    else
    {
        int t = 0;
        while (t < m) { c.vectoraddback(makeC(0.0, 0.0)); ++t; }
    }

    return true;
}

// ======================================================
// BIN (auto-detección por heurística; no rompe legacy)
// Formato legacy (admitido):
//  [int32 rows][int32 cols]
//  rows*cols * (double re, double im)
//  [int32 n] == cols
//  n * ([int32 len] [len bytes etiqueta])
//  [int32 m] == rows
//  m * (double re, double im)
// Usamos heurística sobre los pares para decidir si son Rect o Polar y convertimos.
// ======================================================
bool loadABCLabelsFromBin(const std::string &path, MatrizComplejo &A, VectorIncognitas &b, VectorComplejo &c)
{
    std::ifstream fb(path, std::ios::binary);
    if (!fb.good())
    {
        return false;
    }

    int32_t rows = 0, cols = 0;
    fb.read(reinterpret_cast<char *>(&rows), sizeof(rows));
    fb.read(reinterpret_cast<char *>(&cols), sizeof(cols));
    if (!fb.good() || rows <= 0 || cols <= 0)
    {
        return false;
    }

    // ---------- Leer A como pares crudos (a,b) sin asumir formato ----------
    const int totalPairsA = rows * cols;
    std::vector<double> rawA;
    rawA.reserve((size_t)totalPairsA * 2);

    int p = 0;
    bool cont = true;
    while (cont)
    {
        if (p < totalPairsA)
        {
            double a = 0.0, bval = 0.0;
            fb.read(reinterpret_cast<char *>(&a), sizeof(a));
            if (fb.good())
            {
                fb.read(reinterpret_cast<char *>(&bval), sizeof(bval));
                if (fb.good())
                {
                    rawA.push_back(a);
                    rawA.push_back(bval);
                    ++p;
                }
                else
                {
                    cont = false;
                }
            }
            else
            {
                cont = false;
            }
        }
        else
        {
            cont = false;
        }
    }
    if (p != totalPairsA)
    {
        return false;
    }

    // Armar vista "rowsA" para heurística (cada fila: a b a b ...)
    std::vector<std::vector<double>> rowsA(rows, std::vector<double>((size_t)cols * 2));
    for (int r = 0; r < rows; ++r)
    {
        for (int j = 0; j < cols; ++j)
        {
            int baseRaw = (r * cols + j) * 2;
            rowsA[r][2 * j + 0] = rawA[baseRaw + 0];
            rowsA[r][2 * j + 1] = rawA[baseRaw + 1];
        }
    }

    // Detectar formato y construir A
    NumFormat aFmt = detectFmtPairs(rowsA);
    A = MatrizComplejo(rows, cols);
    for (int r = 0; r < rows; ++r)
    {
        for (int j = 0; j < cols; ++j)
        {
            int baseRaw = (r * cols + j) * 2;
            double a = rawA[baseRaw + 0];
            double bval = rawA[baseRaw + 1];
            complejo z = (aFmt == NumFormat::Rect)
                             ? makeC(a, bval)
                             : (aFmt == NumFormat::PolarDeg)
                                   ? makeFromPolar(a, bval, true)
                                   : makeFromPolar(a, bval, false);
            A.set(r, j, z);
        }
    }

    // ---------- b ----------
    int32_t n = 0;
    fb.read(reinterpret_cast<char *>(&n), sizeof(n));
    if (!fb.good() || n != cols)
    {
        return false;
    }

    b = VectorIncognitas(n);
    int i = 0;
    bool ok = true;
    while (i < n && ok)
    {
        int32_t len = 0;
        fb.read(reinterpret_cast<char *>(&len), sizeof(len));
        if (fb.good() && len >= 0)
        {
            std::string s;
            s.resize((size_t)len);
            if (len > 0)
            {
                fb.read(reinterpret_cast<char *>(&s[0]), (std::streamsize)len);
                if (!fb.good())
                {
                    ok = false;
                }
            }
            if (ok)
            {
                b.set(i, s);
            }
        }
        else
        {
            ok = false;
        }
        ++i;
    }
    if (!ok)
    {
        return false;
    }

    // ---------- c ----------
    int32_t mvec = 0;
    fb.read(reinterpret_cast<char *>(&mvec), sizeof(mvec));
    if (!fb.good() || mvec != rows)
    {
        return false;
    }

    // Leer pares crudos de c
    std::vector<double> rawC;
    rawC.reserve((size_t)mvec * 2);
    int t = 0;
    bool ok2 = true;
    while (t < mvec && ok2)
    {
        double a = 0.0, bval = 0.0;
        fb.read(reinterpret_cast<char *>(&a), sizeof(a));
        if (fb.good())
        {
            fb.read(reinterpret_cast<char *>(&bval), sizeof(bval));
            if (fb.good())
            {
                rawC.push_back(a);
                rawC.push_back(bval);
            }
            else
            {
                ok2 = false;
            }
        }
        else
        {
            ok2 = false;
        }
        ++t;
    }
    if (!ok2)
    {
        return false;
    }

    // Armar vista rowsC para heurística y construir c
    std::vector<std::vector<double>> rowsC(rows, std::vector<double>(2));
    for (int r = 0; r < rows; ++r)
    {
        rowsC[r][0] = rawC[2 * r + 0];
        rowsC[r][1] = rawC[2 * r + 1];
    }
    NumFormat cFmt = detectFmtPairs(rowsC);

    c = VectorComplejo(rows);
    for (int r = 0; r < rows; ++r)
    {
        double a = rowsC[r][0];
        double bval = rowsC[r][1];
        complejo z = (cFmt == NumFormat::Rect)
                         ? makeC(a, bval)
                         : (cFmt == NumFormat::PolarDeg)
                               ? makeFromPolar(a, bval, true)
                               : makeFromPolar(a, bval, false);
        c.vectoraddback(z);
    }

    return true;
}

// ======================================================
// Guardado (igual que tenías): siempre rectangular (re,im)
// ======================================================
bool saveABCLabelsToTxt(const std::string &path, const MatrizComplejo &A, const VectorIncognitas &b, const VectorComplejo &c)
{
    std::ofstream fo(path, std::ios::trunc);
    if (!fo.good())
    {
        return false;
    }

    // A:
    fo << "A:\n";
    int rows = A.getRows(), cols = A.getCols();
    int r = 0;
    while (r < rows)
    {
        int j = 0;
        while (j < cols)
        {
            complejo z = A.get(r, j);
            complejo zc = z; // copia mutable (getters no const)
            fo << zc.GetReal() << " " << zc.GetImag();
            if (j + 1 < cols)
            {
                fo << "   ";
            }
            ++j;
        }
        fo << "\n";
        ++r;
    }

    // b:
    fo << "b:\n";
    int i = 0;
    while (i < b.size())
    {
        fo << b.get(i) << "\n";
        ++i;
    }

    // c:
    fo << "c:\n";
    int t = 0;
    while (t < c.vectorgetsize())
    {
        complejo z = c.vectorget(t);
        complejo zc = z;
        fo << zc.GetReal() << " " << zc.GetImag() << "\n";
        ++t;
    }

    return fo.good();
}

bool saveABCLabelsToBin(const std::string &path, const MatrizComplejo &A, const VectorIncognitas &b, const VectorComplejo &c)
{
    std::ofstream fb(path, std::ios::binary | std::ios::trunc);
    if (!fb.good())
    {
        return false;
    }

    int32_t rows = (int32_t)A.getRows();
    int32_t cols = (int32_t)A.getCols();
    if (!(b.size() == cols && c.vectorgetsize() == rows))
    {
        return false;
    }

    // header A
    fb.write(reinterpret_cast<const char *>(&rows), sizeof(rows));
    fb.write(reinterpret_cast<const char *>(&cols), sizeof(cols));

    // A (re,im)
    int r = 0;
    while (r < rows)
    {
        int j = 0;
        while (j < cols)
        {
            complejo z = A.get(r, j);
            complejo zc = z;
            double re = zc.GetReal(), im = zc.GetImag();
            fb.write(reinterpret_cast<const char *>(&re), sizeof(re));
            if (fb.good())
            {
                fb.write(reinterpret_cast<const char *>(&im), sizeof(im));
            }
            ++j;
        }
        ++r;
    }

    // b: n + strings (len + bytes)
    int32_t n = cols;
    fb.write(reinterpret_cast<const char *>(&n), sizeof(n));
    int i = 0;
    bool ok = fb.good();
    while (i < n && ok)
    {
        const std::string s = b.get(i);
        int32_t len = (int32_t)s.size();
        fb.write(reinterpret_cast<const char *>(&len), sizeof(len));
        if (fb.good())
        {
            if (len > 0)
            {
                fb.write(s.data(), len);
                if (!fb.good())
                {
                    ok = false;
                }
            }
        }
        else
        {
            ok = false;
        }
        ++i;
    }
    if (!ok)
    {
        return false;
    }

    // c: m + (re,im)
    int32_t mvec = rows;
    fb.write(reinterpret_cast<const char *>(&mvec), sizeof(mvec));
    int t = 0;
    bool ok2 = fb.good();
    while (t < mvec && ok2)
    {
        complejo z = c.vectorget(t);
        complejo zc = z;
        double re = zc.GetReal(), im = zc.GetImag();
        fb.write(reinterpret_cast<const char *>(&re), sizeof(re));
        if (fb.good())
        {
            fb.write(reinterpret_cast<const char *>(&im), sizeof(im));
            if (!fb.good())
            {
                ok2 = false;
            }
        }
        else
        {
            ok2 = false;
        }
        ++t;
    }
    if (!ok2)
    {
        return false;
    }

    return true;
}


//--------------------------------------------------------------Funciones para devolver en forma rectangular--------------------------------------------------------------

/*
#include "SistEcuaciones.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include "complejos.h"

// ---------------- helpers (sin break/return en bucles) ----------------
static bool isBlank(const std::string &s)
{
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r'))
    {
        ++i;
    }
    return i == s.size();
}
static bool parseDoublesLine(const std::string &text, std::vector<double> &out)
{
    std::istringstream iss(text);
    double x = 0.0;
    bool any = false;
    bool cont = true;
    while (cont)
    {
        if (iss >> x)
        {
            out.push_back(x);
            any = true;
        }
        else
        {
            cont = false;
        }
    }
    return any;
}
static complejo makeC(double re, double im)
{
    complejo z(re, im);
    return z;
}

// ---------------- TXT ----------------
bool loadABCLabelsFromTxt(const std::string &path, MatrizComplejo &A, VectorIncognitas &b, VectorComplejo &c)
{
    std::ifstream fi(path);
    if (!fi.good())
    {
        return false;
    }

    std::vector<std::vector<double>> rowsA; // cada fila: ... re im re im ...
    std::vector<std::string> rowsB;         // cada fila: UNA etiqueta
    std::vector<std::vector<double>> rowsC; // cada fila: re im

    bool inA = false, inB = false, inC = false;

    std::string line;
    bool reading = true;
    while (reading)
    {
        if (!std::getline(fi, line))
        {
            reading = false;
        }
        else
        {
            if (isBlank(line))
            {
                // nada
            }
            else if (line == "A:" || line == "a:" || line == "A" || line == "a")
            {
                inA = true;
                inB = false;
                inC = false;
            }
            else if (line == "b:" || line == "B:" || line == "b" || line == "B")
            {
                inA = false;
                inB = true;
                inC = false;
            }
            else if (line == "c:" || line == "C:" || line == "c" || line == "C")
            {
                inA = false;
                inB = false;
                inC = true;
            }
            else
            {
                if (inA)
                {
                    std::vector<double> buf;
                    parseDoublesLine(line, buf);
                    if (static_cast<int>(buf.size()) > 0)
                    {
                        rowsA.push_back(buf);
                    }
                }
                else if (inB)
                {
                    // Una etiqueta por línea
                    rowsB.push_back(line);
                }
                else if (inC)
                {
                    std::vector<double> buf;
                    parseDoublesLine(line, buf);
                    if (static_cast<int>(buf.size()) > 0)
                    {
                        rowsC.push_back(buf);
                    }
                }
                else
                {
                    // si no hay bloque activo, lo tratamos como A
                    std::vector<double> buf;
                    parseDoublesLine(line, buf);
                    if (static_cast<int>(buf.size()) > 0)
                    {
                        rowsA.push_back(buf);
                    }
                }
            }
        }
    }

    int m = static_cast<int>(rowsA.size());
    if (m == 0)
    {
        return false;
    }

    int doublesPerRow = static_cast<int>(rowsA[0].size());
    if (doublesPerRow % 2 != 0)
    {
        return false;
    }

    int i = 1;
    bool same = true;
    while (i < m && same)
    {
        if (static_cast<int>(rowsA[i].size()) != doublesPerRow)
        {
            same = false;
        }
        ++i;
    }
    if (!same)
    {
        return false;
    }

    int n = doublesPerRow / 2;

    // Construir A (m x n)
    A = MatrizComplejo(m, n);
    int r = 0;
    while (r < m)
    {
        int j = 0;
        while (j < n)
        {
            int idx = 2 * j;
            complejo z = makeC(rowsA[r][idx], rowsA[r][idx + 1]);
            A.set(r, j, z);
            ++j;
        }
        ++r;
    }

    // Construir b (n etiquetas). Si rowsB vacío, autogenero i1..in
    b = VectorIncognitas(n);
    if (static_cast<int>(rowsB.size()) == n)
    {
        int k = 0;
        while (k < n)
        {
            b.set(k, rowsB[k]);
            ++k;
        }
    }
    else
    {
        int k = 0;
        while (k < n)
        {
            std::ostringstream oss;
            oss << "i" << (k + 1);
            b.set(k, oss.str());
            ++k;
        }
    }

    // Construir c (m complejos)
    c = VectorComplejo(m);
    if (static_cast<int>(rowsC.size()) == m)
    {
        int t = 0;
        bool okpairs = true;
        while (t < m && okpairs)
        {
            if (static_cast<int>(rowsC[t].size()) == 2)
            {
                c.vectoraddback(makeC(rowsC[t][0], rowsC[t][1]));
            }
            else
            {
                okpairs = false;
            }
            ++t;
        }
        if (!okpairs)
        {
            return false;
        }
    }
    else
    {
        // si no vino c, lo llenamos con 0+0j
        int t = 0;
        while (t < m)
        {
            c.vectoraddback(makeC(0.0, 0.0));
            ++t;
        }
    }

    return true;
}

// ---------------- BIN ----------------
bool loadABCLabelsFromBin(const std::string &path, MatrizComplejo &A, VectorIncognitas &b, VectorComplejo &c)
{
    std::ifstream fb(path, std::ios::binary);
    if (!fb.good())
    {
        return false;
    }

    int32_t rows = 0, cols = 0;
    fb.read(reinterpret_cast<char *>(&rows), sizeof(rows));
    fb.read(reinterpret_cast<char *>(&cols), sizeof(cols));
    if (!fb.good() || rows <= 0 || cols <= 0)
    {
        return false;
    }

    // A: rows*cols pares (re,im)
    std::vector<double> raw;
    raw.reserve(static_cast<size_t>(rows) * static_cast<size_t>(cols) * 2);

    int totalPairs = rows * cols;
    int p = 0;
    bool cont = true;
    while (cont)
    {
        if (p < totalPairs)
        {
            double re = 0.0, im = 0.0;
            fb.read(reinterpret_cast<char *>(&re), sizeof(re));
            if (fb.good())
            {
                fb.read(reinterpret_cast<char *>(&im), sizeof(im));
                if (fb.good())
                {
                    raw.push_back(re);
                    raw.push_back(im);
                    ++p;
                }
                else
                {
                    cont = false;
                }
            }
            else
            {
                cont = false;
            }
        }
        else
        {
            cont = false;
        }
    }
    if (p != totalPairs)
    {
        return false;
    }

    // Construir A
    A = MatrizComplejo(rows, cols);
    int r = 0;
    while (r < rows)
    {
        int j = 0;
        while (j < cols)
        {
            int base = (r * cols + j) * 2;
            A.set(r, j, makeC(raw[base], raw[base + 1]));
            ++j;
        }
        ++r;
    }

    // b: [int32 n] + (n strings)
    int32_t n = 0;
    fb.read(reinterpret_cast<char *>(&n), sizeof(n));
    if (!fb.good() || n != cols)
    {
        return false;
    }

    b = VectorIncognitas(n);
    int i = 0;
    bool ok = true;
    while (i < n && ok)
    {
        int32_t len = 0;
        fb.read(reinterpret_cast<char *>(&len), sizeof(len));
        if (fb.good() && len >= 0)
        {
            std::string s;
            s.resize(static_cast<size_t>(len));
            if (len > 0)
            {
                fb.read(reinterpret_cast<char *>(&s[0]), static_cast<std::streamsize>(len));
                if (!fb.good())
                {
                    ok = false;
                }
            }
            if (ok)
            {
                b.set(i, s);
            }
        }
        else
        {
            ok = false;
        }
        ++i;
    }
    if (!ok)
    {
        return false;
    }

    // c: [int32 m] + m pares (re,im)
    int32_t mvec = 0;
    fb.read(reinterpret_cast<char *>(&mvec), sizeof(mvec));
    if (!fb.good() || mvec != rows)
    {
        return false;
    }

    c = VectorComplejo(rows);
    int t = 0;
    bool ok2 = true;
    while (t < mvec && ok2)
    {
        double re = 0.0, im = 0.0;
        fb.read(reinterpret_cast<char *>(&re), sizeof(re));
        if (fb.good())
        {
            fb.read(reinterpret_cast<char *>(&im), sizeof(im));
            if (fb.good())
            {
                c.vectoraddback(makeC(re, im));
            }
            else
            {
                ok2 = false;
            }
        }
        else
        {
            ok2 = false;
        }
        ++t;
    }
    if (!ok2)
    {
        return false;
    }

    return true;
}

// ---------------- Guardado ----------------
bool saveABCLabelsToTxt(const std::string &path, const MatrizComplejo &A, const VectorIncognitas &b, const VectorComplejo &c)
{
    std::ofstream fo(path, std::ios::trunc);
    if (!fo.good())
    {
        return false;
    }

    // A:
    fo << "A:\n";
    int rows = A.getRows(), cols = A.getCols();
    int r = 0;
    while (r < rows)
    {
        int j = 0;
        while (j < cols)
        {
            complejo z = A.get(r, j);
            complejo zc = z; // copia mutable (getters no const)
            fo << zc.GetReal() << " " << zc.GetImag();
            if (j + 1 < cols)
            {
                fo << "   ";
            }
            ++j;
        }
        fo << "\n";
        ++r;
    }

    // b:
    fo << "b:\n";
    int i = 0;
    while (i < b.size())
    {
        fo << b.get(i) << "\n";
        ++i;
    }

    // c:
    fo << "c:\n";
    int t = 0;
    while (t < c.vectorgetsize())
    {
        complejo z = c.vectorget(t);
        complejo zc = z;
        fo << zc.GetReal() << " " << zc.GetImag() << "\n";
        ++t;
    }

    return fo.good();
}

bool saveABCLabelsToBin(const std::string &path, const MatrizComplejo &A, const VectorIncognitas &b, const VectorComplejo &c)
{
    std::ofstream fb(path, std::ios::binary | std::ios::trunc);
    if (!fb.good())
    {
        return false;
    }

    int32_t rows = static_cast<int32_t>(A.getRows());
    int32_t cols = static_cast<int32_t>(A.getCols());
    if (!(b.size() == cols && c.vectorgetsize() == rows))
    {
        return false;
    }

    // header A
    fb.write(reinterpret_cast<const char *>(&rows), sizeof(rows));
    fb.write(reinterpret_cast<const char *>(&cols), sizeof(cols));

    // A (re,im)
    int r = 0;
    while (r < rows)
    {
        int j = 0;
        while (j < cols)
        {
            complejo z = A.get(r, j);
            complejo zc = z;
            double re = zc.GetReal(), im = zc.GetImag();
            fb.write(reinterpret_cast<const char *>(&re), sizeof(re));
            if (fb.good())
            {
                fb.write(reinterpret_cast<const char *>(&im), sizeof(im));
            }
            ++j;
        }
        ++r;
    }

    // b: n + strings (len + bytes)
    int32_t n = cols;
    fb.write(reinterpret_cast<const char *>(&n), sizeof(n));
    int i = 0;
    bool ok = fb.good();
    while (i < n && ok)
    {
        const std::string s = b.get(i);
        int32_t len = static_cast<int32_t>(s.size());
        fb.write(reinterpret_cast<const char *>(&len), sizeof(len));
        if (fb.good())
        {
            if (len > 0)
            {
                fb.write(s.data(), len);
                if (!fb.good())
                {
                    ok = false;
                }
            }
        }
        else
        {
            ok = false;
        }
        ++i;
    }
    if (!ok)
    {
        return false;
    }

    // c: m + (re,im)
    int32_t mvec = rows;
    fb.write(reinterpret_cast<const char *>(&mvec), sizeof(mvec));
    int t = 0;
    bool ok2 = fb.good();
    while (t < mvec && ok2)
    {
        complejo z = c.vectorget(t);
        complejo zc = z;
        double re = zc.GetReal(), im = zc.GetImag();
        fb.write(reinterpret_cast<const char *>(&re), sizeof(re));
        if (fb.good())
        {
            fb.write(reinterpret_cast<const char *>(&im), sizeof(im));
            if (!fb.good())
            {
                ok2 = false;
            }
        }
        else
        {
            ok2 = false;
        }
        ++t;
    }
    if (!ok2)
    {
        return false;
    }

    return true;
}
*/