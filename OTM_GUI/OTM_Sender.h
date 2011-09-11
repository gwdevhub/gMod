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
along with FooOpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef OTM_SENDER_H_
#define OTM_SENDER_H_
#include "OTM_Main.h"

// an object of this class is owned by each OTM_GamePage object. It act as sender ^^
class OTM_Sender
{
public:
  OTM_Sender(PipeStruct &pipe);
  ~OTM_Sender(void);

  int SendSaveAllTextures(bool val);
  int SendSaveSingleTexture(bool val);

  int SendTextures(const wxArrayString &textures, unsigned long *hash, bool *add, bool *force);

  int SendKeyBack(int key);
  int SendKeySave(int key);
  int SendKeyNext(int key);

  int SendPath( const wxString &path);

private:

  int SendToGame( void* msg, unsigned long len);
  PipeStruct &Pipe;
};


#endif /* OTM_SENDER_H_ */
