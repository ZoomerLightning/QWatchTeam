#ifndef __MYHASH_H__
#define __MYHASH_H__

#include <algorithm>
#include <string>
#include <hash_set>

struct HASH
{
    static unsigned const MULTIPLIER = 31;
    struct myhash : public std::hash<std::string>
    {
        struct mystate : public std::unary_function<char, void>
        {
            mystate() : h(0) {}
            void operator()(char c) {h = MULTIPLIER*h+(unsigned char)c;}
            unsigned h;
        };
        size_t operator()(std::string const &s) const
        {
            return (size_t) std::for_each(s.begin(), s.end(), mystate()).h;
        }
    };
    typedef std::hash_set<std::string, myhash, std::equal_to<std::string> > myhashset;
};

#endif
