#ifndef LOGGING_LOGGING_H_
#define LOGGING_LOGGING_H_

#define SPDLOG_HEADER_ONLY
#define SPDLOG_WCHAR_FILENAMES
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace logging {
extern void InitLogger(const wchar_t *logFileName);
}

#endif // LOGGING_LOGGING_H_