#include "stdafx.h"
#include "Logging.h"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <string>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <boost/config.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/attribute_value_impl.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/core/null_deleter.hpp>

namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

//#define LOG_LOCALE "English"
//#define LOG_LOCALE "Japanese"
#define LOG_LOCALE "en_US.UTF-8"

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", SEVERITY_LEVEL)

namespace logging {
  std::wstring getTimestamp() {
    SYSTEMTIME systemTime;
    GetLocalTime(&systemTime);
    std::wstringstream ss;
    ss << systemTime.wYear << L"/" << std::setfill(L'0') << std::setw(2) << systemTime.wMonth << L"/"
       << std::setfill(L'0') << std::setw(2) << systemTime.wDay << L" " << std::setfill(L'0') << std::setw(2)
       << systemTime.wHour << L":" << std::setfill(L'0') << std::setw(2) << systemTime.wMinute << L":"
       << std::setfill(L'0') << std::setw(2) << systemTime.wSecond << L"." << std::setfill(L'0') << std::setw(3)
       << systemTime.wMilliseconds;
    return ss.str();
  }

  class SystemTimestampImpl : public boost::log::attribute::impl {
  public:
    boost::log::attribute_value get_value() {
      return attrs::make_attribute_value(getTimestamp());
    }
  };

  class SystemTimestamp : public boost::log::attribute {
  public:
    SystemTimestamp()
        : boost::log::attribute(new SystemTimestampImpl()) {
    }
    explicit SystemTimestamp(attrs::cast_source const &source)
        : boost::log::attribute(source.as<SystemTimestampImpl>()) {
    }
  };

  void InitLogger(const wchar_t *logFileName) {
    _wsetlocale(LC_ALL, _T(LOG_LOCALE));
    std::wofstream *wof(new std::wofstream());
    wof->imbue(std::locale(LOG_LOCALE, LC_ALL));
    wof->open(logFileName /*, std::ios::app*/);
    boost::shared_ptr<std::wofstream> strm(wof);
    if (!strm->good()) {
      throw std::runtime_error("Failed to open a text log file");
    }

    // Create a text file sink
    auto sink = boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::wtext_ostream_backend>>();

    sink->locked_backend()->add_stream(strm);

#ifdef DEBUG
    sink->set_filter(severity >= SEVERITY_LEVEL_DEBUG);
    sink->locked_backend()->auto_flush(true);
#else
    sink->set_filter(severity >= SEVERITY_LEVEL_INFO);
#endif

    sink->set_formatter(expr::stream << expr::attr<std::wstring>("SystemTimestamp") << L" ["
                                     << expr::attr<SEVERITY_LEVEL>("Severity") << L"] " << expr::wmessage);

    // Add it to the core
    boost::log::core::get()->add_sink(sink);

    // Add some attributes too
    boost::shared_ptr<boost::log::core> core = boost::log::core::get();
    core->add_global_attribute("SystemTimestamp", SystemTimestamp());
  }
}