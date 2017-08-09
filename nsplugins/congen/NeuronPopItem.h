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
#ifndef __NSLIB__NEURON_ITEM__
#define __NSLIB__NEURON_ITEM__

#include <nslib/Color.h>
#include <nslib/InteractionManager.h>
#include <nslib/reps/InteractiveItem.h>
#include <nslib/reps/Item.h>
#include <nslib/reps/SelectableItem.h>
#include "NeuronPopRep.h"
#include <QGraphicsEllipseItem>

namespace nslib
{
  namespace congen
  {

    class NeuronPopItem
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

      NeuronPopItem( const NeuronPopRep& neuronRep,
                  unsigned int size = 100,
                  bool interactive = true );

      virtual ~NeuronPopItem( void ) {}

      virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event_ )
      {
        if ( _interactive )
        {
          auto qGraphicsItemRep =
            dynamic_cast< QGraphicsItemRepresentation* >( _parentRep );
          if ( qGraphicsItemRep )
            for ( auto& item : qGraphicsItemRep->items( ))
            {
              auto qAbstractGraphicItem =
                dynamic_cast< QAbstractGraphicsShapeItem* >( item.second );
              if ( qAbstractGraphicItem )
                InteractionManager::hoverEnterEvent( qAbstractGraphicItem, event_ );
            }
          InteractionManager::highlightConnectivity( this );
        }
      }

      virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
      {
        if ( _interactive )
        {
          auto qGraphicsItemRep =
            dynamic_cast< QGraphicsItemRepresentation* >( _parentRep );
          if ( qGraphicsItemRep )
            for ( auto& item : qGraphicsItemRep->items( ))
            {
              auto qAbstractGraphicItem =
                dynamic_cast< QAbstractGraphicsShapeItem* >( item.second );
              if ( qAbstractGraphicItem )
                InteractionManager::hoverLeaveEvent( qAbstractGraphicItem, event_ );
            }
          InteractionManager::highlightConnectivity( this, false );
        }
      }

      virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
      {
        if ( _interactive )
          InteractionManager::contextMenuEvent( this, event_ );
      }

      // virtual void mousePressEvent( QGraphicsSceneMouseEvent* event_ )
      // {
      //   if ( _interactive )
      //     InteractionManager::mousePressEvent( this, event_ );
      // }


    protected:


    };


  } // namespace congen
} // namespace nslib

#endif
