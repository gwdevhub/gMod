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



uMod_GameInfo::uMod_GameInfo(void)
{
  Init();
}


uMod_GameInfo::~uMod_GameInfo(void)
{
}

void uMod_GameInfo::Init(void)
{
  myShowCollCapturePane = false;

  SaveSingleTexture = false;
  SaveAllTextures = false;
  myShowSingleTextureString = false;
  myShowSingleTexture = false;

  KeyBack = -1;
  KeySave = -1;
  KeyNext = -1;
  FontColour[0]=255;FontColour[1]=0;FontColour[2]=0;FontColour[3]=255;
  TextureColour[0]=0;TextureColour[1]=255;TextureColour[2]=0;TextureColour[3]=255;

  myUseSizeFilter = false;
  myHeightMin = 1;
  myHeightMax = 1;
  myWidthMin = 1;
  myWidthMax = 1;
  myDepthMin = 1;
  myDepthMax = 1;

  myUseFormatFilter = false;
  myFormatFilter = 0u;
  myFileFormat = uMod_D3DXIFF_DDS;

  SavePath = wxGetCwd();
  SavePath << "\\textures";

  OpenPath.Empty();


  myShowCollSettingsPane = false;
  mySupportTPF = false;
  myComputeRenderTargets = false;
  myExtractTexturesToDisk = true;
  myDeleteExtractedTexturesOnDisk = false;

  ExtractPath = wxGetCwd();
  ExtractPath << "\\ExtractedMods";
}

int uMod_GameInfo::SaveToString( wxString &content )
{
  content.Empty();

  wxString temp;

  temp.Printf( "ShowCollCapturePane:%d\n", myShowCollCapturePane);
  content << temp;

  if (SavePath.Len()>0)
  {
    temp.Printf( "SavePath:%ls\n", SavePath.wc_str());
    content << temp;
  }

  if (OpenPath.Len()>0)
  {
    temp.Printf( "OpenPath:%ls\n", OpenPath.wc_str());
    content << temp;
  }

  temp.Printf( "SaveAllTextures:%d\nSaveSingleTexture:%d\n", SaveAllTextures, SaveSingleTexture);
  content << temp;

  temp.Printf( "ShowSingleTextureString:%d\n", myShowSingleTextureString);
  content << temp;

  temp.Printf( "ShowSingleTexture:%d\n", myShowSingleTexture);
  content << temp;

  if (KeyBack>=0)
  {
    temp.Printf( "KeyBack:%d\n", KeyBack);
    content << temp;
  }
  if (KeySave>=0)
  {
    temp.Printf( "KeySave:%d\n", KeySave);
    content << temp;
  }
  if (KeyNext>=0)
  {
    temp.Printf( "KeyNext:%d\n", KeyNext);
    content << temp;
  }
  temp.Printf("UseSizeFilter:%d\n", myUseSizeFilter);
  content << temp;
  temp.Printf( "Height:%d,%d\n", myHeightMin, myHeightMax);
  content << temp;
  temp.Printf( "Width:%d,%d\n", myWidthMin, myWidthMax);
  content << temp;
  temp.Printf( "Depth:%d,%d\n", myDepthMin, myDepthMax);
  content << temp;


  temp.Printf("UseFormatFilter:%d\n", myUseFormatFilter);
  content << temp;
  if (myFormatFilter>0u)
  {
    temp.Printf( "FormatFilter:%lu\n", myFormatFilter);
    content << temp;
  }
  if (myFileFormat>0u)
  {
    temp.Printf( "FileFormat:%lu\n", myFileFormat);
    content << temp;
  }

  temp.Printf( L"FontColour:%u,%u,%u,%u\n", FontColour[0], FontColour[1], FontColour[2],FontColour[3]);
  content << temp;
  temp.Printf( L"TextureColour:%u,%u,%u,%u\n", TextureColour[0], TextureColour[1], TextureColour[2], TextureColour[3]);
  content << temp;



  temp.Printf( "ShowCollSettingsPane:%d\n", myShowCollSettingsPane);
  content << temp;

  temp.Printf( "SupportTPF:%d\n", mySupportTPF);
  content << temp;

  temp.Printf( "ComputeRenderTargets:%d\n", myComputeRenderTargets);
  content << temp;

  temp.Printf( "ExtractTexturesToDisk:%d\n", myExtractTexturesToDisk);
  content << temp;

  temp.Printf( "DeleteExtractedTexturesOnDisk:%d\n", myDeleteExtractedTexturesOnDisk);
  content << temp;

  if (ExtractPath.Len()>0)
  {
    temp.Printf( "ExtractPath:%ls\n", ExtractPath.wc_str());
    content << temp;
  }



  return 0;
}


