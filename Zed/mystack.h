/*
** mystack.h
**
** Ugly implementation of stack.
**
** Copyright (c) 2000-2007 Alex Derbeev.
**
** You can contact me via internet,
** my e-mail address is <zedplace@mail.ru>.
*/

#ifndef __MYSTACK_H__
#define __MYSTACK_H__

class __StackEmpty {};
class __BadAccess : public __StackEmpty {};

template <class T> class __stack
{
    __stack<T>  *next;
public:
    T           data;

    T getdata() const           {return data;}
public:
    __stack(__stack<T> *sp, T const &stuff) :
        next(sp),
        data(stuff)
    {}

public:
    static void push(__stack<T>* &sp, T const &stuff);
    static T pop(__stack<T>* &sp);
    static T poptail(__stack<T>* &sp);
    static void destroy(__stack<T>* &sp);
};

template <class T> void __stack<T>::push(__stack<T>* &sp, T const &stuff)
{
    sp = new __stack<T>(sp, stuff);
}

template <class T> T __stack<T>::pop(__stack<T>* &sp)
{
    if (sp == 0)
        throw __StackEmpty();
    T stuff = sp->data;
    __stack<T> *t = sp->next;
    delete sp;
    sp = t;
    return stuff;
}

template <class T> T __stack<T>::poptail(__stack<T>* &sp)
{
    if (sp == 0)
        throw __StackEmpty();
    if (sp->next == 0)
        return pop(sp);
    return poptail(sp->next);
}

template <class T> void __stack<T>::destroy(__stack<T>* &sp)
{
    while (sp != 0)
    {
        __stack<T> *t = sp->next;
        delete sp;
        sp = t;
    }
}


template <class T> class Stack
{
    __stack<T>  *sp;
    int         count;
public:
    Stack() :
        sp(0),
        count(0)
    {}
    ~Stack()                            {destroy();}
    int getcount() const                {return count;}
    int operator~ () const              {return count;}
    T getdata() const
    {
        if (sp)
            return sp->data;
        throw __BadAccess();
    }
    T operator() () const               {return getdata();}
    T operator* () const                {return getdata();}
    void push(T const &stuff)
    {
        __stack<T>::push(sp, stuff);
        count++;
    }
    T pop()
    {
        if (count > 0)
            count--;
        return __stack<T>::pop(sp);
    }
    T poptail()
    {
        if (count > 0)
            count--;
        return __stack<T>::poptail(sp);
    }
    void operator+= (T const &stuff)    {push(stuff);}
    T operator-- ()    /* prefix */     {return pop();}
    T operator-- (int) /* postfix */    {return poptail();}
    void destroy()
    {
        if (sp)
            __stack<T>::destroy(sp);
        count = 0;
    }
    void copy(Stack<T> &src)    // create copy of src (src *destroyed*)
    {
        destroy();
        while (~src)
            push(src--);
    }
    void icopy(Stack<T> &src)   // create inverse copy of src (src *destroyed*)
    {
        destroy();
        while (~src)
            push(--src);
    }
    void inverse()
    {
        if (!sp)
            return;
        Stack<T> t;
        while (count)
            t += poptail();
        icopy(t);
    }
};

#endif //__MYSTACK_H__
