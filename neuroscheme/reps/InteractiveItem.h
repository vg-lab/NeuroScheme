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

    InteractiveItem( void )
    {
    }
    virtual ~InteractiveItem( void ) { }

    void init ( void )
    {
      std::cout << "-ii-" << this << std::endl;
      QGraphicsItem* qgi = dynamic_cast< QGraphicsItem* >( this );
      if ( qgi )
      {
        Log::log( "ll" );
        qgi->setAcceptHoverEvents( true );
      } else
        Log::log( "InteractiveItem not QGraphicsItem", LOG_LEVEL_WARNING );
    }

    void hoverEnterEvent( QGraphicsSceneHoverEvent* /*event*/ )
    {
      std::cout << "hoooover" << std::endl;
    }

  };

} // namespace neuroscheme

#endif // __NEUROSCHEME__COLLAPSABLE_ITEM__
