#include "~/morxesjorge/ED2/TrabHash/Cliente.h"

typedef Cliente *Hash;

void initializeHashTable(Hash *tab, int m)
{
    int i;
    for (i = 0; i < m; i++)
    {
        tab[i] = NULL;
    }
}