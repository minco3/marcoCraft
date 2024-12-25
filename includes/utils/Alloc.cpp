#include "Alloc.h"
#include <stdlib.h>

Alloc::Alloc(unsigned long long size)
    : root(new Entry(nullptr, nullptr, size)), spaceFree(size), spaceUsed(0)
{
    root->data = new uint8_t[size];
}

Alloc::~Alloc()
{
    delete[] root->data;
    Entry* last;
    while (root)
    {
        last = root;
        root = root->next;
        delete last;
    }
}

uint8_t* Alloc::alloc(unsigned long long size)
{
    Entry* walker = root;
    while (walker)
    {
        if (walker->size >= size && walker->free)
            break;
        walker = walker->next;
    }
    if (!walker) return nullptr;

    spaceFree -= size;
    spaceUsed += size;

    if (walker->size == size)
    {
        walker->free = false;
        return walker->data;
    }
    else
    {
        Entry* remander = new Entry(walker, walker->next, walker->size-size);
        remander->data = walker->data + size;
        walker->next = remander;
        walker->free = false;
        walker->size = size;        
    }

    return walker->data;
}

void Alloc::free(uint8_t* ptr)
{
    Entry* walker = root;
    while (walker)
    {
        if (walker->data == ptr && !walker->free)
            break;
        walker = walker->next;
    }
    if (!walker) return;

    if (walker->prev && walker->prev->free)
    {
        walker->prev->size += walker->size;
        walker->prev->next = walker->next;
        walker->prev = nullptr;
        walker->next = nullptr;
        delete walker;
    }
    if (walker->next && walker->next->free)
    {
        walker->next->size += walker->size;
        walker->next->prev = walker->prev;
        walker->prev = nullptr;
        walker->next = nullptr;
        delete walker;
    }
    else
    {
        walker->free = true;
    }

}