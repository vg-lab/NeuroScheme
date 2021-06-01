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
#include <QtWidgets>

namespace nslib
{
  GridLayout::GridLayout( void )
    : Layout( "Grid", Layout::SORT_ENABLED | Layout::FILTER_ENABLED, new QWidget )
    , _lineEditPaddingX( new QDoubleSpinBox )
    , _lineEditPaddingY( new QDoubleSpinBox )
  {
    auto layout_ = new QGridLayout;
    layout_->setAlignment( Qt::AlignTop );
    _layoutSpecialProperties->setLayout( layout_ );

    const auto labelPaddingX =
      new QLabel( QString::fromStdString( "Padding X:" ));
    _lineEditPaddingX->setRange( 95.0, 230.0 );
    _lineEditPaddingX->setValue( 110 );
    _lineEditPaddingX->setEnabled( true );

    layout_->addWidget( labelPaddingX, 0, 0 );
    layout_->addWidget( _lineEditPaddingX, 0, 1 );

    const auto labelPaddingY =
      new QLabel( QString::fromStdString( "Padding Y:" ));
    _lineEditPaddingY->setRange( 95.0, 230.0 );
    _lineEditPaddingY->setValue( 110 );
    _lineEditPaddingY->setEnabled( true );

    layout_->addWidget( labelPaddingY, 1, 0 );
    layout_->addWidget( _lineEditPaddingY, 1, 1 );

    QPushButton* button = new QPushButton( "Apply" );

    layout_->addWidget( button, 2, 0, 1, 2 );

    connect( button, SIGNAL( clicked( )), this,
      SLOT( refreshCanvas( )));
  }

  void GridLayout::padding( float paddingX, float paddingY )
  {
    _lineEditPaddingX->setValue( paddingX );
    _lineEditPaddingY->setValue( paddingY );
  }

  float GridLayout::paddingX( ) const
  {
    return _lineEditPaddingX->value( );
  }

  float GridLayout::paddingY( ) const
  {
    return _lineEditPaddingY->value( );
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
        nslib::Loggers::get( )->log( "Item null",
          nslib::LOG_LEVEL_WARNING );
      }
      else
      {
        auto item = graphicsItemRep->item( &_canvas->scene( ));
        if ( !item->parentItem( ))
        {
          ++repsToBeArranged;
          if ( doFiltering && useOpacityForFilter )
          {
            if ( std::find( postFilterReps.begin( ), postFilterReps.end( ),
              representation ) == postFilterReps.end( ))
            {
              filteredOutItems.insert( item );
            }
          }

          const QRectF rect = item->childrenBoundingRect( ) | item->boundingRect( );

          if ( rect.width( ) > maxItemWidth )
          {
            maxItemWidth = static_cast< unsigned int >( rect.width( ));
          }
          if ( rect.height( ) > maxItemHeight )
          {
            maxItemHeight = static_cast<unsigned int>( rect.height( ));
          }
        }
      }
    }

    constexpr unsigned int marginX = 20;
    constexpr unsigned int marginY = 20;
    const float scalePaddingX = static_cast< float >(
      _lineEditPaddingX->value( )) * 0.01f;
    const float scalePaddingY = static_cast< float >(
      _lineEditPaddingY->value( )) * 0.01f;

    unsigned int deltaX = static_cast< unsigned int >(
      scalePaddingX * maxItemWidth );
    unsigned int deltaY = static_cast< unsigned int >(
      scalePaddingY * maxItemHeight );

    int _x = 0;
    int _y = 0;

    const float iconAspectRatio = float( maxItemWidth ) / float( maxItemHeight);

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

    const float scaleX = float( gv->width( ) - 2.0f * marginX ) /
      float( numColumns * deltaX );
    const float scaleY = float( gv->height( ) - 2.0f * marginY ) /
      float( numRows * deltaY );
    const qreal repsScale = std::min( scaleX, scaleY );

    const int leftMargin = static_cast< int >( ( ( deltaX * repsScale ) +
      ( gv->width( ) - numColumns * deltaX * repsScale )) * 0.5f );
    const int topMargin = static_cast< int >( ( ( deltaY * repsScale ) +
      ( gv->height( ) - numRows * deltaY * repsScale )) * 0.5f);

    const auto opacity = _filterWidget
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
          const QRectF rect = graphicsItem->childrenBoundingRect( ) |
            graphicsItem->boundingRect( );
          const qreal posX = _x * deltaX * repsScale - gv->width( ) * 0.5f +
            leftMargin - repsScale * rect.center( ).x( );
          const qreal posY = _y * deltaY * repsScale - gv->height( ) * 0.5f +
            topMargin - repsScale * rect.center( ).y( );

          if ( useOpacityForFilter && filteredOutItems.count( graphicsItem ) == 1 )
          {
            graphicsItem->setOpacity( opacity );
          }
          else
            graphicsItem->setOpacity( 1.0f );

          if ( obj && animate )
          {
            animateItem( graphicsItem, repsScale, QPoint( posX, posY ));
          }
          else
          {
            graphicsItem->setPos( posX, posY );
            graphicsItem->setScale( repsScale );
          }
        }
      }

      _canvas->repsScale( repsScale );

      ++_x;
      if (((unsigned int ) _x ) >= numColumns )
      {
        _x = 0;
        ++_y;
      }
    }
  }

  void GridLayout::_updateOptionsWidget( void )
  {}

  Layout* GridLayout::clone( void ) const
  {
    GridLayout* layout = new GridLayout( );
    layout->padding(this->paddingX( ),this->paddingY( ));
    return layout;
  }
}
