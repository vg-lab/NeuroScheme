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

      auto circleItem = new QGraphicsEllipseItem( // this 
        );
      auto circleItemSize = roundf( size * 0.75f );
      circleItem->setRect( - int( circleItemSize ) / 2,
                           - int( circleItemSize ) / 2,
                           circleItemSize, circleItemSize );
      circleItem->setPen( Qt::NoPen );
      circleItem->setBrush( QBrush( bgColor ));

      QPainterPath path_;
      QPolygon poly;

      int size_2 = roundf( size * 0.5f );
      int sizeTopBottomX = roundf( size_2 * 0.5f );
      int sizeTopBottomY = roundf( size_2 * 0.7f );
      int sizeMiddle = roundf( size_2 * 0.9f );
      QPoint pTL ( -sizeTopBottomX, -sizeTopBottomY );
      QPoint pTR (  sizeTopBottomX, -sizeTopBottomY );
      QPoint pBR (  sizeTopBottomX,  sizeTopBottomY );
      QPoint pBL ( -sizeTopBottomX,  sizeTopBottomY );

      QPoint pMR (  sizeMiddle,  0           );
      QPoint pML ( -sizeMiddle,  0           );

      poly << pTL << pTR << pMR << pBR << pBL << pML;
      // QPoint pUL (-int(size)/2, -int(size)/24);
      // QPoint pUM (           0, -int(size)/6);
      // QPoint pUR ( int(size)/2, -int(size)/24);

      // QPoint pLR ( int(size)/2, +int(size)/24);
      // QPoint pLM (           0, +int(size)/6);
      // QPoint pLL (-int(size)/2, +int(size)/24);

      // poly << pLR << pLM
      //      << pLL
      //      << pUL //<< pUM
      //      << pUR;

      path_.addPolygon( poly );
      path_.closeSubpath(  );

      auto icon = new QGraphicsPathItem( this );
      icon->setPath( path_ );
      icon->setPen( Qt::NoPen );
      icon->setBrush( bgColor ); //QBrush( QColor( 114, 188, 196 )));
      //icon->setBrush( QBrush( baseColor ));

      // auto lineContainerWidth = roundf( circleItemSize * .9f );
      // auto lineContainerHeight = roundf( circleItemSize * .1f );
      // auto lineContainer = new QGraphicsRectItem(
      //   roundf( - int( lineContainerWidth ) * .5f ),
      //   roundf( - int( lineContainerHeight ) * .5f ),
      //   lineContainerWidth,
      //   lineContainerHeight );

      // lineContainer->setPen( Qt::NoPen );
      // lineContainer->setBrush( QBrush( QColor( 255, 255, 255 )));
      // // lineContainer->setParentItem( this );

      //auto linePadding = roundf( lineContainerWidth * 0.01f );
      // auto lineWidth = lineContainerWidth; //roundf(  circleItemSize * .85f );
      // auto lineHeight = lineContainerHeight; //roundf( circleItemSize * .09f );
      auto line = new QGraphicsRectItem(
        pBL.x( ),
        pBL.y( ) - size_2 * 0.03,
        roundf( ( pBR.x( ) - pBL.x( )) *
                neuronRep.getProperty( "line perc" ).value< float >( )),
        size_2 * 0.06 );

        // roundf( - int( lineWidth ) * .5f ) + linePadding,
        // roundf( - int( lineHeight ) * .5f ) + linePadding,
        // roundf( lineWidth *
        //         neuronRep.getProperty( "line perc" ).value< float >( )) -
        // 2 * linePadding,
        // lineHeight - 2 * linePadding);

      line->setPen( Qt::NoPen );
      line->setBrush( QBrush( QColor( 180, 70, 70 )));
      line->setParentItem( this );

      this->_parentRep = &( const_cast< NeuronPopRep& >( neuronRep ));
    }

  } // namespace congen
} // namespace nslib
