#include "dirlist.h"

void createEmptyListD(tListD *L)
{
    *L = LNULL;
}

bool isEmptyListD(tListD L)
{
    return (L == LNULL);
}

tPosD firstD(tListD L)
{
    return L;
}

tPosD lastD(tListD L)
{
    tPosD p;
    for (p = L; p->next != LNULL; p = p->next)
        ;
    return p;
}

tPosD nextD(tPosD p)
{
    return p->next;
}

tPosD previousD(tPosD p, tListD L)
{
    tPosD q;
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

bool insertItemD(tDir d, tPosD p, tListD *L)
{
    tPosD q, r;
    q = malloc(sizeof(*q));
    if (q == LNULL)
    {
        return false;
    }
    else
    {
        q->dir = strdup(d);
        if (q->dir == NULL)
        {
            free(q);
            return false;
        }
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
            q->dir = p->dir;
            p->dir = d;
            q->next = p->next;
            p->next = q;
        }
        return true;
    }
}

tDir getItemD(tPosD p)
{
    return p->dir;
}

tPosD findItemD(tDir d, tListD L)
{
    tPosD p;
    if (L == LNULL)
    {
        return LNULL;
    }
    else
    {
        for (p = L; p != LNULL; p = p->next)
        {
            if (strcmp(p->dir, d) == 0)
            {
                return p;
            }
        }
        return LNULL;
    }
}

void deleteAtPositionD(tPosD p, tListD *L)
{
    tPosD q;
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
        free(p->dir);
        p->dir = strdup(q->dir);
        p->next = q->next;
        free(q->dir);
        free(q);
        return;
    }
    free(p->dir);
    free(p);
}
