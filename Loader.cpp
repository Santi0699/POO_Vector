#include "Loader.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include "complejos.h"
#include "FasorConv.h"

// =================== Helpers comunes ===================
static bool isBlank(const std::string &s)
{
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r'))
        ++i;
    return i == s.size();
}
static std::string trim(const std::string &x)
{
    size_t a = 0, b = x.size();
    while (a < b && (x[a] == ' ' || x[a] == '\t'))
        ++a;
    while (b > a && (x[b - 1] == ' ' || x[b - 1] == '\t' || x[b - 1] == '\r'))
        --b;
    return x.substr(a, b - a);
}
static int countPipes(const std::string &s)
{
    int cnt = 0;
    size_t i = 0;
    while (i < s.size())
    {
        if (s[i] == '|')
            ++cnt;
        ++i;
    }
    return cnt;
}
static bool parseDoublesLine(const std::string &txt, std::vector<double> &out)
{
    std::istringstream iss(txt);
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
            cont = false;
    }
    return any;
}
static complejo makeC(double re, double im) { return complejo(re, im); }

// NUEVO: interpreta el par (a,b) según el orden elegido para Rectangular
static inline complejo fromRectPair(double a, double b, PairOrder order)
{
    if (order == PairOrder::ReIm)
        return makeC(a, b); // a=re, b=im
    return makeC(b, a);     // a=im, b=re
}

// ================================================================
// ===============   CARGA INTEGRADA (1 archivo)   =================
// ================================================================

// --------- TXT Rect (con orden configurable) ---------
bool loadFromTxtRect(const std::string &path,
                     MatrizComplejo &A,
                     VectorIncognitas &b,
                     VectorComplejo &c,
                     PairOrder order)
{
    std::ifstream fi(path);
    if (!fi.good())
        return false;

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fi, line))
        if (!isBlank(line))
            lines.push_back(line);
    if (lines.empty())
        return false;

    // ¿formato por filas con pipes?
    int withTwo = 0;
    int N = (int)lines.size();
    int ii = 0;
    while (ii < N)
    {
        if (countPipes(lines[ii]) == 2)
            ++withTwo;
        ++ii;
    }
    bool rowMode = (withTwo == N);

    if (rowMode)
    {
        // "re im re im ... | etiqueta | re im"
        std::vector<std::vector<double>> rowsA, rowsC;
        std::vector<std::string> labels;

        int r = 0;
        bool ok = true;
        while (r < N && ok)
        {
            const std::string &L = lines[r];
            size_t p1 = L.find('|');
            size_t p2 = (p1 != std::string::npos ? L.find('|', p1 + 1) : std::string::npos);
            if (p1 == std::string::npos || p2 == std::string::npos)
                ok = false;
            else
            {
                std::string left = trim(L.substr(0, p1));
                std::string mid = trim(L.substr(p1 + 1, p2 - p1 - 1));
                std::string right = trim(L.substr(p2 + 1));

                std::vector<double> bufL, bufR;
                parseDoublesLine(left, bufL);
                parseDoublesLine(right, bufR);
                if (((int)bufL.size() % 2) != 0 || (int)bufR.size() != 2)
                    ok = false;
                else
                {
                    rowsA.push_back(bufL);
                    labels.push_back(mid);
                    rowsC.push_back(bufR);
                }
            }
            ++r;
        }
        if (!ok)
            return false;

        int m = (int)rowsA.size();
        int n = (int)rowsA[0].size() / 2;
        A = MatrizComplejo(m, n);
        int i = 0;
        while (i < m)
        {
            int j = 0;
            while (j < n)
            {
                int idx = 2 * j;
                A.set(i, j, fromRectPair(rowsA[i][idx], rowsA[i][idx + 1], order));
                ++j;
            }
            ++i;
        }

        b = VectorIncognitas(n);
        int k = 0;
        while (k < n)
        {
            b.set(k, (m == n ? labels[k] : std::string("i") + std::to_string(k + 1)));
            ++k;
        }

        c = VectorComplejo(m);
        int t = 0;
        while (t < m)
        {
            c.vectoraddback(fromRectPair(rowsC[t][0], rowsC[t][1], order));
            ++t;
        }
        return true;
    }

    // Formato por bloques A:/b:/c:
    std::vector<std::vector<double>> rowsA, rowsC;
    std::vector<std::string> rowsB;
    bool inA = false, inB = false, inC = false;
    int i2 = 0;
    while (i2 < N)
    {
        const std::string &l = lines[i2];
        if (l == "A:" || l == "a:" || l == "A" || l == "a")
        {
            inA = true;
            inB = false;
            inC = false;
        }
        else if (l == "b:" || l == "B:" || l == "b" || l == "B")
        {
            inA = false;
            inB = true;
            inC = false;
        }
        else if (l == "c:" || l == "C:" || l == "c" || l == "C")
        {
            inA = false;
            inB = false;
            inC = true;
        }
        else
        {
            if (inA || (!inB && !inC))
            {
                std::vector<double> buf;
                if (parseDoublesLine(l, buf))
                    rowsA.push_back(buf);
            }
            else if (inB)
            {
                rowsB.push_back(trim(l));
            }
            else if (inC)
            {
                std::vector<double> buf;
                if (parseDoublesLine(l, buf))
                    rowsC.push_back(buf);
            }
        }
        ++i2;
    }
    int m = (int)rowsA.size();
    if (m <= 0)
        return false;
    int dpr = (int)rowsA[0].size();
    if ((dpr % 2) != 0)
        return false;
    int n = dpr / 2;

    A = MatrizComplejo(m, n);
    int r = 0;
    while (r < m)
    {
        int j = 0;
        while (j < n)
        {
            int idx = 2 * j;
            A.set(r, j, fromRectPair(rowsA[r][idx], rowsA[r][idx + 1], order));
            ++j;
        }
        ++r;
    }

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
            b.set(k, std::string("i") + std::to_string(k + 1));
            ++k;
        }
    }

    c = VectorComplejo(m);
    if ((int)rowsC.size() == m)
    {
        int t = 0;
        bool ok = true;
        while (t < m && ok)
        {
            if ((int)rowsC[t].size() == 2)
                c.vectoraddback(fromRectPair(rowsC[t][0], rowsC[t][1], order));
            else
                ok = false;
            ++t;
        }
        if (!ok)
            return false;
    }
    else
    {
        int t = 0;
        while (t < m)
        {
            c.vectoraddback(makeC(0, 0));
            ++t;
        }
    }
    return true;
}

