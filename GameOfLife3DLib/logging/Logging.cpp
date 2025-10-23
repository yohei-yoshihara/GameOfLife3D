
// #define SPDLOG_HEADER_ONLY 1
// #define SPDLOG_WCHAR_TO_UTF8_SUPPORT 1
// #include "spdlog/spdlog.h2"
// #include "spdlog/sinks/basic_file_sink.h"
#include "Logging.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#include "stdafx.h"

namespace logging {

std::shared_ptr<spdlog::logger> logger;

void InitLogger(const wchar_t *logFileName) {
  logger = spdlog::basic_logger_mt("basic_logger", logFileName);
  spdlog::set_default_logger(logger);
#ifdef _DEBUG
  spdlog::set_level(spdlog::level::debug);
#else
  spdlog::set_level(spdlog::level::info);
#endif
  spdlog::debug("start logging");
}

}  // namespace logging