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
#include "NeuronPopItem.h"
#include <nslib/reps/RingItem.h>
#include <QPen>

namespace nslib
{
  namespace congen
  {
    NeuronPopItem::NeuronPopItem( const NeuronPopRep& neuronRep,
                                  unsigned int size,
                                  bool interactive_ )
    {
      setInteractive( interactive_ );
      if ( interactive_ )
      {
        this->setAcceptHoverEvents( true );
      }

      int itemSize = ceil( float( size ) / 2.0f );
      this->setRect ( -itemSize, -itemSize,
                      itemSize * 2 , itemSize * 2 );
      this->setPen( QPen( Qt::NoPen ));

      const Color& bgColor = neuronRep.getProperty( "color" ).value< Color >( );

      auto circleItem = new QGraphicsEllipseItem( this );
      circleItem->setRect( - int( size ) / 2, - int( size ) / 2,
                           size, size );
      circleItem->setPen( Qt::NoPen );
      circleItem->setBrush( QBrush( bgColor ));

      this->_parentRep = &( const_cast< NeuronPopRep& >( neuronRep ));
    }

  } // namespace congen
} // namespace nslib
