#include "Log.h"

#include <string>
#include <iostream>

#include <neuroscheme/defines.h>


namespace neuroscheme
{

  TLogLevel Log::_logLevel =
#ifdef _DEBUG
    LOG_LEVEL_VERBOSE;
#else
    LOG_LEVEL_ERROR;
#endif

  std::ostream* Log::_stream = &std::cerr;


  TLogLevel Log::logLevel( void )
  {
    return _logLevel;
  }

  void Log::setLogLevel( const TLogLevel logLevel_ )
  {
    _logLevel = logLevel_;
  }

  void Log::setStream( std::ostream& stream_ )
  {
    _stream = &stream_;
  }

  const std::ostream& Log::stream( void )
  {
    return *_stream;
  }

#ifdef NEUROSCHEME_WITH_LOGGING

  void Log::log( const std::string& msg,
                 const TLogLevel level )
  {
    Log::log( msg, *_stream, level );
  }

  TLogLevel stringToLogLevel( std::string logLevelString )
  {
  if ( logLevelString == "WARNING" )
    return LOG_LEVEL_WARNING;

  if ( logLevelString == "ERROR" )
    return LOG_LEVEL_ERROR;

  if ( logLevelString == "VERBOSE" )
    return LOG_LEVEL_VERBOSE;

  std::cerr << "neuroscheme: Unknown log level" << std::endl;
  return LOG_LEVEL_UNKNOWN;
}


  void Log::log( const std::string& msg,
                 std::ostream& stream,
                 const TLogLevel level )
  {
    TLogLevel currentLogLevel = _logLevel;

    char* envLogLevelString = getenv ( "NEUROSCHEME_LOG_LEVEL" );

    if ( envLogLevelString )
    {
      auto envLogLevel = stringToLogLevel( envLogLevelString );

      if ( envLogLevel != LOG_LEVEL_UNKNOWN )
        currentLogLevel = envLogLevel;
    }

    if ( currentLogLevel >= level )
    {
      stream << std::boolalpha
             << msg << std::endl;
    }
  }


#else
  void Log::log( const std::string&,
                 std::ostream&,
                 const TLogLevel )
  {
  }

  void Log::log( const std::string&,
                 const TLogLevel )
  {
  }
#endif



} // namespace neuroscheme
