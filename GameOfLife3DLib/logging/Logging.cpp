#include "stdafx.h"
#include "Logging.h"

//#define LOG_LOCALE "English"
//#define LOG_LOCALE "Japanese"
#define LOG_LOCALE "en_US.UTF-8"

namespace logging
{
void InitLogger(const wchar_t* logFileName)
{
    _wsetlocale(LC_ALL, _T(LOG_LOCALE));
    std::wofstream *wof(new std::wofstream());
    wof->imbue(std::locale(LOG_LOCALE, LC_ALL));
    wof->open(logFileName /*, std::ios::app*/);
    boost::shared_ptr< std::wofstream > strm(wof);
    if (!strm->good()) {
        throw std::runtime_error("Failed to open a text log file");
    }

    // Create a text file sink
    boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::wtext_ostream_backend>>
            sink(new boost::log::sinks::synchronous_sink< boost::log::sinks::wtext_ostream_backend >);

    sink->locked_backend()->add_stream(strm);

#ifdef DEBUG
    sink->set_filter(
        boost::log::filters::attr< SEVERITY_LEVEL >(L"Severity", std::nothrow) >= SEVERITY_LEVEL_DEBUG
    );
    sink->locked_backend()->auto_flush(true);
#else
    sink->set_filter(
        boost::log::filters::attr< SEVERITY_LEVEL >(L"Severity", std::nothrow) >= SEVERITY_LEVEL_INFO
    );
#endif
    sink->locked_backend()->set_formatter(
        boost::log::formatters::format(L"%1% [%2%] <%3%> %4%")
        % boost::log::formatters::date_time(L"TimeStamp", std::nothrow)
        % boost::log::formatters::time_duration(L"Uptime", std::nothrow)
        % boost::log::formatters::attr< SEVERITY_LEVEL >(L"Severity", std::nothrow)
        % boost::log::formatters::wmessage()
    );

    // Add it to the core
    boost::log::wcore::get()->add_sink(sink);

    // Add some attributes too
    boost::shared_ptr< boost::log::attribute > attr(new boost::log::attributes::local_clock);
    boost::log::wcore::get()->add_global_attribute(L"TimeStamp", attr);
    boost::log::wcore::get()->add_global_attribute(
        L"Uptime",
        boost::make_shared< boost::log::attributes::timer >());
}
}