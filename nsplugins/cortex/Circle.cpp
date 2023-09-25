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

#include <nslib/InteractionManager.h>
#include "Circle.h"

namespace nslib
{
  namespace cortex
  {
    Circle::Circle( QGraphicsItem* parent_ )
        : QGraphicsEllipseItem( parent_ )
    {
      setAcceptHoverEvents( true );
      setFlags( QGraphicsItem::ItemIsPanel );
    }

    shift::Representation* Circle::parentRep( void ) const
    {
      return dynamic_cast< Item* >(this->parentItem( ))->parentRep( );
    }

    void Circle::hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverEnterEvent( this, event_ );
    }

    void Circle::hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverLeaveEvent( this, event_ );
    }

    void Circle::contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
    {
      InteractionManager::contextMenuEvent( dynamic_cast
        < QAbstractGraphicsShapeItem* >(this->parentItem( )), event_ );
    }
  }
}
