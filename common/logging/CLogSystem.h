#ifndef COMMON_LOGGING_CLOGSYSTEM_H
#define COMMON_LOGGING_CLOGSYSTEM_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/file_sinks.h>

#include "LogDefs.h"

namespace spdlog
{
namespace sinks
{
class sink;
}
}

namespace logging
{
/**
*	@brief Manages logger access & creation, logging settings
*/
class CLogSystem final
{
private:
	enum class State
	{
		UNINITIALIZED = 0,
		ACTIVE
	};

public:
	CLogSystem();
	~CLogSystem();

	bool Initialize();

	void Shutdown();

	std::shared_ptr<spdlog::logger> CreateBasicLogger( const std::string& logger_name, const spdlog::filename_t& filename, bool truncate = false );

	std::shared_ptr<spdlog::logger> CreateRotatingLogger( const std::string& logger_name, const spdlog::filename_t& filename, size_t max_file_size, size_t max_files );

	std::shared_ptr<spdlog::logger> CreateDailyLogger( const std::string& logger_name, const spdlog::filename_t& filename, int hour = 0, int minute = 0 );

	/**
	*	@brief Creates a logger that outputs to the console only
	*/
	std::shared_ptr<spdlog::logger> CreateConsoleLogger( const std::string& logger_name );

	/**
	*	@brief Creates a logger that outputs to the console and optionally to a log file, based on user configuration
	*/
	std::shared_ptr<spdlog::logger> CreateMultiLogger( const std::string& logger_name );

	/**
	*	@brief Creates a null logger
	*	Prefer using the global null logger when possible
	*/
	std::shared_ptr<spdlog::logger> CreateNullLogger( const std::string& logger_name );

	std::shared_ptr<spdlog::logger> GetLogger( const std::string& szLoggerName );

	/**
	*	@brief Drops the given logger, it is no longer managed by the system and should no longer be used
	*/
	void DropLogger( const std::shared_ptr<spdlog::logger>& logger );

	spdlog::level::level_enum GetLogLevel( const std::string& szLoggerName ) const;

	bool SetLogLevel( spdlog::logger& logger, spdlog::level::level_enum level );

	bool SetLogLevel( spdlog::logger& logger, const char* pszLogLevel );

	bool SetLogLevel( const std::string& szLoggerName, spdlog::level::level_enum level );

	bool SetLogLevel( const std::string& szLoggerName, const char* pszLogLevel );

	const std::string& GetBasePath() const { return m_szBasePath; }

	/**
	*	@brief Sets the base path. This path is prepended to all log file names
	*/
	void SetBasePath( std::string&& szPath );

	bool IsLogToFileEnabled() const;

	void SetLogToFile( const bool bState );

	void EnableLogToFile();

	void DisableLogToFile();

private:
	void Command_LogLevel();

	void Command_LogToFile();

	void Command_ListLoggers();

	void Command_TestLogger();

	void LogErrorHandler( const std::string& szErrorMessage );

	/*
	*	@brief Prepares a filename as needed so that loggers create files in the local game directory
	*/
	std::string PrepareFilename( const std::string& szFilename ) const;

	std::shared_ptr<LOGGING_FACTORY( spdlog::sinks::daily_file_sink )> CreateLogSink( const std::string& szBaseName );

private:
	std::string m_szBasePath;

	State m_State = State::UNINITIALIZED;

	//Used for loggers that log to nothing (loggers should always have at least one sink)
	std::shared_ptr<spdlog::sinks::sink> m_NullSink;
	//Used for console debug output
	std::shared_ptr<spdlog::sinks::sink> m_DebugSink;
	//Used for regular console output
	std::shared_ptr<spdlog::sinks::sink> m_ConsoleSink;
	//The sink for log file output. May be null if logging to file is disabled
	std::shared_ptr<spdlog::sinks::sink> m_LogFileSink;

	//Sink used to add/remove the log file sink to all loggers that care about it
	std::shared_ptr<LOGGING_FACTORY( spdlog::sinks::dist_sink )> m_LogDistSink;

private:
	CLogSystem( const CLogSystem& ) = delete;
	CLogSystem& operator=( const CLogSystem& ) = delete;
};

CLogSystem& LogSystem();
}

#endif //COMMON_LOGGING_CLOGSYSTEM_H
