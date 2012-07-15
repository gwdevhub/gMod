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

wxString uMod_TreeViewModel::LastError = wxString();


void uMod_TreeViewModel::GetValue( wxVariant &variant,
                                 const wxDataViewItem &item, unsigned int col ) const
{
  wxASSERT(item.IsOk());

  uMod_TreeViewNode *node = (uMod_TreeViewNode*) item.GetID();
  switch (col)
  {
  case 0:
    if (node->myElement->Title().IsEmpty()) variant = node->myElement->File();
    else variant = node->myElement->Title();
    break;
  case 1:
    variant = (bool) (node->myElement->Status()==uMod_ModElement::Activate);
    break;
  case 2:
    variant = (bool) (node->myElement->Status()==uMod_ModElement::Deactivate);
    break;
  case 3:
    variant = node->myElement->Author();
    break;
  case 4:
    variant = node->myElement->Comment();
    break;

  default:
    break;
  }
}

bool uMod_TreeViewModel::SetValue( const wxVariant &variant,
                                 const wxDataViewItem &item, unsigned int col )
{
  wxASSERT(item.IsOk());
  if (col<1 || col>2) return false;

  uMod_TreeViewNode *node = (uMod_TreeViewNode*) item.GetID();
  bool set;
  variant.Convert( &set);

  uMod_ModElement::enumStatus status = uMod_ModElement::NONE;
  if (set)
  {
    switch (col)
    {
    case 1:
      status = uMod_ModElement::Activate;
      break;
    case 2:
      status = uMod_ModElement::Deactivate;
      break;
  default:
    return false;
    break;
    }
    node->myElement->Status() = status;
  }
  else
  {
    if (node->myElement->Type() == uMod_ModElement::Group)
    {
      node->myElement->Status() = uMod_ModElement::NONE;
    }
  }

  return true;
}

bool uMod_TreeViewModel::IsEnabled( const wxDataViewItem &item,
                                  unsigned int col ) const
{
  //only the toggle are enabled
  if (col<1 || col >3) return false;

  uMod_TreeViewNode *node = (uMod_TreeViewNode*) item.GetID();

  //an item is only enabled if no one of the parents is set.
  uMod_TreeViewNode *parent = node->GetParent();
  while (parent != (uMod_TreeViewNode*)0)
  {
    if (parent->myElement->Status()!=uMod_ModElement::NONE) return false;
    parent = parent->GetParent();
  }
  return true;
}

wxDataViewItem uMod_TreeViewModel::GetParent( const wxDataViewItem &item ) const
{
  // the invisible root node has no parent
  if (!item.IsOk())
      return wxDataViewItem(0);

  uMod_TreeViewNode *node = (uMod_TreeViewNode*) item.GetID();
  return wxDataViewItem( (void*) node->GetParent() );
}

bool uMod_TreeViewModel::HasContainerColumns( const wxDataViewItem &WXUNUSED(item) ) const
{
  return true;
}

bool uMod_TreeViewModel::IsContainer( const wxDataViewItem &item ) const
{
    // the invisble root node can have children
    if (!item.IsOk())
        return true;

    uMod_TreeViewNode *node = (uMod_TreeViewNode*) item.GetID();
    return node->IsContainer();
}

unsigned int uMod_TreeViewModel::GetChildren( const wxDataViewItem &parent,
                                            wxDataViewItemArray &array ) const
{
  uMod_TreeViewNode *node = (uMod_TreeViewNode*) parent.GetID();
  if (!node)
  {

    unsigned int count = myPackages.GetCount();
    for (unsigned int pos = 0; pos < count; pos++)
    {
      uMod_TreeViewNode *child = myPackages.Item( pos );
      array.Add( wxDataViewItem( (void*) child ) );
    }
    return count;
  }

  if (node->GetChildCount() == 0)
  {
    return 0;
  }

  unsigned int count = node->GetChildren().GetCount();
  for (unsigned int pos = 0; pos < count; pos++)
  {
    uMod_TreeViewNode *child = node->GetChildren().Item( pos );
    array.Add( wxDataViewItem( (void*) child ) );
  }
  return count;
}

