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
#include "GridLayout.h"
#include "reps/Item.h"
#include "reps/QGraphicsItemRepresentation.h"
#include "error.h"
#include "RepresentationCreatorManager.h"
#include <QToolBox>

namespace neuroscheme
{

  GridLayout::GridLayout( void )
    : Layout( "Grid",
              Layout::SORT_ENABLED |
              Layout::FILTER_ENABLED )
  {
  }

  void GridLayout::_arrangeItems( const shift::Representations& reps,
                                  bool animate,
                                  const shift::Representations& postFilterReps )
  {
    // std::cout << "pre filter: " << reps.size( ) << std::endl;
    // std::cout << "post filter: " << postFilterReps.size( ) << std::endl;
    std::unordered_set< QGraphicsItem* > filteredOutItems;
    auto useOpacityForFilter = _filterWidget->useOpacityForFiltering( );
    bool doFiltering =
      _filterWidget &&
      _filterWidget->filterSetConfig( ).filters( ).size( ) > 0;
    unsigned int maxItemWidth = 0, maxItemHeight = 0;
    for ( const auto& representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< neuroscheme::QGraphicsItemRepresentation* >(
          representation );
      if ( !graphicsItemRep )
      {
        std::cerr << "Item null" << std::endl;
      }
      else
      {
        auto item = graphicsItemRep->item( _scene );
        if ( doFiltering && useOpacityForFilter )
        {
          if ( std::find( postFilterReps.begin( ), postFilterReps.end( ),
                          representation ) == postFilterReps.end( ))
          {
            filteredOutItems.insert( item );
          }
        }
        //_scene->addItem( item );
        QRectF rect = item->childrenBoundingRect( ) | item->boundingRect( );

        if ( rect.width( ) > maxItemWidth )
          maxItemWidth = rect.width( );

        if ( rect.height( ) > maxItemHeight )
          maxItemHeight = rect.height( );
      }
    }
    //std::cout << "filtered out " << filteredOutItems.size( ) << std::endl;

    bool forceScale = false;
    float forcedScale = 1.0f;
    const unsigned int marginX = 20;
    const unsigned int marginY = 20;
    const float scalePaddingX = 1.1f;
    const float scalePaddingY = 1.1f;
    const unsigned int fixedPaddingX = 0;
    const unsigned int fixedPaddingY = 0;

    unsigned int deltaX = scalePaddingX * maxItemWidth + fixedPaddingX;
    unsigned int deltaY = scalePaddingY * maxItemHeight + fixedPaddingY;

    int _x = 0;
    int _y = 0;

    float iconAspectRatio = float( maxItemWidth ) / float( maxItemHeight);
    float canvasAspectRatio;

    QGraphicsView* gv = _scene->views( )[0];
    if ( gv->width( ) > gv->height( ))
      canvasAspectRatio = float( gv->width( )) / float( gv->height( ));
    else
      canvasAspectRatio = float( gv->height( )) / float( gv->width( ));


    unsigned int numRows =
      floor( sqrt( iconAspectRatio * float( reps.size( )) /
                   canvasAspectRatio ));

    if ( numRows < 1 && reps.size( ) > 0 )
      numRows = 1;

    unsigned int numColumns =
      ceil( float( reps.size( )) / float( numRows ));

    if ( numColumns < 1 && reps.size( ) > 0 )
      numColumns = 1;

    if ( gv->width( ) < gv->height( ) )
      std::swap( numColumns, numRows );

    // std::cout << "Num rows: " << numRows << " Num cols: " << numColumns << std::endl;

    float scale;
    float scaleX = float( gv->width( ) - 2 * marginX ) /
      float( numColumns * deltaX );
    float scaleY = float( gv->height( ) - 2 * marginY ) /
      float( numRows * deltaY );
    scale = std::min( scaleX, scaleY );

    // std::cout << "Scale: " << scaleX << " " << scaleY << " " << scale << std::endl;

    int leftMargin = (( deltaX * scale )  +
                      ( gv->width( ) - numColumns * deltaX * scale )) / 2;
    int topMargin = (( deltaY * scale ) +
                     ( gv->height( ) - numRows * deltaY * scale )) / 2;

    auto opacity = 1.0f;
    if ( _filterWidget )
      opacity = float( _filterWidget->opacityValue( )) / 100.0;
    for ( const auto representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< neuroscheme::QGraphicsItemRepresentation* >(
          representation );
      if ( !graphicsItemRep )
      {
        std::cerr << "Item null" << std::endl;
      }
      else
      {
        auto graphicsItem = graphicsItemRep->item( _scene );
        auto item = dynamic_cast< Item* >( graphicsItem );
        auto obj = dynamic_cast< QObject* >( graphicsItem );
        if ( item )
        {
          QRectF rect = graphicsItem->childrenBoundingRect( ) |
            graphicsItem->boundingRect( );
          qreal posX = _x * deltaX * scale - gv->width( ) / 2 +
            leftMargin - scale * rect.center( ).x( );
          qreal posY = _y * deltaY * scale - gv->height( ) / 2 +
            topMargin - scale * rect.center( ).y( );
          qreal scale_ = forceScale ? forcedScale : scale;

          if ( useOpacityForFilter &&
               filteredOutItems.count( graphicsItem ) == 1 )
          {
            std::cout << "opacity " << _filterWidget->opacityValue( ) << std::endl;
            graphicsItem->setOpacity( opacity );
          }
          else
            graphicsItem->setOpacity( 1.0f );

          if ( obj && animate )
          {
            animateItem( graphicsItem, scale_, QPoint( posX, posY ));
          }
          else
          {
            graphicsItem->setPos( posX, posY );
            graphicsItem->setScale( scale_ );
          }
          // std::cout << posX << " " << posY << " " << scale_ << std::endl;
        }
      }

      _x++;
      if (((unsigned int ) _x ) >= numColumns )
      {
        _x = 0;
        _y++;
      }

    }

  }

  void GridLayout::_updateOptionsWidget( void )
  {

  }

  Layout* GridLayout::clone( void ) const
  {
    return new GridLayout( );
  }

}
