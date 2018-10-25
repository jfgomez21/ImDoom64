// -*- mode: c++ -*-
#ifndef __MAP__59976677
#define __MAP__59976677

#include <prelude.hh>

void* W_GetMapLump(int lump);

void W_CacheMapLump(int map);

void W_FreeMapLump();

int W_MapLumpLength(int lump);

void P_InitTextureHashTable(void);

uint32 P_GetTextureHashKey(int hash);

#endif //__MAP__59976677
