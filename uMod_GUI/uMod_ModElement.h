/*
This file is part of Universal Modding Engine.


Universal Modding Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Universal Modding Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Universal Modding Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "uMod_Main.h"
#ifndef UMOD_MODELEMEMT_H_
#define UMOD_MODELEMEMT_H_


template <class T>
class uMod_Data
{
public:
  uMod_Data(void): myData((T*)0), myLen(0) {}
  ~uMod_Data() {if (myData!=(T*)0) delete [] myData;}

  T* &Data(void) {return myData;}
  const T* &Data(void) const {return myData;}

  unsigned int &Len(void) {return myLen;}
  const unsigned int &Len(void) const {return myLen;}

  int SetSize(int size)
  {
    if (myData!=(T*)0) delete [] myData;
    myData = (T*)0;
    if (size<=0) return 0;

    try {myData = new T[size];}
    catch (...) {myData=(T*)0;return -1;}
    myLen = size;
    return 0;
  }

private:

  T *myData;
  unsigned int myLen;

};


/**
 * This class is a representation of one element, to be modified (e.g. one texture)
 */

class uMod_ModElement
{
public:

  enum  enumType
  {
    Group,
    Texture
  };

  enum enumStatus
  {
    Activate,
    Deactivate,
    NONE
  };

  uMod_ModElement( enumType t,  enumStatus s = NONE) : myType(t), myStatus(s), myCounter(1) {}
  virtual ~uMod_ModElement() {}

  int AddRef(void) {return ++myCounter;}
  int Release(void) {if (--myCounter==0) {delete this;} return myCounter;}


  const wxString &File(void) const {return myFile;}
  wxString &File(void) {return myFile;}

  const wxString &Title(void) const {return myTitle;}
  wxString &Title(void) {return myTitle;}

  const wxString &Author(void) const {return myAuthor;}
  wxString &Author(void) {return myAuthor;}

  const wxString &Comment(void) const {return myComment;}
  wxString &Comment(void) {return myComment;}


  enumType Type(void) const {return myType;}
  //void SetType(Type type) {myType = type;}

  const enumStatus &Status(void) const {return myStatus;}
  enumStatus &Status(void) {return myStatus;}

private:

  wxString myFile;
  wxString myTitle;
  wxString myAuthor;
  wxString myComment;
  const  enumType myType;
  enumStatus myStatus;
  int myCounter;


private:
  /**
   * forbid assignment!
   * @param rh
   * @return
   */
  uMod_ModElement operator= (const uMod_ModElement & rh) {(void*) &rh; return *this;}
};


class uMod_TextureElement : public uMod_ModElement
{
public:
  uMod_TextureElement() : uMod_ModElement(Texture) {}
  virtual ~uMod_TextureElement() {}


  uMod_Data<char> &Content(void) {return myContent;}


  const DWORD64 &Hash(void) const {return myHash;}
  DWORD64 &Hash(void) {return myHash;}

private:
  uMod_Data<char> myContent;
  DWORD64 myHash;



  /**
   * forbid assignment!
   * @param rh
   * @return
   */
  uMod_TextureElement operator= (const uMod_TextureElement & rh) {(void*) &rh; return *this;}

};




//WX_DEFINE_ARRAY_PTR( uMod_ModElement*, uMod_ModElement_ArrayPtr );

WX_DEFINE_SORTED_ARRAY( uMod_TextureElement*, uMod_TextureElement_SortedArrayPtr );
int Compare_uMod_TextureElement(uMod_TextureElement* a, uMod_TextureElement* b);





#endif /* UMOD_MODELEMENT_H_ */
