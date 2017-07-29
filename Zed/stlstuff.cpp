#include <algorithm>
#include <string>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <hash_set>
#include <vector>
#include <list>
#include <cassert>
#include "stlstuff.h"
#include "myfile.h"
#include "mystring.h"

using namespace std;

#define e(n) char const *const ext##n[] = {
e(1) "c", "cas", "h", "cc", "cpp", "cxx", "c++", "ccp", "cp", "hpp", "hxx", "hp", "i", "ii", "occ", "m"};
e(2) "asm", "s", "nas", "nsm", "inc", "ash", "asi", "mac"};
e(3) "c--", "h--"};
e(4) "p", "pas", "pp"};
e(5) "idc"};
e(6) "hs"};
e(7) "adb", "ads"};
#undef e
static int const num_ext = 7;

/*typedef hash_set<string, hash<string>, equal_to<string> > myhashset;
static vector<myhashset> ext(num_ext);

static vector<vector<void*> > Ptr_list;*/

void *ptr_add(int type, void *ptr)
{
/*    if (type >= (int)Ptr_list.size())
        Ptr_list.resize(type+1);
    Ptr_list[type].push_back(ptr);
    return ptr;*/
	return 0;
}

int ptr_count(int type)
{
    //return type < (int)Ptr_list.size()? (int)Ptr_list[type].size() : 0;
	return 0;
}

void *ptr_get(int type, int n)
{
/*    assert(type < (int)Ptr_list.size());
    return Ptr_list[type][n];
	*/
	return 0;
}


void ext_init()
{
/*fix
#define extinit(n) ext[n-1].insert(ext##n, ext##n+sizeof(ext##n)/sizeof(ext##n[0]));
    extinit(1)
    extinit(2)
    extinit(3)
    extinit(4)
    extinit(5)
    extinit(6)
    extinit(7)
#undef extinit
*/
}

int getfiletype(char const *filename)
{
/*    String e_ref = getcomponent(filename, 8);
    if (e_ref.getwidth() == 0)
        return -1;
    assert(e_ref[0] == '.');
    if (e_ref.getwidth() == 1)
        return -1;
    e_ref.downcase();

    String e1 = e_ref.c_str()+1;
    String e2 = e_ref.c_str()+e_ref.getwidth();
    String e = e1 + e2;
    string ee(e.c_str());

    for (int i = 0; i < num_ext; i++)
        if (ext[i].find(ee.c_str()) != ext[i].end())
            return i;
*/
    return 0;
}