int uMod_GameInfo::LoadFromString( const wxString &content)
{
  Init();

  wxStringTokenizer token( content, "\n");

  int num = token.CountTokens();


  wxString line;
  wxString command;
  wxString temp;
  //Files.Alloc(num);
  for (int i=0; i<num; i++)
  {
    line = token.GetNextToken();
    command = line.BeforeFirst(':');

    if (command == "ShowCollCapturePane")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myShowCollCapturePane = false;
      else myShowCollCapturePane = true;
    }
    else if (command == "SavePath") SavePath = line.AfterFirst(':');
    else if (command == "OpenPath") OpenPath = line.AfterFirst(':');
    else if (command == "SaveAllTextures")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') SaveAllTextures = false;
      else SaveAllTextures = true;
    }
    else if (command == "SaveSingleTexture")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') SaveSingleTexture = false;
      else SaveSingleTexture = true;
    }
    else if (command == "ShowSingleTextureString")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myShowSingleTextureString = false;
      else myShowSingleTextureString = true;
    }
    else if (command == "ShowSingleTexture")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myShowSingleTexture = false;
      else myShowSingleTexture = true;
    }
    else if  (command == "KeyBack")
    {
      temp = line.AfterFirst(':');
      long key;
      if (temp.ToLong( &key)) KeyBack = key;
      else KeyBack = -1;
    }
    else if  (command == "KeySave")
    {
      temp = line.AfterFirst(':');
      long key;
      if (temp.ToLong( &key)) KeySave = key;
      else KeySave = -1;
    }
    else if  (command == "KeyNext")
    {
      temp = line.AfterFirst(':');
      long key;
      if (temp.ToLong( &key)) KeyNext = key;
      else KeyNext = -1;
    }
    else if (command == "UseSizeFilter")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myUseSizeFilter = false;
      else myUseSizeFilter = true;
    }
    else if  (command == "Height")
    {
      line = line.AfterFirst(':');
      temp = line.Before(',');
      long val;
      if (temp.ToLong( &val)) myHeightMin = val;
      else myHeightMin = 1;
      temp = line.After(',');
      if (temp.ToLong( &val)) myHeightMax = val;
      else myHeightMax = 1;
    }
    else if  (command == "Width")
    {
      line = line.AfterFirst(':');
      temp = line.Before(',');
      long val;
      if (temp.ToLong( &val)) myWidthMin = val;
      else myWidthMin = 1;
      temp = line.After(',');
      if (temp.ToLong( &val)) myWidthMax = val;
      else myWidthMax = 1;
    }
    else if  (command == "Depth")
    {
      line = line.AfterFirst(':');
      temp = line.Before(',');
      long val;
      if (temp.ToLong( &val)) myDepthMin = val;
      else myDepthMin = 1;
      temp = line.After(',');
      if (temp.ToLong( &val)) myDepthMax = val;
      else myDepthMax = 1;
    }
    else if (command == "UseFormatFilter")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myUseFormatFilter = false;
      else myUseFormatFilter = true;
    }
    else if  (command == "FormatFilter")
    {
      temp = line.AfterFirst(':');
      unsigned long val;
      if (temp.ToULong( &val)) myFormatFilter = val;
      else myFormatFilter = 0u;
    }
    else if  (command == "FileFormat")
    {
      temp = line.AfterFirst(':');
      unsigned long val;
      if (temp.ToULong( &val)) myFileFormat = val;
      else myFileFormat = 0u;
    }
    else if (command == "ShowSingleTextureString")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myShowSingleTextureString = false;
      else myShowSingleTextureString = true;
    }
    else if  (command == "FontColour")
    {
      temp = line.AfterFirst(':');

      wxStringTokenizer col( temp, ",");

      int num_col = col.CountTokens();
      for (int i=0; i<num_col && i<4; i++)
      {
        temp = col.GetNextToken();
        long colour;
        if (temp.ToLong( &colour)) FontColour[i] = colour;
      }
    }
    else if  (command == "TextureColour")
    {
      temp = line.AfterFirst(':');

      wxStringTokenizer col( temp, ",");

      int num_col = col.CountTokens();
      for (int i=0; i<num_col && i<4; i++)
      {
        temp = col.GetNextToken();
        long colour;
        if (temp.ToLong( &colour)) TextureColour[i] = colour;
      }
    }
    if (command == "ShowCollSettingsPane")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myShowCollSettingsPane = false;
      else myShowCollSettingsPane = true;
    }
    else if (command == "SupportTPF")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') mySupportTPF = false;
      else mySupportTPF = true;
    }
    else if (command == "ComputeRenderTargets")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myComputeRenderTargets = false;
      else myComputeRenderTargets = true;
    }
    else if (command == "ExtractTexturesToDisk")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myExtractTexturesToDisk = false;
      else myExtractTexturesToDisk = true;
    }
    else if (command == "DeleteExtractedTexturesOnDisk")
    {
      temp = line.AfterFirst(':');
      if (temp.Len()>0 && temp[0]=='0') myDeleteExtractedTexturesOnDisk = false;
      else myDeleteExtractedTexturesOnDisk = true;
    }
    else if (command == "ExtractPath") ExtractPath = line.AfterFirst(':');
  }
  if (SaveAllTextures && SaveSingleTexture)
  {
    SaveAllTextures = false;
    SaveSingleTexture = false;
  }
  return 0;
}