int uMod_TreeViewModel::CreateSingleNode()
{
  if (SingleTextureNode ==(uMod_TreeViewNode*)0)
  {
    uMod_ModElement *package = new uMod_ModElement(uMod_ModElement::Group, uMod_ModElement::Activate);

    SingleTextureNode = new uMod_TreeViewNode( (uMod_TreeViewNode*)0, package);
    package->Release();
    package->Title() = Language->SingleTextureNode;
    myPackages.Add( SingleTextureNode);
    wxDataViewItem child( (void*) SingleTextureNode );
    wxDataViewItem parent( (void*) 0 );
    ItemAdded( parent, child );
  }
  return 0;
}

int uMod_TreeViewModel::AddPackage(const wxString &file_name)
{
  uMod_File file( file_name);
  if (! file.FileSupported())
  {
    LastError << Language->Error_FileNotSupported << "\n" << file_name;
    return -1;
  }
  bool single_file = file.SingleFile();

  uMod_TreeViewNode *node;

  if (single_file)
  {
    CreateSingleNode();
    node = SingleTextureNode;
  }
  else
  {
    uMod_ModElement *package = new uMod_ModElement(uMod_ModElement::Group, uMod_ModElement::Activate);

    node = new uMod_TreeViewNode( (uMod_TreeViewNode*)0, package);
    package->Release();
    package->File() = file_name;
  }

  if (int ret = file.GetContent(node))
  {
    LastError = file.LastError;
    if (node != SingleTextureNode) delete node;
    return ret;
  }

  if (node != SingleTextureNode)
  {
    myPackages.Add( node);

    wxDataViewItem child( (void*) node );
    wxDataViewItem parent( (void*) 0 );
    ItemAdded( parent, child );
  }
  else if (SingleTextureNode != (uMod_TreeViewNode*)0)
  {
    wxDataViewItem child( (void*) SingleTextureNode->myChildren.Last() );
    wxDataViewItem parent( SingleTextureNode );
    ItemAdded( parent, child );
  }

  return 0;
}

int uMod_TreeViewModel::AddPackages(const wxArrayString &files)
{
  LastError.Empty();
  uMod_File file;
  wxDataViewItemArray added_items;
  wxDataViewItemArray added_single_items;
  /*
  bool single_file = false;

  // detect if there are more than one single file
  for (unsigned int i=0; i<files.GetCount(); i++)
  {
    file.SetFile( files[i]);
    if (file.SingleFile()) {single_file = true;
  }
  */

/*
  if (single_files>1)
  {
    uMod_ModElement *element = new uMod_ModElement(uMod_ModElement::Group, uMod_ModElement::NONE);
    element->Title() = Language->MultipleSingleFiles;
    single_file_node = new uMod_TreeViewNode( (uMod_TreeViewNode*)0, element);
    element->Release();
  }
*/

  for (unsigned int i=0; i<files.GetCount(); i++)
  {
    file.SetFile( files[i]);

    if (! file.FileSupported())
    {
      LastError << Language->Error_FileNotSupported << "\n" << files[i] << "\n" ;
      continue;
    }

    if (file.SingleFile())
    {
      CreateSingleNode();
      if (file.GetContent(SingleTextureNode))
      {
        LastError << file.LastError ;
        file.LastError.Empty();
        continue;
      }
      wxDataViewItem child( (void*) SingleTextureNode->myChildren.Last() );
      added_single_items.Add( child);
    }
    else
    {
      uMod_ModElement *package = new uMod_ModElement(uMod_ModElement::Group, uMod_ModElement::Activate);
      uMod_TreeViewNode *node = new uMod_TreeViewNode( (uMod_TreeViewNode*)0, package);
      package->Release();
      package->File() = files[i];

      if (file.GetContent(node))
      {
        LastError <<  file.LastError;
        delete node;
        file.LastError.Empty();
        continue;
      }
      myPackages.Add( node);

      wxDataViewItem child( (void*) node );
      added_items.Add( child);
    }
  }

  if (added_items.GetCount()>0)
  {
    wxDataViewItem parent( (void*) 0 );
    ItemsAdded( parent, added_items);
  }

  if (added_single_items.GetCount()>0)
  {
    wxDataViewItem parent( SingleTextureNode );
    ItemsAdded( parent, added_single_items);
  }

  if (LastError.IsEmpty()) return 0;
  else return -1;
}


