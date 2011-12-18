#include "stdafx.h"
#include "animation/IAnimationWindow.h"
#include "animation/AnimationManagerEventHandler.h"

COM_DECLSPEC_NOTHROW HRESULT animation::AnimationManagerEventHandler::CreateInstance(
    std::weak_ptr<animation::IAnimationWindow> animationWindow,
    IUIAnimationManagerEventHandler** ppManagerEventHandler)
{
    AnimationManagerEventHandler* pAnimationManagerEventHandler;
    HRESULT hr = CUIAnimationCallbackBase::CreateInstance(
                     ppManagerEventHandler,
                     &pAnimationManagerEventHandler);
    if (SUCCEEDED(hr)) {
        pAnimationManagerEventHandler->m_animationWindow = animationWindow;
    }
    return hr;
}

animation::AnimationManagerEventHandler::AnimationManagerEventHandler() :
    m_animationWindow()
{
}

animation::AnimationManagerEventHandler::~AnimationManagerEventHandler()
{
}

HRESULT animation::AnimationManagerEventHandler::OnManagerStatusChanged(
    UI_ANIMATION_MANAGER_STATUS newStatus,
    UI_ANIMATION_MANAGER_STATUS previousStatus)
{
    UNREFERENCED_PARAMETER(previousStatus);
    HRESULT hr = S_OK;

    if (newStatus == UI_ANIMATION_MANAGER_BUSY) {
        if (!m_animationWindow.expired()) {
            hr = m_animationWindow.lock()->Invalidate();
        }
    }

    return hr;
}
