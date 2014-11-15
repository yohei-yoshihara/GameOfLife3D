#ifndef LOGGING_LOGGING_H_
#define LOGGING_LOGGING_H_

// Boost Log
#define BOOST_LOG_USE_WCHAR_T
#pragma warning(push)
#pragma warning(disable: 4100)
#pragma warning(disable: 4244)
#pragma warning(disable: 4251)
#pragma warning(disable: 4634)
#pragma warning(disable: 4714)
#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#define _AFX_SECURE_NO_WARNINGS
#include <boost/log/core.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/record_ordering.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#undef _AFX_SECURE_NO_WARNINGS
#undef _CRT_NONSTDC_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#pragma warning(pop)

namespace logging
{
extern void InitLogger(const wchar_t* logFileName);
}

#define LOG(sev) BOOST_LOG_SEV(logger::get(),sev) << __FUNCTION__ << L"(" << __LINE__ << L") - "
#define LOG_HRESULT(sev, hr) BOOST_LOG_SEV(logger::get(),sev) << __FUNCTION__ << L"(" << __LINE__ << L") - " << L"HRESULT = " << std::hex << hr << std::dec << L", "
#define LOG_ENTER(sev) BOOST_LOG_SEV(logger::get(),sev) << __FUNCTION__ << L"(" << __LINE__ << L") <ENTER> - "
#define LOG_LEAVE(sev) BOOST_LOG_SEV(logger::get(),sev) << __FUNCTION__ << L"(" << __LINE__ << L") <LEAVE> - "
#define LOG_LEAVE_HRESULT(sev, hr) BOOST_LOG_SEV(logger::get(),sev) << __FUNCTION__ << L"(" << __LINE__ << L") <LEAVE> - " << std::hex << L"HRESULT = " << hr << std::dec

enum SEVERITY_LEVEL {
    SEVERITY_LEVEL_DEBUG_L3,
    SEVERITY_LEVEL_DEBUG_L2,
    SEVERITY_LEVEL_DEBUG,
    SEVERITY_LEVEL_INFO,
    SEVERITY_LEVEL_WARN,
    SEVERITY_LEVEL_ERROR,
    SEVERITY_LEVEL_FATAL
};

inline std::wostream& operator<< (std::wostream& strm, SEVERITY_LEVEL const& val)
{
    switch (val) {
    case SEVERITY_LEVEL_DEBUG_L3:
        strm << L"DDD";
        break;
    case SEVERITY_LEVEL_DEBUG_L2:
        strm << L"DD";
        break;
    case SEVERITY_LEVEL_DEBUG:
        strm << L'D';
        break;
    case SEVERITY_LEVEL_INFO:
        strm << L'I';
        break;
    case SEVERITY_LEVEL_WARN:
        strm << L'W';
        break;
    case SEVERITY_LEVEL_ERROR:
        strm << L'E';
        break;
    case SEVERITY_LEVEL_FATAL:
        strm << L'F';
        break;
    }
    return strm;
}

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger, boost::log::sources::wseverity_logger< SEVERITY_LEVEL >);

#endif // LOGGING_LOGGING_H_