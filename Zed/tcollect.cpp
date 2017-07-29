/*
** tcollect.cpp
**
** Defines TCollection etc. related members.
**
** Copyright (c) 2000-2007 Alex Derbeev (adapted from public domain 
**   by Borland/Inprise).
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#include <string.h>

#include "tcollect.h"
#include "minmax.h"

inline void TObject::destroy(TObject *o)
{
    if (o != 0)
        o->shutDown();
    if (o)
        delete o;
}

TNSCollection::TNSCollection(ccIndex aLimit, ccIndex aDelta) :
    items(0),
    count(0),
    limit(0),
    delta(aDelta),
    shouldDelete(True)
{
    setLimit(aLimit);
}

TNSCollection::TNSCollection() :
    items(0),
    count(0),
    limit(0),
    delta(0),
    shouldDelete(True)
{
    items = 0;
}

TNSCollection::~TNSCollection()
{
    if (items)
        delete[] items;
}

void TNSCollection::shutDown()
{
    if (shouldDelete)
        freeAll();
    setLimit(0);
    TObject::shutDown();
}

void *TNSCollection::at(ccIndex index)
{
    if (index < 0 || index >= count)
        error(1, index);
    return items[index];
}

void TNSCollection::atRemove(ccIndex index)
{
    if (index < 0 || index >= count)
        error(1, index);

    count--;
    if (count - index)
        memmove(&items[index], &items[index + 1], (count - index) * sizeof(void *));
}

void TNSCollection::atRemove(ccIndex index, int n)
{
    if (index < 0 || index >= count)
        error(1, index);
    if (n < 0)
        error(2, n);
    if (n == 0)
        return;

    int N = MIN(n, count - index);
    count -= N;
    if (count - index)
        memmove(items + index, items + index + N, (count - index) * sizeof(void *));
}

void TNSCollection::atInsertDummy(ccIndex index, int n)
{
    if (index < 0 || index > count)
        error(1, index);
    if (n < 0)
        error(2, n);
    if (n == 0)
        return;

    int nextcount = count + n;

    if (nextcount > limit)
    {
        int nextlimit = limit;
        while (nextcount > nextlimit)
            nextlimit = calculate_limit(nextlimit, delta);
        setLimit(nextlimit);
    }

    if (count - index)
        memmove(&items[index + n], &items[index], (count - index) * sizeof(void *));
    count += n;

    for (int i = 0; i < n; ++i)
        items[index + i] = NULL;
}

void TNSCollection::atFree(ccIndex index)
{
    void *item = at(index);
    atRemove(index);
    freeItem(item);
}

void TNSCollection::atFree(ccIndex index, int n)
{
    if (index < 0 || index >= count)
        error(1, index);
    if (n < 0)
        error(2, n);
    if (n == 0)
        return;

    int N = MIN(n, count - index);
    for (int i = index + N - 1; i >= index; --i)
        freeItem(at(i));
    atRemove(index, n);
}

void TNSCollection::atInsert(ccIndex index, void const *item)
{
    if (index < 0)
        error(1, index);
    if (count == limit)
        setLimit(calculate_limit(count, delta));

    if (count - index)
        memmove(&items[index + 1], &items[index], (count - index) * sizeof(void *));
    count++;

    items[index] = (void *) item;
}

void TNSCollection::atPut(ccIndex index, void const *item)
{
    if (index < 0 || index >= count)
        error(1, index);

    items[index] = (void *) item;
}

void TNSCollection::remove(void const *item)
{
    atRemove(indexOf(item));
}

void TNSCollection::error(int code, ccIndex t)
{
    if (code == 1)
        throw TCEInvalidIndex(t);
    else throw TCollectionException();
}

void *TNSCollection::firstThat(ccTestFunc Test, void *arg)
{
    for (ccIndex i = 0; i < count; i++)
    {
        if (Test(items[i], arg) == True)
            return items[i];
    }
    return 0;
}

void *TNSCollection::lastThat(ccTestFunc Test, void *arg)
{
    for (ccIndex i = count; i > 0; i--)
    {
        if (Test(items[i - 1], arg) == True)
            return items[i - 1];
    }
    return 0;
}

void TNSCollection::forEach(ccAppFunc action, void *arg)
{
    for (ccIndex i = 0; i < count; i++)
        action(items[i], arg);
}

#ifdef MSS
#undef free //cpp must be destroyed!(bjarne)
#endif
void TNSCollection::free(void *item)
{
    remove(item);
    freeItem(item);
}

void TNSCollection::freeAll()
{
    for (ccIndex i =  0; i < count; i++)
        freeItem(at(i));
    count = 0;
}

void TNSCollection::freeItem(void *item)
{
    if (item)
        delete (char*)item;
}

ccIndex TNSCollection::indexOf(void const *item)
{
    for (ccIndex i = 0; i < count; i++)
    {
        if(item == items[i])
            return i;
    }

    /*error(1, -1);*/
    return ccNotFound;
}

ccIndex TNSCollection::insert(void const *item)
{
    ccIndex loc = count;
    atInsert(count, item);
    return loc;
}

void TNSCollection::pack()
{
    void **curDst = items;
    void **curSrc = items;
    void **last = items + count;
    while (curSrc < last)
    {
        if (*curSrc != 0)
            *curDst++ = *curSrc;
        curSrc++;
    }
}

void TNSCollection::setLimit(ccIndex aLimit)
{
    if (aLimit < count)
        aLimit =  count;
    if (aLimit > maxCollectionSize)
        aLimit = maxCollectionSize;
    if (aLimit != limit)
    {
        void **aItems;
        if (aLimit == 0)
            aItems = 0;
        else
        {
            aItems = new void *[aLimit];
            if (count != 0)
                memcpy(aItems, items, count * sizeof(void *));
        }
        if (items)
            delete[] items;
        items =  aItems;
        limit =  aLimit;
    }
}

ccIndex TNSSortedCollection::indexOf(void const *item)
{
    ccIndex  i;

    if (search(keyOf(item), i) == 0)
        return ccNotFound;
    else
    {
        if (duplicates)
        {
            while (i < count && item != items[i])
                i++;
        }
        if (i < count)
            return i;
        else return ccNotFound;
    }
}

ccIndex TNSSortedCollection::insert(void const *item)
{
    ccIndex  i;
    if (search(keyOf(item), i) == 0 || duplicates)      // order dependency!
        atInsert(i, item);                              // must do Search
                                                        // before calling
                                                        // AtInsert
    return i;
}

void *TNSSortedCollection::keyOf(void const *item)
{
    return (void *) item;
}

Boolean TNSSortedCollection::search(void const *key, ccIndex& index)
{
    ccIndex l = 0;
    ccIndex h = count - 1;
    Boolean res = False;

    while (l <= h)
    {
        ccIndex i = (l + h) >> 1;
        ccIndex c = compare(keyOf(items[i]), key);
        if (c < 0)
            l = i + 1;
        else
        {
            h = i - 1;
            if (c == 0)
            {
                res = True;
                if (!duplicates)
                    l = i;
            }
        }
    }
    index = l;
    return res;
}

TStringCollection::TStringCollection(int aLimit, int aDelta) :
    TSortedCollection(aLimit, aDelta)
{
}

int TStringCollection::compare(void const *key1, void const *key2)
{
    return strcmp((char *) key1, (char *) key2);
}

void TStringCollection::freeItem(void *item)
{
    if (item)
        delete[] (char *) item;
}
