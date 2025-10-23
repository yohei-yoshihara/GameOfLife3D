#ifndef ANIMATION_IANIMATIONWINDOW_H_
#define ANIMATION_IANIMATIONWINDOW_H_

namespace animation {
  class IAnimationWindow {
  public:
    virtual HRESULT Invalidate() = 0;
  };
}

#endif // ANIMATION_IANIMATIONWINDOW_H_