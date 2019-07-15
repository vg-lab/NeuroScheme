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
#include <nslib/InteractionManager.h>
#include "Triangle.h"

namespace nslib
{
  namespace cortex
  {
    Triangle::Triangle( QPolygonF polygon_, QGraphicsItem* parent_ )
      : QGraphicsPolygonItem( polygon_, parent_ )
    {
      setAcceptHoverEvents( true );
      setFlags( QGraphicsItem::ItemIsPanel );
    }

    shift::Representation* Triangle::parentRep( void ) const
    {
      return dynamic_cast< Item* >(this->parentItem( ))->parentRep( );
    }

    void Triangle::hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverEnterEvent( this, event_ );
      // dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
      // event_ );
    }

    void Triangle::hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverLeaveEvent( this, event_ );
      // dynamic_cast< QAbstractGraphicsShapeItem* >(this ->parentItem( )),
      // event_ );
    }

    void Triangle::contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
    {
      InteractionManager::contextMenuEvent(
        dynamic_cast< QAbstractGraphicsShapeItem* >(this->parentItem( )),
        event_ );
    }
  }
}