// --------- TXT Fasor ---------
bool loadFromTxtFasor(const std::string &path,
                      MatrizComplejo &A,
                      VectorIncognitas &b,
                      VectorComplejo &c)
{
    std::ifstream fi(path);
    if (!fi.good())
        return false;

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fi, line))
        if (!isBlank(line))
            lines.push_back(line);
    if (lines.empty())
        return false;

    int withTwo = 0;
    int N = (int)lines.size();
    int ii = 0;
    while (ii < N)
    {
        if (countPipes(lines[ii]) == 2)
            ++withTwo;
        ++ii;
    }
    bool rowMode = (withTwo == N);

    if (rowMode)
    {
        // "mod ang ... | etiqueta | mod ang"
        std::vector<std::vector<double>> rowsA, rowsC;
        std::vector<std::string> labels;

        int r = 0;
        bool ok = true;
        while (r < N && ok)
        {
            const std::string &L = lines[r];
            size_t p1 = L.find('|');
            size_t p2 = (p1 != std::string::npos ? L.find('|', p1 + 1) : std::string::npos);
            if (p1 == std::string::npos || p2 == std::string::npos)
                ok = false;
            else
            {
                std::string left = trim(L.substr(0, p1));
                std::string mid = trim(L.substr(p1 + 1, p2 - p1 - 1));
                std::string right = trim(L.substr(p2 + 1));

                std::vector<double> bufL, bufR;
                parseDoublesLine(left, bufL);
                parseDoublesLine(right, bufR);
                if (((int)bufL.size() % 2) != 0 || (int)bufR.size() != 2)
                    ok = false;
                else
                {
                    rowsA.push_back(bufL);
                    labels.push_back(mid);
                    rowsC.push_back(bufR);
                }
            }
            ++r;
        }
        if (!ok)
            return false;

        int m = (int)rowsA.size();
        int n = (int)rowsA[0].size() / 2;
        A = MatrizComplejo(m, n);
        int i = 0;
        while (i < m)
        {
            int j = 0;
            while (j < n)
            {
                int idx = 2 * j;
                A.set(i, j, fasorToComplejo(rowsA[i][idx], rowsA[i][idx + 1]));
                ++j;
            }
            ++i;
        }

        b = VectorIncognitas(n);
        int k = 0;
        while (k < n)
        {
            b.set(k, (m == n ? labels[k] : std::string("i") + std::to_string(k + 1)));
            ++k;
        }

        c = VectorComplejo(m);
        int t = 0;
        while (t < m)
        {
            c.vectoraddback(fasorToComplejo(rowsC[t][0], rowsC[t][1]));
            ++t;
        }
        return true;
    }

    // bloques
    std::vector<std::vector<double>> rowsA, rowsC;
    std::vector<std::string> rowsB;
    bool inA = false, inB = false, inC = false;
    int i2 = 0;
    while (i2 < N)
    {
        const std::string &l = lines[i2];
        if (l == "A:" || l == "a:" || l == "A" || l == "a")
        {
            inA = true;
            inB = false;
            inC = false;
        }
        else if (l == "b:" || l == "B:" || l == "b" || l == "B")
        {
            inA = false;
            inB = true;
            inC = false;
        }
        else if (l == "c:" || l == "C:" || l == "c" || l == "C")
        {
            inA = false;
            inB = false;
            inC = true;
        }
        else
        {
            if (inA || (!inB && !inC))
            {
                std::vector<double> buf;
                if (parseDoublesLine(l, buf))
                    rowsA.push_back(buf);
            }
            else if (inB)
            {
                rowsB.push_back(trim(l));
            }
            else if (inC)
            {
                std::vector<double> buf;
                if (parseDoublesLine(l, buf))
                    rowsC.push_back(buf);
            }
        }
        ++i2;
    }
    int m = (int)rowsA.size();
    if (m <= 0)
        return false;
    int dpr = (int)rowsA[0].size();
    if ((dpr % 2) != 0)
        return false;
    int n = dpr / 2;

    A = MatrizComplejo(m, n);
    int r = 0;
    while (r < m)
    {
        int j = 0;
        while (j < n)
        {
            int idx = 2 * j;
            A.set(r, j, fasorToComplejo(rowsA[r][idx], rowsA[r][idx + 1]));
            ++j;
        }
        ++r;
    }

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
            b.set(k, std::string("i") + std::to_string(k + 1));
            ++k;
        }
    }

    c = VectorComplejo(m);
    if ((int)rowsC.size() == m)
    {
        int t = 0;
        bool ok = true;
        while (t < m && ok)
        {
            if ((int)rowsC[t].size() == 2)
                c.vectoraddback(fasorToComplejo(rowsC[t][0], rowsC[t][1]));
            else
                ok = false;
            ++t;
        }
        if (!ok)
            return false;
    }
    else
    {
        int t = 0;
        while (t < m)
        {
            c.vectoraddback(makeC(0, 0));
            ++t;
        }
    }
    return true;
}