int uMod_GameInfo::SetSaveSingleTexture(bool val)
{
  SaveSingleTexture = val;
  return 0;
}

int uMod_GameInfo::SetSaveAllTextures(bool val)
{
  SaveAllTextures = val;
  return 0;
}

uMod_GameInfo& uMod_GameInfo::operator = (const  uMod_GameInfo &rhs)
{
  myShowCollCapturePane = rhs.myShowCollCapturePane;

  SaveSingleTexture = rhs.SaveSingleTexture;
  SaveAllTextures = rhs.SaveAllTextures;

  KeyBack = rhs.KeyBack;
  KeySave = rhs.KeySave;
  KeyNext = rhs.KeyNext;

  myUseSizeFilter = rhs.myUseSizeFilter;
  myHeightMin = rhs.myHeightMin;
  myHeightMax = rhs.myHeightMax;
  myWidthMin = rhs.myWidthMin;
  myWidthMax = rhs.myWidthMax;
  myDepthMin = rhs.myDepthMin;
  myDepthMax = rhs.myDepthMax;

  myUseFormatFilter = rhs.myUseFormatFilter;
  myFormatFilter = rhs.myFormatFilter;
  myFileFormat = rhs.myFileFormat;

  myShowSingleTextureString = rhs.myShowSingleTextureString;

  for (int i=0; i<4; i++) FontColour[i]=rhs.FontColour[i];
  for (int i=0; i<4; i++) TextureColour[i]=rhs.TextureColour[i];

  SavePath = rhs.SavePath;
  OpenPath = rhs.OpenPath;
  ExtractPath = rhs.ExtractPath;

  myShowCollSettingsPane = rhs.myShowCollSettingsPane;
  mySupportTPF = rhs.mySupportTPF;
  myComputeRenderTargets = rhs.myComputeRenderTargets;
  myExtractTexturesToDisk = rhs.myExtractTexturesToDisk;
  ExtractPath = rhs.ExtractPath;

  return *this;
}

