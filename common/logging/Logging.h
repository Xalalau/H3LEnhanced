#ifndef COMMON_LOGGING_LOGGING_H
#define COMMON_LOGGING_LOGGING_H

#include <spdlog/logger.h>

namespace logging
{
/**
*	@brief Logger that outputs to nothing
*/
extern std::shared_ptr<spdlog::logger> null_logger;

/**
*	@brief Logger that outputs to the console only
*/
extern std::shared_ptr<spdlog::logger> con;

/**
*	@brief Default logger to use for information that should be logged to console, debug log, and user configured log file
*/
extern std::shared_ptr<spdlog::logger> log;
}

#endif //COMMON_LOGGING_LOGGING_H
