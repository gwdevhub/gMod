/*
This file is part of OpenTexMod.


OpenTexMod is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenTexMod is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef OTM_ADDTEXTURE_H_
#define OTM_ADDTEXTURE_H_

#include "OTM_Main.h"

class AddTextureClass
{
public:
  AddTextureClass(void);
  ~AddTextureClass(void);
  int ReleaseMemory(void);

  int SetSize(int num);
  int InheriteMemory(AddTextureClass &tex);

  unsigned int Num;
  char **Textures;
  unsigned int *Size;
  unsigned long *Hash;
  bool *WasAdded;
  unsigned int Len;

  bool Add;
  bool Force;
  bool Loaded;
  bool OwnMemory;
  wxString File;
  wxString Comment;


};



#endif /* OTM_ADDTEXTURE_H_ */
