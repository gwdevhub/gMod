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



#ifndef uMod_GAMEPAGE_H_
#define uMod_GAMEPAGE_H_
#include "uMod_Main.h"

// this page is opened if a game is started.
class uMod_GamePage : public wxPanel
{
public:
  uMod_GamePage( wxNotebook *parent, const wxString &exe, int injection_method, const wxString &save, PipeStruct &pipe);
  virtual ~uMod_GamePage(void);

  int UpdateGame(void);
  int ReloadGame(void);

  int SaveTemplate( const wxString &file_name);
  int SaveTemplateToString( wxString &content);

  int LoadTemplate( const wxString &file_name);
  int LoadTemplateFromString( const wxString &content);

  wxString GetExeName(void) {return ExeName;}
  wxString GetTemplateName(void) {return TemplateName;}

  int SetOpenPath(const wxString &path) {return Game.SetOpenPath(path);}
  wxString GetOpenPath(void) {return Game.GetOpenPath();}

  int SetSavePath(const wxString &path);
  wxString GetSavePath(void) {return Game.GetSavePath();}

  /**
   * User started drag
   * @param event
   */
  void OnBeginDrag( wxDataViewEvent &event);
  /**
   * Ask if drop position is valid
   * @param event
   */
  void OnDropPossible( wxDataViewEvent &event);
  /**
   * User dropped the item
   * @param event
   */
  void OnDrop( wxDataViewEvent &event);

  void OnDropFile(wxDropFilesEvent& event);

  /**
   * Gets called, when the capture texture panel is shown/hidden
   * @param WXUNUSED
   */
  void OnCollPane(wxCollapsiblePaneEvent& WXUNUSED(event));

  /**
   * Gets called, when the colour buttons are pressed
   * @param event
   */
  void OnButtonColour(wxCommandEvent& event);

  /**
   * Gets called, when one of the check boxes gets checked/unchecked
   * @param event
   */
  void OnCheckBox(wxCommandEvent& event);

  /**
   * Gets called, press the button to select the filtered texture formats.
   * @param event
   */
  void OnButtonFormatFilter(wxCommandEvent& event);

  /**
   * Gets called, when the save-path button is pressed
   * @param event
   */
  void OnButtonSavePath(wxCommandEvent& event);


  /**
   * Gets called by a rigth click in the ViewCtrl
   * @param event
   */
  void OnContextMenu( wxDataViewEvent &event );
  int OpenPackage(void);


  /**
   * call to change the labels of all items to the actual language
   * @return
   */
  int UpdateLanguage(void);

  /**
   * get the actual name of the page (displayed in the tab of the notebook window)
   * @return
   */
  wxString GetPageName(void);

  /**
   * The game has created a DirectX device.
   * @param version device version (e.g. DX9, DX9Ex, DX10 DX10.1
   * @return
   */
  int AddDXDevice(int version);
  /**
   * The game hast destroyed a DirectX device.
   * @param version
   * @return
   */
  int RemoveDXDevice(int version);


  wxString LastError;

private:

  /**
   * Set the info text (which injection method is used and which kind of DirectX device is connected)
   * @return
   */
  int SetInfo(void);

  const int InjectionMethod;
  int CounterDX9, CounterDX9EX;
  int CounterDX10, CounterDX101;

  /**
   * add a single package (or file)
   * @param file_name
   * @return
   */
  int AddPackage( const wxString &file_name);

  /**
   * adds multiple packages (or files)
   * @param files
   * @param num
   * @return
   */
  int AddPackages(const wxString *files, int num);

  /**
   * adds multiple packages (or files)
   * @param files
   * @return
   */
  int AddPackages(const wxArrayString &files);

  /**
   * ask for all values from the various control elements of this panel and updates \a Game
   * @return
   */
  int GetSettings(void);


  wxString ExeName;
  wxString TemplateName;



  wxBoxSizer *MainSizer;
  wxTextCtrl *DX_DLL_Info;
  wxTextCtrl *TemplateFile;

  wxCollapsiblePane *CollPane;
  wxBoxSizer *CollSizer;

  wxBoxSizer *TextureSizer;

  wxPanel *SingleTexturePanel;
  wxBoxSizer *SingleTextureSizer;
  wxCheckBox *SaveSingleTexture;
  wxCheckBox *ShowSingleTextureString;


  uMod_KeyPanel *KeyBack;
  uMod_KeyPanel *KeySave;
  uMod_KeyPanel *KeyNext;

  wxButton *FontColour;
  wxButton *TextureColour;


  wxPanel *AllTexturePanel;
  wxBoxSizer *AllTextureSizer;
  wxCheckBox *SaveAllTextures;

  wxCheckBox *UseSizeFilter;

  uMod_SpinPanel *SpinWidth;
  uMod_SpinPanel *SpinHeight;
  uMod_SpinPanel *SpinDepth;


  wxCheckBox *UseFormatFilter;
  wxButton *FormatFilter;


  uMod_CheckBoxArray *FileFormats;
  wxButton *SavePathButton;
  wxTextCtrl *SavePath;

  wxDataViewCtrl *ViewCtrl;
  uMod_TreeViewModel *ViewModel;
  wxDataViewItem *LastDataViewItem;

  int NumberOfEntry;
  int MaxNumberOfEntry;


  wxArrayString Files;
  uMod_GameInfo Game;
  uMod_GameInfo GameOld;

  uMod_Sender Sender;


  DECLARE_EVENT_TABLE();
};

#endif /* uMod_GAMEPAGE_H_ */