// --------- BIN Rect (con orden configurable) ---------
bool loadFromBinRect(const std::string &path,
                     MatrizComplejo &A,
                     VectorIncognitas &b,
                     VectorComplejo &c,
                     PairOrder order)
{
    std::ifstream fb(path, std::ios::binary);
    if (!fb.good())
        return false;

    int32_t rows = 0, cols = 0;
    fb.read((char *)&rows, sizeof(rows));
    fb.read((char *)&cols, sizeof(cols));
    if (rows <= 0 || cols <= 0 || !fb.good())
        return false;

    A = MatrizComplejo(rows, cols);
    int r = 0;
    bool ok = true;
    while (r < rows && ok)
    {
        int j = 0;
        while (j < cols && ok)
        {
            double a = 0.0, bval = 0.0;
            fb.read((char *)&a, sizeof(a));
            fb.read((char *)&bval, sizeof(bval));
            if (fb.good())
                A.set(r, j, fromRectPair(a, bval, order));
            else
                ok = false;
            ++j;
        }
        ++r;
    }
    if (!ok)
        return false;

    int32_t n = 0;
    fb.read((char *)&n, sizeof(n));
    if (n != cols || !fb.good())
        return false;
    b = VectorIncognitas(n);
    int i = 0;
    ok = true;
    while (i < n && ok)
    {
        int32_t len = 0;
        fb.read((char *)&len, sizeof(len));
        if (fb.good() && len >= 0)
        {
            std::string s;
            s.resize((size_t)len);
            if (len > 0)
            {
                fb.read(&s[0], len);
                if (!fb.good())
                    ok = false;
            }
            if (ok)
                b.set(i, s);
        }
        else
            ok = false;
        ++i;
    }
    if (!ok)
        return false;

    int32_t mvec = 0;
    fb.read((char *)&mvec, sizeof(mvec));
    if (mvec != rows || !fb.good())
        return false;
    c = VectorComplejo(rows);
    int t = 0;
    ok = true;
    while (t < mvec && ok)
    {
        double a = 0.0, bval = 0.0;
        fb.read((char *)&a, sizeof(a));
        fb.read((char *)&bval, sizeof(bval));
        if (fb.good())
            c.vectoraddback(fromRectPair(a, bval, order));
        else
            ok = false;
        ++t;
    }
    return ok && fb.good();
}

