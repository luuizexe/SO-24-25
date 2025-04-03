#include "memlist.h"

void createEmptyListM(tListM *L)
{
    *L = LNULL;
}

bool isEmptyListM(tListM L)
{
    return (L == LNULL);
}

tPosM firstM(tListM L)
{
    return L;
}

tPosM lastM(tListM L)
{
    tPosM p;
    for (p = L; p->next != LNULL; p = p->next)
        ;
    return p;
}

tPosM nextM(tPosM p)
{
    return p->next;
}

tPosM previousM(tPosM p, tListM L)
{
    tPosM q;
    if (p == L)
    {
        return LNULL;
    }
    else
    {
        for (q = L; q->next != p; q = q->next)
            ;
        return q;
    }
}

bool insertItemM(tItemM d, tPosM p, tListM *L)
{
    tPosM q, r;
    q = malloc(sizeof(*q));
    if (q == LNULL)
    {
        return false;
    }
    else
    {
        q->data = d;
        q->next = LNULL;
        if (*L == LNULL)
        {
            *L = q;
        }
        else if (p == LNULL)
        {
            for (r = *L; r->next != LNULL; r = r->next)
                ;
            r->next = q;
        }
        else if (p == *L)
        {
            q->next = *L;
            *L = q;
        }
        else
        {
            q->data = p->data;
            p->data = d;
            q->next = p->next;
            p->next = q;
        }
        return true;
    }
}

tItemM getItemM(tPosM p)
{
    return p->data;
}

tPosM findItemByName(char d[], tListM L)
{
    tPosM p;
    if (L == LNULL)
    {
        return LNULL;
    }
    else
    {
        for (p = L; p != LNULL; p = p->next)
        {
            if (strcmp(p->data.name, d) == 0 && p->data.type == MMAP)
            {
                return p;
            }
        }
        return LNULL;
    }
}
tPosM findItemByBytes(size_t n, tListM L)
{
    tPosM p;
    if (L == LNULL)
    {
        return LNULL;
    }
    else
    {
        for (p = L; p != LNULL; p = p->next)
        {
            if (p->data.tam == n && p->data.type == MALLOC)
            {
                return p;
            }
        }
        return LNULL;
    }
}
tPosM findItemByKey(key_t n, tListM L)
{
    tPosM p;
    if (L == LNULL)
    {
        return LNULL;
    }
    else
    {
        for (p = L; p != LNULL; p = p->next)
        {
            if (p->data.key == n && p->data.type == SHARED)
            {
                return p;
            }
        }
        return LNULL;
    }
}
tPosM findItemByAddress(void *d, tListM L)
{
    tPosM p;
    if (L == LNULL)
    {
        return LNULL;
    }
    else
    {
        for (p = L; p != LNULL; p = p->next)
        {
            if (p->data.address == d)
            {
                return p;
            }
        }
        return LNULL;
    }
}

int countBlocksWithKey(key_t clave, tListM L)
{
    // Funcion para contar cuantos bloques tienen una determinada clave
    int count = 0;
    tPosM p;

    if (L == LNULL)
    {
        return count;
    }
    else
    {
        for (p = L; p != LNULL; p = p->next)
        {
            if (p->data.key == clave && p->data.type == SHARED)
            {
                count++;
            }
        }
        return count;
    }
}

void deleteAtPositionM(tPosM p, tListM *L)
{
    tPosM q;
    if (p == *L)
    {
        *L = (*L)->next;
    }
    else if (p->next == LNULL)
    {
        for (q = *L; q->next != p; q = q->next)
            ;
        q->next = LNULL;
    }
    else
    {
        q = p->next;
        p->data = q->data;
        p->next = q->next;
        p = q;
    }
    free(p);
}
