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



#ifndef OTM_SENDER_H_
#define OTM_SENDER_H_
#include "OTM_Main.h"

class AddTextureClass
{
public:
  AddTextureClass(void);
  void SetSize(int num);
  ~AddTextureClass(void);

  unsigned int Num;
  char **Textures;
  unsigned int *Size;
  unsigned long *Hash;
  bool *Add;
  bool *Force;

  unsigned int Len;
};


// an object of this class is owned by each OTM_GamePage object. It act as sender ^^
class OTM_Sender
{
public:
  OTM_Sender(PipeStruct &pipe, OTM_Language &lang);
  ~OTM_Sender(void);

  int Send( const OTM_GameInfo &game, const OTM_GameInfo &game_old);

  wxString LastError;

private:
  int SendSaveAllTextures(bool val);
  int SendSaveSingleTexture(bool val);

  int SendTextures(unsigned int num, AddTextureClass *tex);

  int SendKey(int key, int ctr);

  int SendPath( const wxString &path);

  int SendColour( int* colour, int ctr);

  char *Buffer;
  int SendToGame( void* msg, unsigned long len);

  int AddFile( AddTextureClass *tex, wxString file, bool add, bool force);
  int AddZip( AddTextureClass *tex, wxString file, bool add, bool force, bool tpf);
  int AddContent( char* buffer, unsigned int len, const char* pw, AddTextureClass *tex, bool add, bool force);

  PipeStruct &Pipe;
  OTM_Language &Language;
};


#endif /* OTM_SENDER_H_ */
