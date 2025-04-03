#include "proclist.h"

void createEmptyListP(tListP *L)
{
    *L = LNULL;
}

bool isEmptyListP(tListP L)
{
    return (L == LNULL);
}

tPosP firstP(tListP L)
{
    return L;
}

tPosP lastP(tListP L)
{
    tPosP p;
    for (p = L; p->next != LNULL; p = p->next)
        ;
    return p;
}

tPosP nextP(tPosP p)
{
    if (p == NULL)
    {
        return LNULL;
    }
    return p->next;
}

tPosP previousP(tPosP p, tListP L)
{
    tPosP q;
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

bool insertItemP(tItemP d, tPosP p, tListP *L)
{
    tPosP q, r;
    q = malloc(sizeof(*q));
    if (q == LNULL)
    {
        return false;
    }
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
        if (p->data.cmd != NULL)
            free(p->data.cmd);
        if (p->data.tlaunch != NULL)
            free(p->data.tlaunch);

        q->data = p->data;
        p->data = d;
        q->next = p->next;
        p->next = q;
    }
    return true;
}

tItemP getItemP(tPosP p)
{
    return p->data;
}

tPosP findItemP(pid_t pid, tListP L)
{
    tPosP p;
    if (L == LNULL)
    {
        return LNULL;
    }
    else
    {
        for (p = L; p != LNULL; p = p->next)
        {
            if (pid == p->data.pid)
            {
                return p;
            }
        }
        return LNULL;
    }
}

void deleteAtPositionP(tPosP p, tListP *L)
{
    tPosP q;
    if (p == *L)
    {
        *L = (*L)->next;
    }
    else
    {
        for (q = *L; q->next != p; q = q->next)
            ;
        q->next = p->next;
        if (p->data.cmd != NULL)
        {
            free(p->data.cmd);
        }
        if (p->data.tlaunch != NULL)
        {
            free(p->data.tlaunch);
        }
        free(p);
    }
}
