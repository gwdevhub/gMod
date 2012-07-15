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

#ifndef __CDT_PARSER__

BEGIN_EVENT_TABLE(uMod_SpinPanel, wxPanel)
EVT_SPINCTRL(ID_SpinMin, uMod_SpinPanel::OnSpin)
EVT_SPINCTRL(ID_SpinMax, uMod_SpinPanel::OnSpin)
END_EVENT_TABLE()

#endif


uMod_SpinPanel::uMod_SpinPanel(wxWindow *parent, int  winid,
              const wxPoint& pos, const wxSize& size,
              long style, const wxString& name)
  :  wxPanel( parent, winid, pos, size, style, name)
{

  Text = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);

  Min = new wxSpinCtrl ( this, ID_SpinMin, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 65535, 1);

  Max = new wxSpinCtrl ( this, ID_SpinMax, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 65535, 1);
  Max->SetRange( 1, 65535);

  wxBoxSizer* spin_sizer = new wxBoxSizer(wxHORIZONTAL);
  spin_sizer->Add( (wxWindow*) Text, 0, wxEXPAND, 0);

  spin_sizer->Add( (wxWindow*) Min, 0, wxEXPAND, 0);
  spin_sizer->Add( (wxWindow*) new wxTextCtrl(this, wxID_ANY, "  -", wxDefaultPosition, wxSize(25,10), wxTE_READONLY), 0, wxEXPAND, 0);
  spin_sizer->Add( (wxWindow*) Max, 0, wxEXPAND, 0);
  spin_sizer->AddStretchSpacer();

  SetSizer(spin_sizer);

}


void uMod_SpinPanel::GetValue( int &min, int &max)
{
  min = Min->GetValue();
  max = Max->GetValue();
}

void uMod_SpinPanel::SetValue(int min, int max)
{
  if (max<min) return;
  Min->SetValue( min);
  Max->SetValue( max);
}


void uMod_SpinPanel::SetLabel( const wxString &label)
{
  Text->SetValue( label);
}


void uMod_SpinPanel::OnSpin(wxSpinEvent& event)
{
  int min = Min->GetValue();
  int max = Max->GetValue();

  if (event.GetId() == ID_SpinMin)
  {
    if (max<min) Max->SetValue(min);
  }
  else
  {
    if (min>max) Min->SetValue(max);
  }
}





uMod_CheckBoxArray::uMod_CheckBoxArray(wxWindow *parent, const wxArrayString &choices, int col, int  winid,
    const wxPoint& pos, const wxSize& size,
    long style, const wxString& name)
:  wxPanel( parent, winid, pos, size, style, name)
{
  if (col>0)
  {
    wxGridSizer *sizer = new wxGridSizer( col, 0,0);

    for (unsigned int i=0; i<choices.GetCount(); i++)
    {
      wxCheckBox *check_box = new wxCheckBox ( this, wxID_ANY, choices[i], wxDefaultPosition, wxDefaultSize);
      CheckBoxes.Add(check_box);
      sizer->Add(check_box);
    }
    SetSizer(sizer);
  }
  else
  {
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    for (unsigned int i=0; i<choices.GetCount(); i++)
    {
      wxCheckBox *check_box = new wxCheckBox ( this, wxID_ANY, choices[i], wxDefaultPosition, wxDefaultSize);
      CheckBoxes.Add(check_box);
      sizer->Add(check_box, 0, wxEXPAND, 0);
    }
    sizer->AddStretchSpacer();
    SetSizer(sizer);
  }
}

void uMod_CheckBoxArray::SetChoices(const wxArrayShort &choices)
{
  for (unsigned int i=0; i<choices.GetCount() && i<CheckBoxes.GetCount(); i++)
  {
    if (choices[i]) CheckBoxes[i]->SetValue(true);
    else CheckBoxes[i]->SetValue(false);
  }
}

void uMod_CheckBoxArray::GetChoices(wxArrayShort &choices)
{
  choices.Empty();
  for (unsigned int i=0; i<CheckBoxes.GetCount(); i++)
  {
    if (CheckBoxes[i]->GetValue()) choices.Add(1);
    else choices.Add(0);
  }
}


void uMod_CheckBoxArray::SetValue( unsigned long val)
{
  for (unsigned int i=0; i<CheckBoxes.GetCount(); i++)
  {
    if (val & 1<<i) CheckBoxes[i]->SetValue(true);
    else  CheckBoxes[i]->SetValue(false);
  }
}

void uMod_CheckBoxArray::GetValue( unsigned long &val)
{
  val = 0;
  for (unsigned int i=0; i<CheckBoxes.GetCount(); i++)
  {
    if (CheckBoxes[i]->GetValue()) val |= 1<<i;
  }
}






#ifndef __CDT_PARSER__

BEGIN_EVENT_TABLE(uMod_KeyPanel, wxPanel)
EVT_BUTTON(ID_KeyPress, uMod_KeyPanel::OnButton)
END_EVENT_TABLE()

#endif


uMod_KeyPanel::uMod_KeyPanel(wxWindow *parent, const wxString &label, int key, int  winid,
      const wxPoint& pos, const wxSize& size, long style, const wxString& name)
