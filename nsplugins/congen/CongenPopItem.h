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

#ifndef __NSLIB__NEURON_ITEM__
#define __NSLIB__NEURON_ITEM__

#include <nslib/Color.h>
#include <nslib/InteractionManager.h>
#include <nslib/reps/InteractiveItem.h>
#include <nslib/reps/Item.h>
#include <nslib/reps/SelectableItem.h>
#include "CongenPopRep.h"
#include <QGraphicsEllipseItem>
#include <nslib/ItemText.h>

namespace nslib
{
  namespace congen
  {
    class CongenPopItem
      : public QObject
      , public QGraphicsEllipseItem
      , public nslib::Item
      , public nslib::SelectableItem
      , public nslib::InteractiveItem
    {
      Q_OBJECT
      Q_PROPERTY( QPointF pos READ pos WRITE setPos )
      Q_PROPERTY( qreal scale READ scale WRITE setScale )

    public:
      CongenPopItem( const CongenPopRep& entityRep,
        const unsigned int size = 100, const bool interactive = true );

      virtual ~CongenPopItem( void );

      virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ );

      virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ );

      virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ );

    protected:
      ItemText* _itemText;
    };

  } // namespace congen
} // namespace nslib

#endif
