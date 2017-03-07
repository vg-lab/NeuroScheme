/**
 * @file    Log.h
 * @brief   Class to handle log errors
 * @author  Pablo Toharia <pablo.toharia@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */
#ifndef __NSLIB_LOG__
#define __NSLIB_LOG__

#include <nslib/api.h>
#include <iostream>

namespace nslib
{

#define NS_LOG_HEADER                                           \
  std::string( "NeuroScheme: " +                                \
               std::string( __FILE__ ) + std::string( "(" ) +   \
               std::to_string( __LINE__ ) +                     \
               std::string( ")::" ) +                           \
               std::string( __FUNCTION__ ) +                    \
               std::string( ": " ))


  //! Log levels suported
  typedef enum
  {
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_VERBOSE,
    LOG_LEVEL_UNKNOWN
  } TLogLevel;

  /**
   * Static class to log output
   *
   */
  class Log
  {
  public:

    /**
     * Set current log level
     * @param level level to be set
     */
    NSLIB_API
    static void setLogLevel( const TLogLevel logLevel );

    /**
     * Get current log level
     * @return current log level
     */
    NSLIB_API
    static TLogLevel logLevel( void );

    /**
     * Set current output stream
     * @param stream output stream to be set
     */
    NSLIB_API
    static void setStream( std::ostream& stream );

    /**
     * Get current output stream
     * @return current output stream
     */
    NSLIB_API
    static const std::ostream& stream( void );

    /**
     * Method to log out a message in a specific level
     * @param msg message to log
     * @param level level of the message
     */
    NSLIB_API
    static void log( const std::string& msg,
                     const TLogLevel level = LOG_LEVEL_VERBOSE );

    /**
     * Method to log out a message in a specific level to a specific stream
     * @param msg message to log
     * @param level level of the message
     * @param stream stream where log will be written
     */
    NSLIB_API
    static void log( const std::string& msg,
                     std::ostream& stream,
                     const TLogLevel level = LOG_LEVEL_VERBOSE );

  protected:

    //! Current log level. This can be overwritten with proper value
    //! assigned to environment variable NEUROSCHEME_LOG_LEVEL
    static TLogLevel _logLevel;

    //! Current stream to write log to
    static std::ostream* _stream;

  };
}

#endif
