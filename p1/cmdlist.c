#include "cmdlist.h"

void createEmptyListC(tListC *L)
{
    *L = LNULL;
}

bool isEmptyListC(tListC L)
{
    return (L == LNULL);
}

tPosC firstC(tListC L)
{
    return L;
}

tPosC lastC(tListC L)
{
    tPosC p;
    for (p = L; p->next != LNULL; p = p->next)
        ;
    return p;
}

tPosC nextC(tPosC p)
{
    return p->next;
}

tPosC previousC(tPosC p, tListC L)
{
    tPosC q;
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

bool insertItemC(tItemC d, tPosC p, tListC *L)
{
    tPosC q, r;
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

tItemC getItemC(tPosC p)
{
    return p->data;
}

tPosC findItemC(int d, tListC L)
{
    tPosC p;
    if (L == LNULL)
    {
        return LNULL;
    }
    else
    {
        for (p = L; p != LNULL; p = p->next)
        {
            if (p->data.pos == d)
            {
                return p;
            }
        }
        return LNULL;
    }
}

void deleteAtPositionC(tPosC p, tListC *L)
{
    tPosC q;
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