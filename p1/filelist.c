#include "filelist.h"

void createEmptyListF(tListF *L)
{
    *L = LNULL;
}

bool isEmptyListF(tListF L)
{
    return (L == LNULL);
}

tPosF firstF(tListF L)
{
    return L;
}

tPosF lastF(tListF L)
{
    tPosF p;
    for (p = L; p->next != LNULL; p = p->next)
        ;
    return p;
}

tPosF nextF(tPosF p)
{
    return p->next;
}

tPosF previousF(tPosF p, tListF L)
{
    tPosF q;
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

bool insertItemF(tItemF d, tPosF p, tListF *L)
{
    tPosF q, r;
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
            if (d.df < (*L)->data.df)
            {
                q->next = *L;
                *L = q;
            }
            else
            {
                for (r = *L; r->next != LNULL && r->next->data.df < d.df; r = r->next)
                    ;
                q->next = r->next;
                r->next = q;
            }
        }
        else
        {
            q->next = p->next;
            p->next = q;
        }

        return true;
    }
}

void deleteAtPositionF(tPosF p, tListF *L)
{
    tPosF q;
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

tItemF getItemF(tPosF p)
{
    return p->data;
}

tPosF findItemF(int d, tListF L)
{
    tPosF p;
    if (L == LNULL)
    {
        return LNULL;
    }
    else
    {
        for (p = L; p != LNULL; p = p->next)
        {
            if (p->data.df == d)
            {
                return p;
            }
        }
        return LNULL;
    }
}
