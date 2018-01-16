#ifndef COMMON_LOGGING_CCONSOLELOGSINK_H
#define COMMON_LOGGING_CCONSOLELOGSINK_H

#include <spdlog/sinks/base_sink.h>

#include "extdll.h"
#include "util.h"

namespace logging
{
/**
*	@brief spdlog sink to log to the console
*	Logs critical to info to the console at all times, logs debug and trace only if developer mode is enabled
*/
template<typename MUTEX>
class CConsoleLogSink : public spdlog::sinks::base_sink<MUTEX>
{
public:
	CConsoleLogSink() = default;
	~CConsoleLogSink() = default;

protected:
	void _sink_it( const spdlog::details::log_msg& msg ) override
	{
		if( msg.level >= spdlog::level::info )
		{
			Con_Printf( "%s", msg.formatted.c_str() );
		}
		else
		{
			Con_DPrintf( "%s", msg.formatted.c_str() );
		}
	}

	void _flush() override {}
};
}

#endif //COMMON_LOGGING_CCONSOLELOGSINK_H
