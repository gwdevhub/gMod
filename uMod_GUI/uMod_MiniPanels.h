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

#ifndef UMOD_MINIPANELS_H_
#define UMOD_MINIPANELS_H_


class uMod_SpinPanel : public wxPanel
{
public:
  uMod_SpinPanel(wxWindow *parent, int  winid = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxTAB_TRAVERSAL | wxNO_BORDER,
              const wxString& name = wxPanelNameStr);

  virtual ~uMod_SpinPanel() {}

  /**
   * Get the min an max value
   * @param[out] min
   * @param[out] max
   */
  void GetValue( int &min, int &max);

  /**
   * Set the min and max value
   * @param[in] min
   * @param[in] max
   */
  void SetValue(int min, int max);


  /**
   * Set the label of the text ctrl
   * @param label
   * @return
   */
  void SetLabel( const wxString &label);

protected:

  /**
   * Gets called, when one of the spin boxes changes their value
   * @param event
   */
  void OnSpin(wxSpinEvent& event);

  wxTextCtrl *Text;
  wxSpinCtrl *Min, *Max;

  DECLARE_EVENT_TABLE();
};


WX_DEFINE_ARRAY_PTR( wxCheckBox*, wxCheckBox_Array );

class uMod_CheckBoxArray : public wxPanel
{
public:
  uMod_CheckBoxArray(wxWindow *parent, const wxArrayString &choices, int col, int  winid = wxID_ANY,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER,
      const wxString& name = wxPanelNameStr);
  virtual ~uMod_CheckBoxArray() {}

  void SetChoices(const wxArrayShort &choices);
  void GetChoices(wxArrayShort &choices);


  void SetValue( unsigned long val);
  void GetValue( unsigned long &val);

private:
  wxCheckBox_Array CheckBoxes;
};



/**
 * this class is a mini panel with a set key button and string beside it which shows the actual set key
 */
class uMod_KeyPanel : public wxPanel
{
public:
  uMod_KeyPanel(wxWindow *parent, const wxString &label, int key, int  winid = wxID_ANY,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL | wxNO_BORDER,
      const wxString& name = wxPanelNameStr);
  virtual ~uMod_KeyPanel() {}

  int GetKey() {return myKey;}
  void SetKey(int key) {myKey = key; SetKeyLabel();}

  /**
   * set the label of the button (for changing the language)
   * @param label
   */
  void SetLabel(const wxString &label);
private:

  /**
   * called if button was clicked
   * @param event
   */
  void OnButton(wxCommandEvent& event);

  void SetKeyLabel();

  wxButton *Button;
  wxTextCtrl *KeyText;

  int myKey;
  DECLARE_EVENT_TABLE();
};


class uMod_AskForKey : public wxDialog
{
public:
  uMod_AskForKey(const wxString &title);
  virtual ~uMod_AskForKey() {}


  int Key() {return myKey;}

private:

  /**
   * called if a key was hit
   * @param event
   */
  void OnKey(wxKeyEvent& event);

  int myKey;
  DECLARE_EVENT_TABLE();
};

/*

class uMod_TextureFormat : public wxDialog
{
public:
  uMod_TextureFormat(wxWindow *parent, const wxString &title);
  virtual ~uMod_TextureFormat() {}

  void SetValue( unsigned long val) {TextureFormats->SetValue(val);}
  void GetValue( unsigned long &val) {TextureFormats->GetValue(val);}
private:

  uMod_CheckBoxArray *TextureFormats;
};

*/
#endif /* UMOD_MINIPANELS_H_ */
