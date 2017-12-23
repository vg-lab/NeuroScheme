/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
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
#ifndef __NSLIB_LOGGERS__
#define __NSLIB_LOGGERS__

#include <nslib/api.h>
#include <nslib/Logger.hpp>
#include <assert.h>

namespace nslib
{
  class Loggers
  {
  public:

    static Logger* get( unsigned int idx = 0 )
    {
      return _loggers.at( idx );
    }

    static void add( Logger* logger, unsigned int idx = 0 )
    {
      _loggers[ idx ] = logger;
    }

  protected:
    NSLIB_API static std::unordered_map< unsigned int, Logger* > _loggers;
  };
}
#endif
