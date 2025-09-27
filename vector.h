#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
using namespace std;

class vector
{
    private:
    int* a;
    int size;
    int maxsize;
    
    public:

    vector(int size=10);
    ~vector();
    int vectorget(int index);
    void vectorset(int index, int value);
    void vectorinsert(int index, int value);
    int vectorisfull();
    void vectorresize();
    void vectorremove(int value);
    int vectorfind(int index, int value);
    void vectoraddback(int value);
    void vectoraddfront(int value);
    int vectorgetsize();
    int vectorgetmaxsize();
    vector operator+()const;
    vector operator-()const;
    vector operator*()const;

};

#endif