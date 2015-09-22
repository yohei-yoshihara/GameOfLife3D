#ifndef GRAPHICS_DECORATION_FRAME_H_
#define GRAPHICS_DECORATION_FRAME_H_

namespace graphics {
  namespace decorator {
    class Frame {
    public:
      enum BRUSH_REFERENCE_TYPE {
        BRUSH_REFERENCE_TYPE_BY_INDEX,
        BRUSH_REFERENCE_TYPE_BY_LABEL,
        BRUSH_REFERENCE_TYPE_AUTO_INDEX,
      };

    private:
      FLOAT m_thick;
      BRUSH_REFERENCE_TYPE m_brushReferenceType;
      size_t m_brushIndex;
      std::wstring m_brushLabel;

    public:
      Frame(void)
          : m_brushReferenceType(BRUSH_REFERENCE_TYPE_BY_INDEX)
          , m_brushIndex(0)
          , m_brushLabel()
          , m_thick(1.0f) {
      }
      Frame(size_t brushIndex, FLOAT thick = 1.0)
          : m_brushReferenceType(BRUSH_REFERENCE_TYPE_BY_INDEX)
          , m_brushIndex(brushIndex)
          , m_brushLabel()
          , m_thick(thick) {
      }
      Frame(const std::wstring &brushLabel, FLOAT thick = 1.0)
          : m_brushReferenceType(BRUSH_REFERENCE_TYPE_BY_LABEL)
          , m_brushIndex()
          , m_brushLabel(brushLabel)
          , m_thick(thick) {
      }
      Frame(const Frame &other)
          : m_brushReferenceType(other.m_brushReferenceType)
          , m_brushIndex(other.m_brushIndex)
          , m_brushLabel(other.m_brushLabel)
          , m_thick(other.m_thick) {
      }
      ~Frame(void) {
      }
      Frame &operator=(const Frame &other) {
        if (this != &other) {
          m_brushReferenceType = other.m_brushReferenceType;
          m_brushIndex = other.m_brushIndex;
          m_brushLabel = other.m_brushLabel;
          m_thick = other.m_thick;
        }
        return *this;
      }
      bool operator==(const Frame &other) const {
        if (m_brushReferenceType == other.m_brushReferenceType) {
          switch (m_brushReferenceType) {
          case BRUSH_REFERENCE_TYPE_BY_INDEX:
            return m_brushIndex == other.m_brushIndex && m_thick == other.m_thick;
            break;
          case BRUSH_REFERENCE_TYPE_BY_LABEL:
            return m_brushLabel == other.m_brushLabel && m_thick == other.m_thick;
            break;
          }
        }
        return false;
      }
      bool operator!=(const Frame &other) const {
        return !(*this == other);
      }
      void SetBrushReferenceType(BRUSH_REFERENCE_TYPE brushReferenceType) {
        m_brushReferenceType = brushReferenceType;
      }
      BRUSH_REFERENCE_TYPE GetBrushReferenceType() const {
        return m_brushReferenceType;
      }
      void SetBrushIndex(size_t brushIndex) {
        m_brushIndex = brushIndex;
      }
      size_t GetBrushIndex() const {
        return m_brushIndex;
      }
      void SetBrushLabel(const std::wstring &brushLabel) {
        m_brushLabel = brushLabel;
      }
      const std::wstring &GetBrushLabel() const {
        return m_brushLabel;
      }
      void SetThick(FLOAT thick) {
        m_thick = thick;
      }
      FLOAT GetThick() const {
        return m_thick;
      }
      friend std::wostream &operator<<(std::wostream &os, const graphics::decorator::Frame &frame) {
        switch (frame.m_brushReferenceType) {
        case graphics::decorator::Frame::BRUSH_REFERENCE_TYPE_BY_INDEX:
          os << typeid(frame).name() << L"[brushReferenceType=byIndex,brushIndex=" << frame.m_brushIndex << L",thick="
             << frame.m_thick << L"]";
          break;
        case graphics::decorator::Frame::BRUSH_REFERENCE_TYPE_BY_LABEL:
          os << typeid(frame).name() << L"[brushReferenceType=byLabel,brushLabel=" << frame.m_brushLabel << L",thick="
             << frame.m_thick << L"]";
          break;
        }
        return os;
      }
    };
  }
}

#endif // GRAPHICS_DECORATION_FRAME_H_