// --------- BIN Fasor ---------
bool loadFromBinFasor(const std::string &path,
                      MatrizComplejo &A,
                      VectorIncognitas &b,
                      VectorComplejo &c)
{
    std::ifstream fb(path, std::ios::binary);
    if (!fb.good())
        return false;

    int32_t rows = 0, cols = 0;
    fb.read((char *)&rows, sizeof(rows));
    fb.read((char *)&cols, sizeof(cols));
    if (rows <= 0 || cols <= 0 || !fb.good())
        return false;

    A = MatrizComplejo(rows, cols);
    int r = 0;
    bool ok = true;
    while (r < rows && ok)
    {
        int j = 0;
        while (j < cols && ok)
        {
            double mod = 0.0, ang = 0.0;
            fb.read((char *)&mod, sizeof(mod));
            fb.read((char *)&ang, sizeof(ang));
            if (fb.good())
                A.set(r, j, fasorToComplejo(mod, ang));
            else
                ok = false;
            ++j;
        }
        ++r;
    }
    if (!ok)
        return false;

    int32_t n = 0;
    fb.read((char *)&n, sizeof(n));
    if (n != cols || !fb.good())
        return false;
    b = VectorIncognitas(n);
    int i = 0;
    ok = true;
    while (i < n && ok)
    {
        int32_t len = 0;
        fb.read((char *)&len, sizeof(len));
        if (fb.good() && len >= 0)
        {
            std::string s;
            s.resize((size_t)len);
            if (len > 0)
            {
                fb.read(&s[0], len);
                if (!fb.good())
                    ok = false;
            }
            if (ok)
                b.set(i, s);
        }
        else
            ok = false;
        ++i;
    }
    if (!ok)
        return false;

    int32_t mvec = 0;
    fb.read((char *)&mvec, sizeof(mvec));
    if (mvec != rows || !fb.good())
        return false;
    c = VectorComplejo(rows);
    int t = 0;
    ok = true;
    while (t < mvec && ok)
    {
        double mod = 0.0, ang = 0.0;
        fb.read((char *)&mod, sizeof(mod));
        fb.read((char *)&ang, sizeof(ang));
        if (fb.good())
            c.vectoraddback(fasorToComplejo(mod, ang));
        else
            ok = false;
        ++t;
    }
    return ok && fb.good();
}

// ================================================================
// ==========   CARGA POR ARCHIVO SEPARADO (A,b,c)   ==============
// ================================================================

// -------- TXT Rect --------
bool loadATxtRect(const std::string &pathA, MatrizComplejo &A, PairOrder order)
{
    std::ifstream fi(pathA);
    if (!fi.good())
        return false;

    std::vector<std::vector<double>> rows;
    std::string line;
    while (std::getline(fi, line))
    {
        if (!isBlank(line))
        {
            std::vector<double> buf;
            if (parseDoublesLine(line, buf))
                rows.push_back(buf);
        }
    }
    if ((int)rows.size() == 0)
        return false;
    int dpr = (int)rows[0].size();
    if ((dpr % 2) != 0)
        return false;
    int m = (int)rows.size(), n = dpr / 2;

    A = MatrizComplejo(m, n);
    int i = 0;
    while (i < m)
    {
        int j = 0;
        while (j < n)
        {
            int idx = 2 * j;
            A.set(i, j, fromRectPair(rows[i][idx], rows[i][idx + 1], order));
            ++j;
        }
        ++i;
    }
    return true;
}
bool loadBTxt(const std::string &pathB, VectorIncognitas &b)
{
    std::ifstream fi(pathB);
    if (!fi.good())
        return false;

    std::vector<std::string> labs;
    std::string line;
    while (std::getline(fi, line))
    {
        if (!isBlank(line))
            labs.push_back(trim(line));
    }
    if ((int)labs.size() == 0)
        return false;

    b = VectorIncognitas((int)labs.size());
    int i = 0;
    while (i < b.size())
    {
        b.set(i, labs[(size_t)i]);
        ++i;
    }
    return true;
}
bool loadCTxtRect(const std::string &pathC, VectorComplejo &c, PairOrder order)
{
    std::ifstream fi(pathC);
    if (!fi.good())
        return false;

    std::vector<std::vector<double>> rows;
    std::string line;
    while (std::getline(fi, line))
    {
        if (!isBlank(line))
        {
            std::vector<double> buf;
            if (parseDoublesLine(line, buf))
                rows.push_back(buf);
        }
    }
    if ((int)rows.size() == 0)
        return false;

    c = VectorComplejo((int)rows.size());
    int i = 0;
    bool ok = true;
    while (i < (int)rows.size() && ok)
    {
        if ((int)rows[i].size() == 2)
            c.vectoraddback(fromRectPair(rows[i][0], rows[i][1], order));
        else
            ok = false;
        ++i;
    }
    return ok;
}

