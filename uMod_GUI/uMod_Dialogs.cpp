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

uMod_Acknowledgement::uMod_Acknowledgement(wxWindow *parent, int id)
: wxDialog(parent, id, Language->MenuAcknowledgement)
{
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxString msg;

  msg << "\nRS for coding the original TexMod and for information about the used hashing algorithm\n\n";

  msg << "Vergil for the uMod logo and useful hints\n";
  msg << "(lustfulillumination.wordpress.com)\n\n";

  msg << "EvilAlex for translation into Russian and bug fixing\n";
  msg << "ReRRemi for translation into French\n";
  msg << "mirHL for transaltion into Italian\n";
  wxTextCtrl *text = new wxTextCtrl( this, wxID_ANY, msg, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_MULTILINE|wxTE_NO_VSCROLL|wxTE_RICH2);
  sizer->Add( (wxWindow*) text, 1, wxEXPAND, 0);

  wxSizer *button_sizer = CreateSeparatedButtonSizer( wxOK );
  if (button_sizer)
      sizer->Add( button_sizer, 0, wxALL|wxGROW, 5 );

  SetIcon( ((wxFrame*) parent)->GetIcon());
  SetSizer( sizer );
}

