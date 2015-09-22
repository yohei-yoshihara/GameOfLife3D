#include "StdAfx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "graphics/Bitmap.h"

graphics::Bitmap::Bitmap(void)
    : m_fileName()
    , m_resourceName()
    , m_resourceType()
    , m_bitmap(nullptr) {
}

graphics::Bitmap::~Bitmap(void) {
}

bool graphics::Bitmap::Check() {
  if (m_fileName != nullptr) {
    DWORD fileAttributes = GetFileAttributes(m_fileName);
    return !(fileAttributes == INVALID_FILE_ATTRIBUTES || (fileAttributes | FILE_ATTRIBUTE_DIRECTORY) != 0);
  } else {
    return FindResource(HINST_THISCOMPONENT, m_resourceName, m_resourceType) != nullptr;
  }
}

HRESULT graphics::Bitmap::Load(ID2D1RenderTarget *pRenderTarget, IWICImagingFactory *pIWICFactory) {
  m_bitmap = nullptr;
  if (m_fileName != nullptr) {
    return D3DInteropHelper::LoadBitmapFile(pRenderTarget, pIWICFactory, m_fileName, &m_bitmap);
  } else {
    return D3DInteropHelper::LoadResourceBitmap(pRenderTarget, pIWICFactory, m_resourceName, m_resourceType, &m_bitmap);
  }
}
