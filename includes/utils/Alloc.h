#pragma once

#include <stdint.h>

struct Entry
{
    Entry(Entry* _prev = nullptr, Entry* _next = nullptr, unsigned long long _size = 0)
        : prev(_prev), next(_next), size(_size), free(true) {}

    Entry* prev;
    Entry* next;
    unsigned long long size;
    bool free;
    uint8_t* data;
};

class Alloc
{
public:
    Alloc(unsigned long long size);
    ~Alloc();

    uint8_t* alloc(unsigned long long size);
    void free(uint8_t* ptr);

private:

    Entry* root;
    unsigned long long spaceUsed;
    unsigned long long spaceFree;
};