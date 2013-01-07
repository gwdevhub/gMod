/*
 * uMod_ModElement.cpp
 *
 *  Created on: 26.06.2012
 *      Author: marts
 */


#include "uMod_Main.h"




int uMod_ModElement::Release()
{
  if (--myCounter==0)
  {
    delete this;
    return 0;
  }
  else
    return myCounter;
}



void uMod_TextureElement::DeleteFileOnDisk(void)
{
  //wxMessageBox(myExtractedFile, "here");
  if (!myExtractedFile.IsEmpty())
    if (wxFileExists(myExtractedFile))
      wxRemoveFile(myExtractedFile);
}

int Compare_uMod_TextureElement(uMod_TextureElement* a,uMod_TextureElement* b)
{
  if ( a->Hash() <  b->Hash()) return -1;
  else if ( a->Hash() ==  b->Hash()) return 0;
  else return +1;
}

