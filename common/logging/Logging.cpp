#include "Logging.h"

namespace logging
{
//Pointers to common loggers
std::shared_ptr<spdlog::logger> null_logger = nullptr;
std::shared_ptr<spdlog::logger> con = nullptr;
std::shared_ptr<spdlog::logger> log = nullptr;
}
