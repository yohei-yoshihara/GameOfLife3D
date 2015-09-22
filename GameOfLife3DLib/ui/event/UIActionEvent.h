#ifndef UI_EVENT_UIACTIONEVENT_H_
#define UI_EVENT_UIACTIONEVENT_H_

namespace ui {
  namespace event {
    // ActionEvent / ActionListener
    class UIActionEvent {
    private:
      std::weak_ptr<UIBase> m_source;
      std::wstring m_actionCommand;

    public:
      UIActionEvent(std::weak_ptr<UIBase> source, const std::wstring &actionCommand)
          : m_source(source)
          , m_actionCommand(actionCommand) {
      }
      std::weak_ptr<UIBase> GetSource() {
        return m_source;
      }
      const std::wstring &GetActionCommand() {
        return m_actionCommand;
      }
    };
    typedef std::function<void(UIActionEvent)> UIActionListenerFunctor;
  }
}

#endif // UI_EVENT_UIACTIONEVENT_H_