#ifndef __NEUROSCHEME_ERROR__
#define __NEUROSCHEME_ERROR__


#include "Log.h"
#include <stdexcept>
#include <exception>
#include <iostream>
#include <typeinfo>



#define NEUROSCHEME_THROW( msg )                                               \
  {                                                                            \
    neuroscheme::Log::log( std::string( msg ), neuroscheme::LOG_LEVEL_ERROR ); \
    throw std::runtime_error( msg );                                           \
  }

#define NEUROSCHEME_CHECK_THROW( cond, errorMsg )   \
  {                                                 \
    if ( ! (cond) ) NEUROSCHEME_THROW( errorMsg );  \
  }

#ifdef DEBUG
#define NEUROSCHEME_DEBUG_CHECK( cond, errorMsg ) \
  {                                               \
    NEUROSCHEME_CHECK_THROW( cond, errorMsg )     \
  }
#else
#define NEUROSCHEME_DEBUG_CHECK( cond, errorMsg )
#endif

#endif // __NEUROSCHEME_ERROR__
