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



#ifndef uMod_TREEVIEW_H_
#define uMod_TREEVIEW_H_

#include "wx/dataview.h"
#include "uMod_ModElement.h"

class uMod_TreeViewNode;
WX_DEFINE_ARRAY_PTR( uMod_TreeViewNode*, uMod_TreeViewNode_ArrayPtr );

class uMod_TreeViewNode
{
public:
  uMod_TreeViewNode( uMod_TreeViewNode* parent, uMod_ModElement *element) : myParent(parent), myElement(element)
  {
    myElement->AddRef();
  }

  ~uMod_TreeViewNode()
  {
    // free all our children nodes
    size_t count = myChildren.GetCount();
    for (size_t i = 0; i < count; i++)
    {
      uMod_TreeViewNode *child = myChildren[i];
      delete child;
    }
    myElement->Release();
  }

  bool IsContainer() const  {if (myElement->Type() == uMod_ModElement::Group) return true; else return false;}

  uMod_TreeViewNode* GetParent() {return myParent;}

  uMod_TreeViewNode_ArrayPtr& GetChildren() {return myChildren;}

  uMod_TreeViewNode* GetNthChild( unsigned int n ) {return myChildren.Item( n);}

  void Insert( uMod_TreeViewNode* child, unsigned int n) {myChildren.Insert( child, n);}

  void Append( uMod_TreeViewNode* child )  {myChildren.Add( child );}

  unsigned int GetChildCount() const {return myChildren.GetCount();}

public:     // public to avoid getters/setters
  uMod_ModElement *myElement;

//private:
  uMod_TreeViewNode* myParent;
  uMod_TreeViewNode_ArrayPtr myChildren;
};



class uMod_TreeViewModel : public wxDataViewModel
{
public:
  uMod_TreeViewModel() : SingleTextureNode((uMod_TreeViewNode*)0) {}
  virtual ~uMod_TreeViewModel() {}

  // implementation of base class virtuals to define model

  virtual unsigned int GetColumnCount() const
  {
    return 5;
  }

  virtual wxString GetColumnType( unsigned int col ) const
  {
    (void) col;
    return wxT("string");
  }

  virtual void GetValue( wxVariant &variant,
                         const wxDataViewItem &item, unsigned int col ) const;
  virtual bool SetValue( const wxVariant &variant,
                         const wxDataViewItem &item, unsigned int col );

  virtual bool IsEnabled( const wxDataViewItem &item,
                          unsigned int col ) const;

  virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
  virtual bool HasContainerColumns( const wxDataViewItem &item ) const;
  virtual bool IsContainer( const wxDataViewItem &item ) const;
  virtual unsigned int GetChildren( const wxDataViewItem &parent,
                                    wxDataViewItemArray &array ) const;


  // implementation of uMod functions
  int CreateSingleNode(void);

  int AddPackage(const wxString &file_name, const wxString &extract_path);
  int AddPackages(const wxArrayString &files, const wxString &extract_path);
  int AddPackagesFromTemplate(const wxArrayString &packages, const wxString &extract_path);

  int DeletePackage( const wxDataViewItem &item, bool delete_files_on_disk = false);
  int DeletePackages( const wxDataViewItemArray &item,  bool delete_files_on_disk = false);
  int DeleteAllPackages( bool delete_files_on_disk = false);
  int DeleteFileOfPackage(uMod_TreeViewNode *node);

  int GetActiveTexture( uMod_TextureElement_SortedArrayPtr &texture);

  static int String2Node( const wxString &input,  uMod_TreeViewNode* node, uMod_TreeViewNode_ArrayPtr *list_node = (uMod_TreeViewNode_ArrayPtr*)0);

  static int Node2String( wxString &out, uMod_TreeViewNode* node);

//  static bool IsSameNod(uMod_TreeViewNode* a, uMod_TreeViewNode* b);

  static wxString LastError;

//private:
  uMod_TreeViewNode *SingleTextureNode;
  uMod_TreeViewNode_ArrayPtr myPackages;

};

#endif /* uMod_TREEVIEW_H_ */
