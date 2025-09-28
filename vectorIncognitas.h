// VectorEtiquetas.h
#ifndef VECTOR_INCOGNITAS_H
#define VECTOR_INCOGNITAS_H

#include <string>
#include <vector>
#include <iostream>

class VectorIncognitas
{
private:
    std::vector<std::string> data;

public:
    VectorIncognitas() {}
    explicit VectorIncognitas(int n) : data(n) {}

    void add(const std::string &s) { data.push_back(s); }
    int size() const { return static_cast<int>(data.size()); }
    std::string get(int i) const { return data[i]; }
    void set(int i, const std::string &s) { data[i] = s; }

    void print(std::ostream &os = std::cout) const
    {
        int i = 0;
        while (i < size())
        {
            os << data[i];
            if (i + 1 < size())
            {
                os << " ";
            }
            ++i;
        }
        os << "\n";
    }
};

#endif