:  wxPanel( parent, winid, pos, size, style, name)
{
  myKey = key;

  Button = new wxButton( this, ID_KeyPress, label, wxDefaultPosition, wxSize(100,24));

  KeyText = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);

  wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add( (wxWindow*) Button, 0, wxEXPAND, 0);
  sizer->AddSpacer(10);
  sizer->Add( (wxWindow*) KeyText, 0, wxEXPAND, 0);
  sizer->AddStretchSpacer();

  SetKeyLabel();
  SetSizer(sizer);
}

void uMod_KeyPanel::SetLabel(const wxString &label)
{
  Button->SetLabel( label);
}

void uMod_KeyPanel::OnButton(wxCommandEvent& WXUNUSED(event))
{
  uMod_AskForKey *dialog = new uMod_AskForKey( Language->AskForKey);
  dialog->ShowModal();
  myKey = dialog->Key();
  delete dialog;

  SetKeyLabel();
}


void uMod_KeyPanel::SetKeyLabel()
{
  wxString temp;
  if (myKey<0)
    temp = Language->KeyNotSet;
  else
  {
    int index = Language->KeyValues.Index( myKey);
    if (index>=0)
      temp = Language->KeyStrings[index];
    else
      temp = Language->UnknownKey;

    wxString code;
    code.Printf(" (%#X)", myKey);
    temp << code;
  }
  KeyText->SetValue(temp);
}





#ifndef __CDT_PARSER__

BEGIN_EVENT_TABLE(uMod_AskForKey, wxDialog)
//EVT_KEY_UP(uMod_AskForKey::OnKey)
EVT_CHAR_HOOK(uMod_AskForKey::OnKey)
END_EVENT_TABLE()

#endif

uMod_AskForKey::uMod_AskForKey(const wxString &title)
: wxDialog( (wxWindow*)0, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER)
{
  myKey = -1;
}

void uMod_AskForKey::OnKey(wxKeyEvent& event)
{
  myKey = event.GetRawKeyCode();//GetKeyCode();
  EndModal(0);
}

/*

uMod_TextureFormat::uMod_TextureFormat(wxWindow *parent, const wxString &title)
  : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER)
{
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  wxArrayString choices;

  choices.Add("R8G8B8");
  choices.Add("A8R8G8B8");
  choices.Add("X8R8G8B8");
  choices.Add("R5G6B");
  choices.Add("X1R5G5B5");
  choices.Add("A1R5G5B5");
  choices.Add("A4R4G4B4");
  choices.Add("R3G3B2");
  choices.Add("A8");
  choices.Add("A8R3G3B2");
  choices.Add("X4R4G4B4");
  choices.Add("A2B10G10R10");
  choices.Add("A8B8G8R8");
  choices.Add("X8B8G8R8");
  choices.Add("G16R16");
  choices.Add("A2R10G10B10");
  choices.Add("A16B16G16R16");

  choices.Add("A8P8");
  choices.Add("P8");

  choices.Add("L8");
  choices.Add("A8L8");
  choices.Add("A4L4");

  choices.Add("V8U8");
  choices.Add("L6V5U5");
  choices.Add("X8L8V8U8");
  choices.Add("Q8W8V8U8");
  choices.Add("V16U16");
  choices.Add("A2W10V10U10");

  choices.Add("UYVY");
  choices.Add("R8G8_B8G8");
  choices.Add("YUY2");
  choices.Add("G8R8_G8B8");
  choices.Add("DXT1");
  choices.Add("DXT2");
  choices.Add("DXT3");
  choices.Add("DXT4");
  choices.Add("DXT5");

  choices.Add("D16_LOCKABLE");
  choices.Add("D32");
  choices.Add("D15S1");
  choices.Add("D24S8");
  choices.Add("D24X8");
  choices.Add("D24X4S4");
  choices.Add("D16");

  choices.Add("D32F_LOCKABLE");
  choices.Add("D24FS8");

  choices.Add("D32_LOCKABLE");
  choices.Add("S8_LOCKABLE");

  choices.Add("L16");

  choices.Add("VERTEXDATA");
  choices.Add("INDEX16");
  choices.Add("INDEX32");

  choices.Add("Q16W16V16U16");

  choices.Add("MULTI2_ARGB8");

  choices.Add("R16F");
  choices.Add("G16R16F");
  choices.Add("A16B16G16R16F");

  choices.Add("R32F");
  choices.Add("G32R32F");
  choices.Add("A32B32G32R32F");

  choices.Add("CxV8U8");

  choices.Add("A1");
  choices.Add("A2B10G10R10_XR_BIAS");
  choices.Add("BINARYBUFFER");

  TextureFormats = new uMod_CheckBoxArray( this, choices, 4);
  sizer->Add( (wxWindow*) TextureFormats, 0, wxEXPAND, 0);

  SetSizer(sizer);
  wxArrayInt selections;
  wxGetSelectedChoices( selections, Language->DeleteGame, Language->DeleteGame, choices);
}
*/
