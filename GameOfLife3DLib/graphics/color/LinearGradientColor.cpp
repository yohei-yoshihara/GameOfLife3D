#include "stdafx.h"
#include "graphics/color/IColor.h"
#include "graphics/color/LinearGradientColor.h"

graphics::color::LinearGradientColor::LinearGradientColor(void)
    : m_gradientDirection(GRADIENT_DIRECTION_TOP_BOTTOM)
    , m_originalStopCollection()
    , m_currentStopCollection() {
}

graphics::color::LinearGradientColor::~LinearGradientColor(void) {
}

void graphics::color::LinearGradientColor::Brighter(FLOAT rate) {
  for (size_t i = 0; i < m_currentStopCollection.GetNumberOfStops(); ++i) {
    m_currentStopCollection.GetGradientStop(i).GetColorValue().Brighter(rate);
  }
}

void graphics::color::LinearGradientColor::Darker(FLOAT rate) {
  for (size_t i = 0; i < m_currentStopCollection.GetNumberOfStops(); ++i) {
    m_currentStopCollection.GetGradientStop(i).GetColorValue().Darker(rate);
  }
}

void graphics::color::LinearGradientColor::SetAlpha(FLOAT alpha) {
  for (size_t i = 0; i < m_currentStopCollection.GetNumberOfStops(); ++i) {
    m_currentStopCollection.GetGradientStop(i).GetColorValue().SetAlpha(alpha);
  }
}

void graphics::color::LinearGradientColor::Reset() {
  m_currentStopCollection = m_originalStopCollection;
}