int uMod_TreeViewModel::AddPackagesFromTemplate(const wxArrayString &packages)
{
  LastError.Empty();

  uMod_File file;
  wxDataViewItemArray added_items;

  DeleteAllPackages();

  for (unsigned int i=0; i<packages.GetCount(); i++)
  {
    uMod_ModElement *element = new uMod_ModElement(uMod_ModElement::Group, uMod_ModElement::Activate);
    uMod_TreeViewNode *node = new uMod_TreeViewNode( (uMod_TreeViewNode*)0, element);
    element->Release();

    if (file.GetContentTemplate( packages[i], node))
    {
      LastError << file.LastError;
      file.LastError.Empty();
      delete node;
      continue;
    }

    myPackages.Add( node);

    wxDataViewItem child( (void*) node );
    added_items.Add( child);

    if (node->myElement->File().IsEmpty())
    {
      SingleTextureNode = node;
      SingleTextureNode->myElement->Title() = Language->SingleTextureNode;
    }
  }

  if (added_items.GetCount()>0)
  {
    wxDataViewItem parent( (void*) 0 );
    ItemsAdded( parent, added_items);
  }

  if (LastError.IsEmpty()) return 0;
  else return -1;
}


int uMod_TreeViewModel::DeletePackage( const wxDataViewItem &item)
{
  uMod_TreeViewNode *node = (uMod_TreeViewNode*) item.GetID();
  if (node)
  {
    if (node->GetParent()== (uMod_TreeViewNode*) 0 )
    {
      myPackages.Remove( node);
      delete node;
      if (node == SingleTextureNode) SingleTextureNode = (uMod_TreeViewNode*) 0;


      wxDataViewItem parent( (void*) 0 );
      ItemDeleted( parent, item );
      return 0;
    }
    return 1;
  }
  return 2;
}

int uMod_TreeViewModel::DeletePackages( const wxDataViewItemArray &items)
{
  int num = items.GetCount();
  wxDataViewItemArray deleted_items;
  for (int i=0; i<num; i++)
  {
    uMod_TreeViewNode *node = (uMod_TreeViewNode*) items[i].GetID();
    if (node)
    {
      if (node->GetParent()== (uMod_TreeViewNode*) 0 )
      {
        myPackages.Remove( node);
        delete node;
        if (node == SingleTextureNode) SingleTextureNode = (uMod_TreeViewNode*) 0;
        deleted_items.Add( items[i]);
      }
    }
  }
  wxDataViewItem parent( (void*) 0 );
  ItemsDeleted( parent, deleted_items);
  return 0;
}

int uMod_TreeViewModel::DeleteAllPackages(void)
{
  wxDataViewItemArray deleted_items;
  for (unsigned int i=0; i<myPackages.GetCount(); i++)
  {
    wxDataViewItem child( myPackages[i]);
    deleted_items.Add( child);
  }
  SingleTextureNode = (uMod_TreeViewNode*) 0;
  wxDataViewItem parent( (void*) 0 );
  ItemsDeleted( parent, deleted_items);
  myPackages.Empty();
  return 0;
}

