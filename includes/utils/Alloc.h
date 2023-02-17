#pragma once

struct Entry
{
    Entry(Entry* _prev = nullptr, Entry* _next = nullptr, unsigned long long _size = 0)
        : prev(_prev), next(_next), size(_size), free(true) {}

    Entry* prev;
    Entry* next;
    unsigned long long size;
    bool free;
    void* data;
};

class Alloc
{
public:
    Alloc(unsigned long long size);
    ~Alloc();

    void* alloc(unsigned long long size);
    void free(void* ptr);

private:

    Entry* root;
    unsigned long long spaceUsed;
    unsigned long long spaceFree;
};