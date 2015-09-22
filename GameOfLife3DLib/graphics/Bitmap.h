#ifndef GRAPHICS_BITMAP_H_
#define GRAPHICS_BITMAP_H_

namespace graphics {
  class Bitmap : public std::enable_shared_from_this<Bitmap> {
  private:
    // file name
    LPCTSTR m_fileName;
    // resource name and type
    LPCTSTR m_resourceName;
    LPCTSTR m_resourceType;

    CComPtr<ID2D1Bitmap> m_bitmap;

  public:
    Bitmap(void);
    virtual ~Bitmap(void);
    void SetFileName(LPCTSTR fileName) {
      m_fileName = fileName;
      m_resourceName = nullptr;
      m_resourceType = nullptr;
    }
    LPCTSTR GetFileName() const {
      return m_fileName;
    }
    void SetResource(LPCTSTR resourceName, LPCTSTR resourceType) {
      m_resourceName = resourceName;
      m_resourceType = resourceType;
      m_fileName = nullptr;
    }
    LPCTSTR GetResourceName() const {
      return m_resourceName;
    }
    LPCTSTR GetResourceType() const {
      return m_resourceType;
    }
    bool Check();
    HRESULT Load(ID2D1RenderTarget *pRenderTarget, IWICImagingFactory *pIWICFactory);
    bool IsLoaded() const {
      return m_bitmap != nullptr;
    }
    ID2D1Bitmap *GetBitmap() {
      return m_bitmap;
    }
  };
}

#endif // GRAPHICS_BITMAP_H_
