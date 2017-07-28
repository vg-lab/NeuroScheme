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
#ifndef __NSLIB__CONNECTIVITY_REP__
#define __NSLIB__CONNECTIVITY_REP__

#include <scoop/scoop.h>

namespace nslib
{
  class ConnectivityRep
  {
  public:
    virtual ~ConnectivityRep( void ) {}
    virtual void highlight( const scoop::Color& color = scoop::Color( 255, 0, 0 ))
    { ( void ) color; }
    virtual void unHighlight( void ) {}
  };
}
#endif
