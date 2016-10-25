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
#ifndef __NEUROSCHEME__NEURON_ITEM__
#define __NEUROSCHEME__NEURON_ITEM__

#include "../../Color.h"
#include "../../InteractionManager.h"
#include "../../reps/InteractiveItem.h"
#include "../../reps/Item.h"
#include "../../reps/SelectableItem.h"
#include "NeuronRep.h"
#include <QGraphicsEllipseItem>

namespace neuroscheme
{

  class NeuronItem
    : public QObject
    , public QGraphicsEllipseItem
    , public Item
    , public SelectableItem
    , public InteractiveItem
  {
    Q_OBJECT
    Q_PROPERTY( QPointF pos READ pos WRITE setPos )
    Q_PROPERTY( qreal scale READ scale WRITE setScale )
    // Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

  public:

    NeuronItem( const NeuronRep& neuronRep,
                unsigned int size = 100,
                bool interactive = true );

    virtual ~NeuronItem( void ) {}

    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event )
    {
      if ( _interactive )
        InteractionManager::hoverEnterEvent( this, event );
    }

    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event )
    {
      if ( _interactive )
        InteractionManager::hoverLeaveEvent( this, event );
    }

    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
    {
      if ( _interactive )
        InteractionManager::contextMenuEvent( this, event );
    }

    virtual void mousePressEvent( QGraphicsSceneMouseEvent* event )
    {
      if ( _interactive )
        InteractionManager::mousePressEvent( this, event );
    }


  protected:

    QGraphicsItem* _createSymbolItem( NeuronRep::TSymbol symbol,
                                      unsigned int size = 100 );

  };


} // namespace neuroscheme

#endif
