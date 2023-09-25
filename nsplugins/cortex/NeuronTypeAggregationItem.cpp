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
#include "NeuronTypeAggregationItem.h"
#include "NeuronItem.h"
#include "Triangle.h"
#include "Circle.h"
#include <nslib/reps/CollapseButtonItem.h>
#include <nslib/error.h>
#include <QPen>

namespace nslib
{
  namespace cortex
  {
    NeuronTypeAggregationItem::NeuronTypeAggregationItem(
      const NeuronTypeAggregationRep& parentRep_ )
      : _symbolItem{nullptr}
    {
      setAcceptHoverEvents( true );
      setFlags( QGraphicsItem::ItemIsPanel );
      assert( this->isPanel( ));
      this->_parentRep = &( const_cast< NeuronTypeAggregationRep& >( parentRep_ ));
    }

    void NeuronTypeAggregationItem::create( const unsigned int size )
    {
      QPolygonF triangle;
      QGraphicsEllipseItem* ellipseItem;
      const int iSize = static_cast<int>(size);
      switch ( this->_parentRep->getPropertyValue< NeuronTypeAggregationRep::TSymbol >( "symbol" ))
      {
        case NeuronTypeAggregationRep::TRIANGLE:
          triangle.append( QPointF(        0, -iSize / 2.8f ));
          triangle.append( QPointF(   size/2,   size / 2 ));
          triangle.append( QPointF( -iSize/2,   size / 2 ));
          triangle.append( QPointF(        0, -iSize / 2.8f ));
          _symbolItem = new Triangle( triangle, this );
          break;
        case NeuronTypeAggregationRep::CIRCLE:
          ellipseItem = new Circle( this );
          ellipseItem->setRect( -iSize / 2.5f , -iSize / 2.5f,
                                          size,          size );
          _symbolItem = ellipseItem;
          break;
        default:
          assert( false );
          break;
      }
    }

    QAbstractGraphicsShapeItem* NeuronTypeAggregationItem::symbolItem( void )
    {
      return _symbolItem;
    }

    void NeuronTypeAggregationItem::hoverEnterEvent(
      QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverEnterEvent( this, event_ );
    }

    void NeuronTypeAggregationItem::hoverLeaveEvent(
      QGraphicsSceneHoverEvent* event_ )
    {
      InteractionManager::hoverLeaveEvent( this, event_ );
    }

    void NeuronTypeAggregationItem::contextMenuEvent(
      QGraphicsSceneContextMenuEvent* event_ )
    {
      InteractionManager::contextMenuEvent( this, event_ );
    }

    void NeuronTypeAggregationItem::disable( void )
    {
      this->setEnabled( false );
      this->setVisible( false );
    }

  } // namespace cortex
} // namespace nslib
