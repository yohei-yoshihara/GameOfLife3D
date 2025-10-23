#include "stdafx.h"

#include "Logging.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

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