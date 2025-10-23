#ifndef UI_EVENT_EVENTSUPPORT_H_
#define UI_EVENT_EVENTSUPPORT_H_

namespace ui {
  namespace event {
    template <typename ListenerFunctor, typename Event>
    class EventSupport {
      std::vector<ListenerFunctor> m_listeners;

    public:
      EventSupport()
          : m_listeners() {
      }
      void AddListener(ListenerFunctor listener) {
        m_listeners.push_back(listener);
      }
      ListenerFunctor GetListener(size_t index) const {
        return m_listeners.at(index);
      }
      size_t GetNumberOfListener() const {
        return m_listeners.size();
      }
      void FireEvent(Event event) {
        for (auto it = m_listeners.begin(); it != m_listeners.end(); ++it) {
          (*it)(event);
        }
      }
    };
  }
}

#endif // UI_EVENT_EVENTSUPPORT_H_
