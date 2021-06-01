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
#define _USE_MATH_DEFINES
#include <cmath>
#include "CircularLayout.h"
#include "../reps/Item.h"
#include "../reps/QGraphicsItemRepresentation.h"
#include "../error.h"
#include "../RepresentationCreatorManager.h"
#include <QToolBox>

namespace nslib
{
  CircularLayout::CircularLayout( void )
    : Layout( "Circular", Layout::SORT_ENABLED | Layout::FILTER_ENABLED,
        new QWidget )
    , _lineEditRadius( new QDoubleSpinBox )
  {
    auto layout_ = new QGridLayout;
    layout_->setAlignment( Qt::AlignTop );
    _layoutSpecialProperties->setLayout( layout_ );

    const auto labelRadius =
      new QLabel( QString::fromStdString( "Radius:" ));
    _lineEditRadius->setRange( 5.0, 305.0 );
    _lineEditRadius->setValue( 50.0 );
    _lineEditRadius->setEnabled( true );

    layout_->addWidget( labelRadius, 0, 0 );
    layout_->addWidget( _lineEditRadius, 0, 1 );

    auto button = new QPushButton( "Apply" );

    layout_->addWidget( button, 1, 0, 1, 2 );

    connect( button, SIGNAL( clicked( )), this,
      SLOT( refreshCanvas( )));
  }

  void CircularLayout::radius( float radius)
  {
    _lineEditRadius->setValue( radius );
  }

  float CircularLayout::radius( ) const
  {
    return _lineEditRadius->value( );
  }

  void CircularLayout::_arrangeItems( const shift::Representations& reps,
    bool animate, const shift::Representations& postFilterReps )
  {
    std::unordered_set< QGraphicsItem* > filteredOutItems;
    auto useOpacityForFilter = _filterWidget->useOpacityForFiltering( );
    const bool doFiltering =
      _filterWidget && !_filterWidget->filterSetConfig( ).filters( ).empty( );
    unsigned int maxItemWidth = 0, maxItemHeight = 0;
    unsigned int repsToBeArranged = 0;
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
          QRectF rect = item->childrenBoundingRect( ) | item->boundingRect( );

          if ( rect.width( ) > maxItemWidth )
            maxItemWidth = static_cast< unsigned int >(rect.width( ));

          if ( rect.height( ) > maxItemHeight )
            maxItemHeight = static_cast< unsigned int >(rect.height( ));
        }
      }
    }

    constexpr unsigned int marginX = 20;
    constexpr unsigned int marginY = 20;

    int counter = 0;

    QGraphicsView* gv = _canvas->scene( ).views( )[0];

    const float deltaAngle = 2.0f * static_cast< float >( M_PI ) / repsToBeArranged;
    const float radius = std::min( gv->width( ), gv->height( )) * 0.5f;
    const float deltaAngleMultRadius =
      ( 1.0f - static_cast< float >( _lineEditRadius->value( )) * 0.01f )
      * deltaAngle * radius;

    const auto opacity = _filterWidget
      ? float( _filterWidget->opacityValue( )) * 0.01f : 1.0f;
    qreal repsScale = 1.0f;
    for ( const auto representation : reps )
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

          qreal posX;
          qreal posY;
          float scaleX;
          float scaleY;

          if ( repsToBeArranged == 1 )
          {
            scaleX = static_cast<float>( ( gv->width( ) - 2 * marginX )  / rect.width( ));
            scaleY = static_cast<float>( ( gv->height( ) - 2 * marginY ) / rect.height( ));
            repsScale = std::min( scaleX, scaleY );
            posX = 0;
            posY = 0;
          }
          else
          {
            scaleX = static_cast<float>( deltaAngleMultRadius / rect.width( ));
            scaleY = static_cast<float>( deltaAngleMultRadius / rect.height( ));
            repsScale = std::min( scaleX, scaleY );
            const qreal angle = counter * deltaAngle;
            posX = radius * cos( angle );
            posY = radius * sin( angle );
          }

          if ( useOpacityForFilter &&
               filteredOutItems.count( graphicsItem ) == 1 )
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
      ++counter;
    }
  }

  void CircularLayout::_updateOptionsWidget( void )
  {
  }

  Layout* CircularLayout::clone( void ) const
  {
    CircularLayout* layout = new CircularLayout( );
    layout->radius(this->radius( ));
    return layout;
  }
}