int uMod_TreeViewModel::GetActiveTexture( uMod_TextureElement_SortedArrayPtr &texture)
{
  texture.Empty();

  uMod_TreeViewNode_ArrayPtr buffer; // this array store the "path" to the current group node (parent0->parent1->parent2->current_group)
  wxArrayInt counter; // this array store the actual index position for each group in the hierarchy
  wxArrayInt active; // this array is need for the inherited active status of parent groups

  for (unsigned int p=0; p<myPackages.GetCount(); p++)
  {
    if (myPackages.Item(p)->myElement->Status()==uMod_ModElement::Deactivate) continue;



    if (myPackages.Item(p)->myElement->Type() == uMod_ModElement::Group)
    {
      buffer.Add(myPackages.Item(p));
      counter.Add(0);
      if (myPackages.Item(p)->myElement->Status()==uMod_ModElement::Activate) active.Add(1);
      else active.Add(0);

      while (!buffer.IsEmpty())
      {
        int pos = buffer.GetCount()-1;
        unsigned int index = counter[pos];
        if (index < buffer[pos]->myChildren.GetCount()) // it has still childrens
        {
          if (buffer[pos]->myChildren[index]->myElement->Type() == uMod_ModElement::Group)
          {
            if ((active[pos]>0)  || (buffer[pos]->myChildren[index]->myElement->Status() != uMod_ModElement::Deactivate))
            {
              buffer.Add(buffer[pos]->myChildren[index]);
              counter.Add(0);
              if (active[pos]>0 || buffer[pos]->myChildren[index]->myElement->Status()==uMod_ModElement::Activate) active.Add(1);
              else  active.Add(0);
            }
          }
          else if (buffer[pos]->myChildren[index]->myElement->Type() == uMod_ModElement::Texture)
          {
            uMod_TextureElement *tex = (uMod_TextureElement*) buffer[pos]->myChildren[index]->myElement;

            if (tex->Content().Len()>0 && ((active[pos]>0)  || (tex->Status()==uMod_ModElement::Activate)))
            {
              if( wxNOT_FOUND == texture.Index(tex))
              {
                texture.Add(tex);
                tex->AddRef();
              }
            }
          }

          counter[pos]++;
        }
        else // finished this group
        {
          buffer.RemoveAt(pos);
          counter.RemoveAt(pos);
          active.RemoveAt(pos);
        }
      }
    }
    else if (myPackages.Item(p)->myElement->Type()==uMod_ModElement::Texture)
    {
      uMod_TextureElement *tex = (uMod_TextureElement*) myPackages.Item(p)->myElement;
      if (tex->Status()==uMod_ModElement::Activate)
        if (wxNOT_FOUND == texture.Index(tex))
        {
         texture.Add(tex);
         tex->AddRef();
        }
    }
  }

  return 0;
}