// -------- TXT Fasor --------
bool loadATxtFasor(const std::string &pathA, MatrizComplejo &A)
{
    std::ifstream fi(pathA);
    if (!fi.good())
        return false;

    std::vector<std::vector<double>> rows;
    std::string line;
    while (std::getline(fi, line))
    {
        if (!isBlank(line))
        {
            std::vector<double> buf;
            if (parseDoublesLine(line, buf))
                rows.push_back(buf);
        }
    }
    if ((int)rows.size() == 0)
        return false;
    int dpr = (int)rows[0].size();
    if ((dpr % 2) != 0)
        return false;
    int m = (int)rows.size(), n = dpr / 2;

    A = MatrizComplejo(m, n);
    int i = 0;
    while (i < m)
    {
        int j = 0;
        while (j < n)
        {
            int idx = 2 * j;
            A.set(i, j, fasorToComplejo(rows[i][idx], rows[i][idx + 1]));
            ++j;
        }
        ++i;
    }
    return true;
}
bool loadCTxtFasor(const std::string &pathC, VectorComplejo &c)
{
    std::ifstream fi(pathC);
    if (!fi.good())
        return false;

    std::vector<std::vector<double>> rows;
    std::string line;
    while (std::getline(fi, line))
    {
        if (!isBlank(line))
        {
            std::vector<double> buf;
            if (parseDoublesLine(line, buf))
                rows.push_back(buf);
        }
    }
    if ((int)rows.size() == 0)
        return false;

    c = VectorComplejo((int)rows.size());
    int i = 0;
    bool ok = true;
    while (i < (int)rows.size() && ok)
    {
        if ((int)rows[i].size() == 2)
            c.vectoraddback(fasorToComplejo(rows[i][0], rows[i][1]));
        else
            ok = false;
        ++i;
    }
    return ok;
}

// -------- BIN Rect --------
bool loadABinRect(const std::string &pathA, MatrizComplejo &A, PairOrder order)
{
    std::ifstream fb(pathA, std::ios::binary);
    if (!fb.good())
        return false;

    int32_t rows = 0, cols = 0;
    fb.read((char *)&rows, sizeof(rows));
    fb.read((char *)&cols, sizeof(cols));
    if (rows <= 0 || cols <= 0 || !fb.good())
        return false;

    A = MatrizComplejo(rows, cols);
    int r = 0;
    bool ok = true;
    while (r < rows && ok)
    {
        int j = 0;
        while (j < cols && ok)
        {
            double a = 0.0, bval = 0.0;
            fb.read((char *)&a, sizeof(a));
            fb.read((char *)&bval, sizeof(bval));
            if (fb.good())
                A.set(r, j, fromRectPair(a, bval, order));
            else
                ok = false;
            ++j;
        }
        ++r;
    }
    return ok && fb.good();
}
bool loadBBin(const std::string &pathB, VectorIncognitas &b)
{
    std::ifstream fb(pathB, std::ios::binary);
    if (!fb.good())
        return false;

    int32_t n = 0;
    fb.read((char *)&n, sizeof(n));
    if (n <= 0 || !fb.good())
        return false;

    b = VectorIncognitas(n);
    int i = 0;
    bool ok = true;
    while (i < n && ok)
    {
        int32_t len = 0;
        fb.read((char *)&len, sizeof(len));
        if (fb.good() && len >= 0)
        {
            std::string s;
            s.resize((size_t)len);
            if (len > 0)
            {
                fb.read(&s[0], len);
                if (!fb.good())
                    ok = false;
            }
            if (ok)
                b.set(i, s);
        }
        else
            ok = false;
        ++i;
    }
    return ok && fb.good();
}
bool loadCBinRect(const std::string &pathC, VectorComplejo &c, PairOrder order)
{
    std::ifstream fb(pathC, std::ios::binary);
    if (!fb.good())
        return false;

    int32_t m = 0;
    fb.read((char *)&m, sizeof(m));
    if (m <= 0 || !fb.good())
        return false;

    c = VectorComplejo(m);
    int i = 0;
    bool ok = true;
    while (i < m && ok)
    {
        double a = 0.0, bval = 0.0;
        fb.read((char *)&a, sizeof(a));
        fb.read((char *)&bval, sizeof(bval));
        if (fb.good())
            c.vectoraddback(fromRectPair(a, bval, order));
        else
            ok = false;
        ++i;
    }
    return ok && fb.good();
}

