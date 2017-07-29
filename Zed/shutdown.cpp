#include "shutdown.h"
//fix #include "stlstuff.h"

#include "readconf.h"
#include "input.h"

void shutdown() //garbage collector :)
{
/*fix
    for (int i = 0, type = PTR_Define, n = ptr_count(type); i < n; i++)
    {
        Define *ptr = (Define*) ptr_get(type, i);
        delete ptr;
    }
    for (int i = 0, type = PTR_History, n = ptr_count(type); i < n; i++)
    {
        History *ptr = (History*) ptr_get(type, i);
        TStringCollection *ptr2 = ptr->data;
        ptr2->freeAll();
        delete ptr2;
        delete ptr;
    }
*/
}
