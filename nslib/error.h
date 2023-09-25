/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
 *
 * Authors: Pablo Toharia <pablo.toharia@upm.es>
 *
 * This file is part of NeuroScheme
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#ifndef __NSLIB_ERROR__
#define __NSLIB_ERROR__


#include "Loggers.h"
#include <stdexcept>
#include <exception>
#include <iostream>
#include <typeinfo>



#define NEUROSCHEME_THROW( msg )                                             \
  {                                                                          \
    nslib::Loggers::get( )->log( std::string( msg ), nslib::LOG_LEVEL_ERROR, \
                                 NEUROSCHEME_FILE_LINE );                    \
    throw std::runtime_error( msg );                                         \
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

#endif // __NSLIB_ERROR__