// -------- BIN Fasor --------
bool loadABinFasor(const std::string &pathA, MatrizComplejo &A)
{
    std::ifstream fb(pathA, std::ios::binary);
    if (!fb.good())
        return false;

    int32_t rows = 0, cols = 0;
    fb.read((char *)&rows, sizeof(rows));
    fb.read((char *)&cols, sizeof(cols));
    if (rows <= 0 || cols <= 0 || !fb.good())
        return false;

    A = MatrizComplejo(rows, cols);
    int r = 0;
    bool ok = true;
    while (r < rows && ok)
    {
        int j = 0;
        while (j < cols && ok)
        {
            double mod = 0.0, ang = 0.0;
            fb.read((char *)&mod, sizeof(mod));
            fb.read((char *)&ang, sizeof(ang));
            if (fb.good())
                A.set(r, j, fasorToComplejo(mod, ang));
            else
                ok = false;
            ++j;
        }
        ++r;
    }
    return ok && fb.good();
}
bool loadCBinFasor(const std::string &pathC, VectorComplejo &c)
{
    std::ifstream fb(pathC, std::ios::binary);
    if (!fb.good())
        return false;

    int32_t m = 0;
    fb.read((char *)&m, sizeof(m));
    if (m <= 0 || !fb.good())
        return false;

    c = VectorComplejo(m);
    int i = 0;
    bool ok = true;
    while (i < m && ok)
    {
        double mod = 0.0, ang = 0.0;
        fb.read((char *)&mod, sizeof(mod));
        fb.read((char *)&ang, sizeof(ang));
        if (fb.good())
            c.vectoraddback(fasorToComplejo(mod, ang));
        else
            ok = false;
        ++i;
    }
    return ok && fb.good();
}

// ================================================================
// =====================  WRAPPERS 3-path  ========================
// ================================================================
bool load3RectTxt(const std::string &pathA,
                  const std::string &pathB,
                  const std::string &pathC,
                  MatrizComplejo &A, VectorIncognitas &b, VectorComplejo &c,
                  PairOrder order)
{
    bool okA = loadATxtRect(pathA, A, order);
    bool okB = loadBTxt(pathB, b);
    bool okC = loadCTxtRect(pathC, c, order);
    return okA && okB && okC;
}
bool load3RectBin(const std::string &pathA,
                  const std::string &pathB,
                  const std::string &pathC,
                  MatrizComplejo &A, VectorIncognitas &b, VectorComplejo &c,
                  PairOrder order)
{
    bool okA = loadABinRect(pathA, A, order);
    bool okB = loadBBin(pathB, b);
    bool okC = loadCBinRect(pathC, c, order);
    return okA && okB && okC;
}
bool load3FasorTxt(const std::string &pathA,
                   const std::string &pathB,
                   const std::string &pathC,
                   MatrizComplejo &A, VectorIncognitas &b, VectorComplejo &c)
{
    bool okA = loadATxtFasor(pathA, A);
    bool okB = loadBTxt(pathB, b);
    bool okC = loadCTxtFasor(pathC, c);
    return okA && okB && okC;
}
bool load3FasorBin(const std::string &pathA,
                   const std::string &pathB,
                   const std::string &pathC,
                   MatrizComplejo &A, VectorIncognitas &b, VectorComplejo &c)
{
    bool okA = loadABinFasor(pathA, A);
    bool okB = loadBBin(pathB, b);
    bool okC = loadCBinFasor(pathC, c);
    return okA && okB && okC;
}
