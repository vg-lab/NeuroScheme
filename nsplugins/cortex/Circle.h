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
#ifndef __NSLIBCORTEX__CIRCLE__
#define __NSLIBCORTEX__CIRCLE__

#include <nslib/reps/SelectableItem.h>
#include <nslib/reps/Item.h>
#include <shift/Representation.h>

namespace nslib
{
  namespace cortex
  {
    class Circle
      : public QGraphicsEllipseItem
      , public SelectableItem
      , public Item
    {
      public:
      Circle( QGraphicsItem* parent_ );
      virtual ~Circle() {};

      shift::Representation* parentRep( void ) const final;

      virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ );

      virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ );

      virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ );
    };
  }
}
#endif //__NSLIBCORTEX__CIRCLE__
