/*
** tcollect.h
**
** Defines TCollection etc. classes.
**
** Copyright (c) 2000-2007 Alex Derbeev (adapted from public domain 
**   by Borland/Inprise).
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

//////////////////////////////////////////////////////////////////////
//             ษอออออออออออออออออออป
//             บ      TObject      บ
//             ศออออออออัออออออออออผ
//              ษอออออออฯอออออออป
//              บ TNSCollection บ
//              ศออัอออออออออัออผ
//          ฺฤฤฤฤฤฤู         ภฤฤฤฤฤฤฟ
//    ษอออออฯอออออออออออออป     ษอออฯออออออออป
//    บTNSSortedCollectionบ     บ TCollectionบ
//    ศอออออัอออออออออออออผ     ศอออัออออออออผ
//          ภฤฤฤฤฤฤฤฤฤฤฤยฤฤฤฤฤฤฤฤฤฤฤู
//            ษอออออออออฯอออออออออป
//            บ TSortedCollection บ
//            ศอออออออออัอออออออออผ
//            ษอออออออออฯอออออออออป
//            บ TStringCollection บ
//            ศอออออออออออออออออออผ
//

#ifndef __TCOLLECT_H__
#define __TCOLLECT_H__

#include <limits.h>

int const maxCollectionSize = (int) (UINT_MAX / sizeof(void *));
enum Boolean {False, True};

typedef unsigned short ushort;
typedef unsigned char uchar;

typedef int ccIndex;

typedef Boolean (*ccTestFunc) (void *, void *);
typedef void (*ccAppFunc) (void *, void *);

int const ccNotFound = -1;

class TCollectionException {};
class TCEInvalidIndex : public TCollectionException
{
public:
    ccIndex index;
    TCEInvalidIndex(ccIndex i = -1) : index(i) {}
};

class TObject
{
public:
    virtual ~TObject() {}

    static void destroy(TObject *);
    virtual void shutDown() {}
};

class TNSCollection : public TObject
{
public:
    TNSCollection(ccIndex aLimit, ccIndex aDelta);
    virtual ~TNSCollection();

    virtual void shutDown();

    void *at(ccIndex index);

    virtual ccIndex indexOf(void const *item);

    void atFree(ccIndex index);
    void atRemove(ccIndex index);

    void atFree(ccIndex index, int n);
    void atRemove(ccIndex index, int n);

    void atInsertDummy(ccIndex index, int n);

    void remove(void const *item);
    void removeAll() {count = 0;}
#ifdef MSS
#undef free //cpp must be destroyed!(bjarne)
#endif
    void free(void *item);
    void freeAll();

    void atInsert(ccIndex index, void const *item);
    void atPut(ccIndex index, void const *item);
    virtual ccIndex insert(void const *item);

    static void error(int code, ccIndex info);

    void *firstThat(ccTestFunc Test, void *arg);
    void *lastThat(ccTestFunc Test, void *arg);
    void forEach(ccAppFunc action, void *arg);

    void pack();
    virtual void setLimit(ccIndex aLimit);

    virtual int calculate_limit(int aLimit, int aDelta) const
    {
        return aLimit + aDelta;
    }

    ccIndex getCount() const {return count;}

protected:
    TNSCollection();

    void **items;
    ccIndex count;
    ccIndex limit;
    ccIndex delta;
    Boolean shouldDelete;

private:
    virtual void freeItem(void *item);
};

class TNSSortedCollection: public virtual TNSCollection
{
public:
    TNSSortedCollection(ccIndex aLimit, ccIndex aDelta) :
        TNSCollection(aLimit, aDelta), duplicates(False)
    {
        delta = aDelta;
        setLimit(aLimit);
    }

    virtual Boolean search(void const *key, ccIndex& index);

    virtual ccIndex indexOf(void const *item);
    virtual ccIndex insert(void const *item);

    Boolean duplicates;
    virtual void *keyOf(void const *item);

protected:
    TNSSortedCollection() : duplicates(False) {}

private:
    virtual int compare(void const *key1, void const *key2) = 0;
};

class TCollection : public virtual TNSCollection
{
public:
    TCollection(ccIndex aLimit, ccIndex aDelta)
    {
        delta = aDelta;
        setLimit(aLimit);
    }
};

class TSortedCollection : public TNSSortedCollection, public TCollection
{
public:
    TSortedCollection(ccIndex aLimit, ccIndex aDelta) :
        TCollection(aLimit, aDelta) {}

private:
    virtual int compare(void const *key1, void const *key2) = 0;
};

class TStringCollection : public TSortedCollection
{
public:
    TStringCollection(int aLimit, int aDelta);

private:
    virtual int compare(void const *key1, void const *key2);
    virtual void freeItem(void *item);
};

#endif //__TCOLLECT_H__
