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
#include "../reps/Item.h"
#include "../reps/QGraphicsItemRepresentation.h"
#include "../error.h"
#include "../RepresentationCreatorManager.h"
#include <QToolBox>

namespace nslib
{

  GridLayout::GridLayout( void )
    : Layout( "Grid", Layout::SORT_ENABLED | Layout::FILTER_ENABLED )
  {
  }

  void GridLayout::_arrangeItems( const shift::Representations& reps,
    bool animate, const shift::Representations& postFilterReps )
  {
    _isGrid = true;
    std::unordered_set< QGraphicsItem* > filteredOutItems;
    auto useOpacityForFilter = _filterWidget->useOpacityForFiltering( );
    bool doFiltering =
      _filterWidget && !_filterWidget->filterSetConfig( ).filters( ).empty( );
    unsigned int maxItemWidth = 0, maxItemHeight = 0;
    unsigned int repsToBeArranged = 0;
    for ( const auto& representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< nslib::QGraphicsItemRepresentation* >( representation );
      if ( !graphicsItemRep )
      {
        std::cerr << "Item null" << std::endl;
      }
      else
      {
        auto item = graphicsItemRep->item( &_canvas->scene( ));
        if ( !item->parentItem( ))
        {
          // std::cout << "++++++ Arranging " << item << std::endl;
          ++repsToBeArranged;
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
          {
            maxItemWidth = static_cast< unsigned int >( rect.width( ));
          }
          if ( rect.height( ) > maxItemHeight )
          {
            maxItemHeight = static_cast<unsigned int>( rect.height( ));
          }
        }
        // else std::cout << "Item with parentItem, skipping" << std::endl;
      }
    } // for all reps
    //std::cout << "filtered out " << filteredOutItems.size( ) << std::endl;
    // std::cout << "Arranging " << repsToBeArranged << std::endl;

    bool forceScale = false;
    float forcedScale = 1.0f;
    const unsigned int marginX = 20;
    const unsigned int marginY = 20;
    const float scalePaddingX = 1.1f;
    const float scalePaddingY = 1.1f;
    const unsigned int fixedPaddingX = 0;
    const unsigned int fixedPaddingY = 0;

    unsigned int deltaX = static_cast< unsigned int >(
      scalePaddingX * maxItemWidth + fixedPaddingX );
    unsigned int deltaY = static_cast< unsigned int >(
      scalePaddingY * maxItemHeight + fixedPaddingY );

    int _x = 0;
    int _y = 0;

    float iconAspectRatio = float( maxItemWidth ) / float( maxItemHeight);

    QGraphicsView* gv = _canvas->scene( ).views( ).first( );
    float canvasAspectRatio = ( gv->width( ) > gv->height( ))
      ? float( gv->width( )) / float( gv->height( ))
      : float( gv->height( )) / float( gv->width( ));

    unsigned int numRows = static_cast< unsigned int >( floorf( sqrtf(
      iconAspectRatio * float( repsToBeArranged ) / canvasAspectRatio )));

    if ( numRows < 1 && repsToBeArranged > 0 )
    {
      numRows = 1;
    }

    unsigned int numColumns = static_cast< unsigned int >(
      ceilf( float( repsToBeArranged ) / float( numRows )));

    if ( numColumns < 1 && repsToBeArranged > 0 )
    {
      numColumns = 1;
    }

    if ( gv->width( ) < gv->height( ))
    {
      std::swap( numColumns, numRows );
    }

    // std::cout << "Num rows: " << numRows << " Num cols: " << numColumns << std::endl;

    float scale;
    float scaleX = float( gv->width( ) - 2.0f * marginX ) /
      float( numColumns * deltaX );
    float scaleY = float( gv->height( ) - 2.0f * marginY ) /
      float( numRows * deltaY );
    scale = std::min( scaleX, scaleY );

    // std::cout << "Scale: " << scaleX << " " << scaleY << " " << scale << std::endl;

    int leftMargin = static_cast< int >( ( ( deltaX * scale ) +
      ( gv->width( ) - numColumns * deltaX * scale )) * 0.5f );
    int topMargin = static_cast< int >( ( ( deltaY * scale ) +
      ( gv->height( ) - numRows * deltaY * scale )) * 0.5f);

    auto opacity = _filterWidget
      ? float( _filterWidget->opacityValue( )) * 0.01 : 1.0f;
    for ( const auto& representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< nslib::QGraphicsItemRepresentation* >( representation );
      if ( !graphicsItemRep )
      {
        Loggers::get( )->log( "Item null", LOG_LEVEL_WARNING );
      }
      else
      {
        auto graphicsItem = graphicsItemRep->item( &_canvas->scene( ));
        auto item = dynamic_cast< Item* >( graphicsItem );
        if ( graphicsItem->parentItem( ))
          continue;

        auto obj = dynamic_cast< QObject* >( graphicsItem );
        if ( item )
        {
          QRectF rect = graphicsItem->childrenBoundingRect( ) |
            graphicsItem->boundingRect( );
          qreal posX = _x * deltaX * scale - gv->width( ) * 0.5f +
            leftMargin - scale * rect.center( ).x( );
          qreal posY = _y * deltaY * scale - gv->height( ) * 0.5f +
            topMargin - scale * rect.center( ).y( );
          qreal scale_ = forceScale ? forcedScale : scale;

          if ( useOpacityForFilter &&
               filteredOutItems.count( graphicsItem ) == 1 )
          {
            //std::cout << "opacity " << _filterWidget->opacityValue( ) << std::endl;
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

      ++_x;
      if (((unsigned int ) _x ) >= numColumns )
      {
        _x = 0;
        ++_y;
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
