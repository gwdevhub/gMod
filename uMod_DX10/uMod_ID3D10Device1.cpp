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



#define UMOD_ID3D10DEVICE_H_

#include "..\uMod_DXMain\uMod_Main.h"
#include "uMod_ID3D10Device1.h"

#define uMod_ID3D10Device uMod_ID3D10Device1
#define ID3D10Device ID3D10Device1
#define m_ID3D10Device m_ID3D10Device1

//#define RETURN_QueryInterface 0x01000001L
#define PRE_MESSAGE "uMod_ID3D10Device1"
#define DEF_DX_VERSION VERSION_DX101


#include "uMod_ID3D10Device.cpp"



HRESULT STDMETHODCALLTYPE uMod_ID3D10Device1::CreateShaderResourceView1( ID3D10Resource *pResource, const D3D10_SHADER_RESOURCE_VIEW_DESC1 *pDesc, ID3D10ShaderResourceView1 **ppSRView)
{
  return m_ID3D10Device1->CreateShaderResourceView1( pResource, pDesc, ppSRView);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device1::CreateBlendState1( const D3D10_BLEND_DESC1 *pBlendStateDesc, ID3D10BlendState1 **ppBlendState)
{
  return m_ID3D10Device1->CreateBlendState1( pBlendStateDesc, ppBlendState);
}

D3D10_FEATURE_LEVEL1 STDMETHODCALLTYPE uMod_ID3D10Device1::GetFeatureLevel(void)
{
  return m_ID3D10Device1->GetFeatureLevel();
}


