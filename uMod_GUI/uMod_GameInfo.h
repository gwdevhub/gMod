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



#ifndef uMod_GAME_H_
#define uMod_GAME_H_

#include "uMod_TreeView.h"
#include "uMod_Main.h"

//this class is intended as a storing object for each game
// one should ad an assignment operator,  loading and saving default values, ...
class uMod_GameInfo
{
public:
  uMod_GameInfo(void);
  ~uMod_GameInfo(void);
  void Init(void);


  int SaveToString( wxString &content);
  int LoadFromString( const wxString &content);

  //int GetActiveTexture( uMod_TextureElement_SortedArrayPtr &texture);



  bool &ShowCollPane(void) {return myShowCollPane;}
  const bool &ShowCollPane(void) const {return myShowCollPane;}


  bool &SupportTPF(void) {return mySupportTPF;}
  const bool &SupportTPF(void) const {return mySupportTPF;}


  int SetSaveSingleTexture(bool val);
  bool GetSaveSingleTexture(void) const {return SaveSingleTexture;}

  int SetSaveAllTextures(bool val);
  bool GetSaveAllTextures(void) const {return SaveAllTextures;}


  int GetKeyBack() const {return KeyBack;}
  int SetKeyBack(int key) {KeyBack=key; return 0;}

  int GetKeySave() const {return KeySave;}
  int SetKeySave(int key) {KeySave=key; return 0;}

  int GetKeyNext() const {return KeyNext;}
  int SetKeyNext(int key) {KeyNext=key; return 0;}

  bool &ShowSingleTextureString() {return myShowSingleTextureString;}
  const bool &ShowSingleTextureString() const {return myShowSingleTextureString;}

  bool &ShowSingleTexture() {return myShowSingleTexture;}
  const bool &ShowSingleTexture() const {return myShowSingleTexture;}

  int SetFontColour(const unsigned char *colour) {FontColour[0]=colour[0];FontColour[1]=colour[1];FontColour[2]=colour[2];FontColour[3]=colour[3];return 0;}
  int GetFontColour(unsigned char *colour) const {colour[0]=FontColour[0];colour[1]=FontColour[1];colour[2]=FontColour[2];colour[3]=FontColour[3];return 0;}

  int SetTextureColour(const unsigned char *colour) {TextureColour[0]=colour[0];TextureColour[1]=colour[1];TextureColour[2]=colour[2];TextureColour[3]=colour[3];return 0;}
  int GetTextureColour(unsigned char *colour) const {colour[0]=TextureColour[0];colour[1]=TextureColour[1];colour[2]=TextureColour[2];colour[3]=TextureColour[3];return 0;}


  bool &UseSizeFilter(void) {return myUseSizeFilter;}
  const bool &UseSizeFilter(void) const {return myUseSizeFilter;}

  int &HeightMin(void) {return myHeightMin;}
  const int &HeightMin(void) const {return myHeightMin;}
  int &HeightMax(void) {return myHeightMax;}
  const int &HeightMax(void) const {return myHeightMax;}

  int &WidthMin(void) {return myWidthMin;}
  const int &WidthMin(void) const {return myWidthMin;}
  int &WidthMax(void) {return myWidthMax;}
  const int &WidthMax(void) const {return myWidthMax;}

  int &DepthMin(void) {return myDepthMin;}
  const int &DepthMin(void) const {return myDepthMin;}
  int &DepthMax(void) {return myDepthMax;}
  const int &DepthMax(void) const {return myDepthMax;}

  bool &UseFormatFilter(void) {return myUseFormatFilter;}
  const bool &UseFormatFilter(void) const {return myUseFormatFilter;}

  unsigned long &FormatFilter(void) {return myFormatFilter;}
  const unsigned long &FormatFilter(void) const {return myFormatFilter;}

  unsigned long &FileFormat(void) {return myFileFormat;}
  const unsigned long &FileFormat(void) const {return myFileFormat;}


  int SetOpenPath(const wxString &path) {OpenPath=path; return 0;}
  wxString GetOpenPath(void) const {return OpenPath;}

  int SetSavePath(const wxString &path) {SavePath=path; return 0;}
  wxString GetSavePath(void) const {return SavePath;}

  uMod_GameInfo& operator = (const  uMod_GameInfo &rhs);

private:
  bool myShowCollPane;

  bool mySupportTPF;

  bool SaveSingleTexture;
  bool SaveAllTextures;

  int KeyBack;
  int KeySave;
  int KeyNext;

  bool myUseSizeFilter;
  int myHeightMin;
  int myHeightMax;
  int myWidthMin;
  int myWidthMax;
  int myDepthMin;
  int myDepthMax;

  bool myUseFormatFilter;
  unsigned long myFormatFilter;
  unsigned long myFileFormat;

  bool myShowSingleTextureString;
  bool myShowSingleTexture;
  unsigned char FontColour[4];
  unsigned char TextureColour[4];

  wxString OpenPath;
  wxString SavePath;
};


#endif /* uMod_SERVER_H_ */
