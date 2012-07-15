/*
 * uMod_ModElement.cpp
 *
 *  Created on: 26.06.2012
 *      Author: marts
 */


#include "uMod_Main.h"


int Compare_uMod_TextureElement(uMod_TextureElement* a,uMod_TextureElement* b)
{
  if ( a->Hash() <  b->Hash()) return -1;
  else if ( a->Hash() ==  b->Hash()) return 0;
  else return +1;
}