int uMod_TreeViewModel::String2Node( const wxString &input,  uMod_TreeViewNode* node, uMod_TreeViewNode_ArrayPtr *list_node)
{
  if (list_node!=(uMod_TreeViewNode_ArrayPtr*)0) list_node->Empty();

  wxStringTokenizer token( input, "\n");
  int num = token.CountTokens();

  bool found_start = false;

  wxString entry;

  wxString command;
  wxString file;
  wxString action;
  wxString hash;
  uMod_TreeViewNode_ArrayPtr buffer;
  buffer.Add(node);

  for (int i=0; i<num; i++)
  {
    entry = token.GetNextToken();
    if (entry.Len()==0) continue;
    if (entry[0]=='#') continue;
    if (entry[0]=='!') continue;

    wxStringTokenizer entry_token( entry, "|", wxTOKEN_RET_EMPTY_ALL);

    command = entry_token.GetNextToken().Trim(true).Trim(false);

    if (!found_start)
    {
      if (command=="PACKAGE")
      {
        found_start = true;

        node->myElement->File() = entry_token.GetNextToken().Trim(true).Trim(false);
        node->myElement->Title() = entry_token.GetNextToken().Trim(true).Trim(false);
        action  = entry_token.GetNextToken().Trim(true).Trim(false);
        if (action=="D") node->myElement->Status() = uMod_ModElement::Deactivate;
        else if (action=="A") node->myElement->Status() = uMod_ModElement::Activate;
        else node->myElement->Status() = uMod_ModElement::NONE;
        node->myElement->Author()  = entry_token.GetNextToken().Trim(true).Trim(false);
        node->myElement->Comment()  = entry_token.GetNextToken().Trim(true).Trim(false);
      }
      continue;
    }

    if (command=="BEGIN_GROUP")
    {
      uMod_ModElement *element = new uMod_ModElement(uMod_ModElement::Group, uMod_ModElement::NONE);
      uMod_TreeViewNode *node_group = new uMod_TreeViewNode(buffer.Last(), element);

      element->Title() = entry_token.GetNextToken().Trim(true).Trim(false);
      action  = entry_token.GetNextToken().Trim(true).Trim(false);
      if (action=="D") element->Status() = uMod_ModElement::Deactivate;
      else if (action=="A") element->Status() = uMod_ModElement::Activate;
      else element->Status() = uMod_ModElement::NONE;
      element->Author()  = entry_token.GetNextToken().Trim(true).Trim(false);
      element->Comment()  = entry_token.GetNextToken().Trim(true).Trim(false);


      if (list_node != (uMod_TreeViewNode_ArrayPtr*)0) list_node->Add(node_group);

      // add node to the last group
      buffer.Last()->Append( node_group);
      element->Release();

      //set this group to the last group
      buffer.Add( node_group);
    }
    else if (command=="END_GROUP")
    {
      if (buffer.GetCount()==0)
      {
        LastError << "There is an END_GROUP without a previous BEGIN_GROUP";
        return -1;
      }
      buffer.RemoveAt(buffer.GetCount()-1);
    }
    else if (command=="TEXTURE")
    {
      if (buffer.GetCount()==0) continue;

      file = entry_token.GetNextToken().Trim(true).Trim(false);
      hash = entry_token.GetNextToken().Trim(true).Trim(false);

      if (file.IsEmpty())
        continue;
      if (hash.IsEmpty())
        continue;

      DWORD64 temp_hash;
      if (!hash.ToULongLong( &temp_hash, 16))
      {
        LastError << Language->Error_Hash << "\n";
        continue;
      }

      uMod_TextureElement *texture = new uMod_TextureElement();

      texture->File() = file;
      texture->Hash() = temp_hash;

      texture->Title() = entry_token.GetNextToken().Trim(true).Trim(false);
      action  = entry_token.GetNextToken().Trim(true).Trim(false);
      if (action=="D") texture->Status() = uMod_ModElement::Deactivate;
      else texture->Status() = uMod_TextureElement::Activate;
      texture->Author()  = entry_token.GetNextToken().Trim(true).Trim(false);
      texture->Comment()  = entry_token.GetNextToken().Trim(true).Trim(false);

      uMod_TreeViewNode *node_texture = new uMod_TreeViewNode( buffer.Last(), texture);

      if (list_node != (uMod_TreeViewNode_ArrayPtr*)0) list_node->Add(node_texture);

      // add node to the last group
      buffer.Last()->Append( node_texture);
      texture->Release();
    }
  }

  if (!found_start)
  {
    LastError = "Did not find 'PACKAGE' directive\n";
    return -1;
  }
  return 0;
}



