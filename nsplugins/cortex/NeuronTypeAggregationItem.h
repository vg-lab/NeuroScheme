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

#ifndef __NSLIB__NEURON_TYPE_AGGREGATION_ITEM__
#define __NSLIB__NEURON_TYPE_AGGREGATION_ITEM__

#include <nslib/Color.h>
#include <nslib/InteractionManager.h>
#include <nslib/reps/CollapsableItem.h>
#include <nslib/reps/Item.h>
#include <nslib/reps/InteractiveItem.h>
#include "ColumnRep.h"
#include "LayerRep.h"
#include "NeuronRep.h"
#include "NeuronTypeAggregationRep.h"
#include <shift_NeuronAggregationRep.h>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>

namespace nslib
{
  namespace cortex
  {
    class NeuronTypeAggregationItem
      : public QGraphicsPathItem
      , public nslib::Item
      , public nslib::SelectableItem
      , public nslib::InteractiveItem
    {
      public:
        NeuronTypeAggregationItem( const NeuronTypeAggregationRep& layerRep  );

        void create( const unsigned int size = 50 );

        QAbstractGraphicsShapeItem* symbolItem( void );

        virtual ~NeuronTypeAggregationItem( void ) {}

        virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ );
        virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ );
        virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ );

      public slots:
        void disable( void );

      protected:
        QAbstractGraphicsShapeItem* _symbolItem;
    };

  } // namespace cortex
} // namespace nslib

#endif
