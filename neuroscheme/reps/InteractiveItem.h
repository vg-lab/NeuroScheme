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
#ifndef __NEUROSCHEME__INTERACTIVE_ITEM__
#define __NEUROSCHEME__INTERACTIVE_ITEM__

#include "Log.h"
#include <QGraphicsItem>

namespace neuroscheme
{
  class InteractiveItem
  {
  public:

    enum
    {
      HIERARCHY_ENABLED = 0x01,
    };

    InteractiveItem( unsigned int flags_ = 0 )
      : _interactive( true )
      , _interactiveItemFlags( flags_ )
    {
    }
    virtual ~InteractiveItem( void ) { }

    bool setInteractive( bool interactive_ )
    {
      return _interactive = interactive_ ;
    }

    bool interactive( void ) const
    {
      return _interactive;
    }

    unsigned int interactiveItemFlags( void )
    {
      return _interactiveItemFlags;
    }

  protected:
    bool _interactive;
    unsigned int _interactiveItemFlags;

  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__COLLAPSABLE_ITEM__