int uMod_TreeViewModel::Node2String( wxString &out, uMod_TreeViewNode* node)
{
  if (node==(uMod_TreeViewNode*)0) return -1;
  out.Empty();
  uMod_TreeViewNode_ArrayPtr buffer; // this array store the "path" to the current group node (parent0->parent1->parent2->current_group)
  wxArrayInt counter; // this array store the actual index position for each group in the hierarchy

  if (node->myElement->Type() != uMod_ModElement::Group) return -1;

  out << "\nPACKAGE|" << node->myElement->File() << "|";
  out << node->myElement->Title() << "|";
  if (node->myElement->Status() == uMod_ModElement::Activate) out << "A";
  else if (node->myElement->Status() == uMod_ModElement::Deactivate) out << "D";
  out << "|" << node->myElement->Author();
  out << "|" << node->myElement->Comment() << "\n\n";

  buffer.Add( node);
  counter.Add(0);
  /*

  if (node->myElement->Type() == uMod_ModElement::Group)
  {
    buffer.Add(node);
    counter.Add(0);

    out << "\nBEGIN_GROUP|"<<node->myElement->Title() << "|";
    if (node->myElement->Status() == uMod_ModElement::Activate) out << "A";
    else if (node->myElement->Status() == uMod_ModElement::Deactivate) out << "D";
    out << "|" << node->myElement->Author();
    out << "|" << node->myElement->Comment();
    out << "\n\n";
*/

    while (!buffer.IsEmpty())
    {
      int pos = buffer.GetCount()-1;
      unsigned int index = counter[pos];
      if (index < buffer[pos]->myChildren.GetCount()) // it has still childrens
      {
        if (buffer[pos]->myChildren[index]->myElement->Type() == uMod_ModElement::Group)
        {
          uMod_TreeViewNode* node_child = buffer[pos]->myChildren[index];

          out << "\nBEGIN_GROUP|"<< node_child->myElement->Title() << "|";
          if (node_child->myElement->Status() == uMod_ModElement::Activate) out << "A";
          else if (node_child->myElement->Status() == uMod_ModElement::Deactivate) out << "D";
          out << "|" << node_child->myElement->Author();
          out << "|" << node_child->myElement->Comment();
          out << "\n";

          buffer.Add(node_child);
          counter.Add(0);
        }
        else if (buffer[pos]->myChildren[index]->myElement->Type() == uMod_ModElement::Texture)
        {
          uMod_TextureElement *tex = (uMod_TextureElement*) buffer[pos]->myChildren[index]->myElement;
          out << "TEXTURE|"<<  tex->File() << "|" ;
          out << wxString::Format("%#llX", tex->Hash()) << "|";
          out << tex->Title() << "|";
          if (tex->Status() == uMod_ModElement::Deactivate) out << "D";
          out << "|" << tex->Author();
          out << "|" << tex->Comment();
          out << "\n";
        }

        counter[pos]++;
      }
      else // finished this group
      {
        buffer.RemoveAt(pos);
        counter.RemoveAt(pos);

        if (buffer.GetCount()>0) out << "END_GROUP\n";
      }
    }
    /*
  }
  else if (node->myElement->Type()==uMod_ModElement::Texture)
  {
    uMod_TextureElement *tex = (uMod_TextureElement*) node->myElement;

    out << "TEXTURE|"<<  tex->File() << "|" ;
    out << wxString::Format("%#lX", tex->Hash()) << "|";
    out << tex->Title() << "|";
    if (tex->Status() == uMod_ModElement::Deactivate) out << "D";
    out << "|" << tex->Author();
    out << "|" << tex->Comment();
    out << "\n\n";

  }
*/
  return 0;
}

/*
bool uMod_TreeViewModel::IsSameNod(uMod_TreeViewNode* a, uMod_TreeViewNode* b)
{
  // at first we test for object specific stats
  if (a->myElement->Type()==uMod_ModElement::Group && b->myElement->Type()==uMod_ModElement::Group)
  {
    //title must be the same
    if (a->myElement->Title() != b->myElement->Title()) return false;
  }
  else if (a->myElement->Type()==uMod_ModElement::Texture && b->myElement->Type()==uMod_ModElement::Texture)
  {
    uMod_TextureElement *tex_a = (uMod_TextureElement*) a->myElement;
    uMod_TextureElement *tex_b = (uMod_TextureElement*) b->myElement;

    //Hash, Title, and File must be the same
    if (tex_a->Hash() != tex_b->Hash()) return false;
    if (tex_a->Title() != tex_b->Title()) return false;
    if (tex_a->File() != tex_b->File()) return false;
  }
  else return false; //not the same objects


  // now we compare their parents
  // the title of their parents must be the same
  if (a->GetParent()!=(uMod_TreeViewNode*)0)
  {
    if (b->GetParent()!=(uMod_TreeViewNode*)0)
    {
      if (a->GetParent()->myElement->Title() == b->GetParent()->myElement->Title()) return true;
      else return false;
    }
    else return false;
  }
  else if (b->GetParent()!=(uMod_TreeViewNode*)0) return false;
  else return true; //both are root textures
}
*/
