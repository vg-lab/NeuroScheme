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
#include "error.h"
#include "RepresentationCreatorManager.h"

namespace neuroscheme
{

  GridLayout::GridLayout( void )
    : Layout( "Grid" )
  {
    //std::cout << "grid options widget" << _optionsWidget << std::endl;
    _optionsWidget->layout( )->addWidget(
      new QPushButton( "hola grid" ), 0, 0 );
    _optionsWidget->layout( )->addWidget(
      new QPushButton( "hola grid2" ), 1, 0 );
  }

  void GridLayout::displayItems( QGraphicsScene* _scene,
                                 const shift::Representations& reps )
  {
    if ( !_scene ) return;
    auto& scene = *_scene;

    _representations = reps;
    if ( reps.empty( ))
      return;

    auto clearFirst = true;
    if ( clearFirst )
    {
      // Remove top items without destroying them
      QList< QGraphicsItem* > items_ = scene.items( );
      for ( auto item = items_.begin( ); item != items_.end( ); ++item )
      {
        auto item_ = dynamic_cast< Item* >( *item );
        if ( item_ && item_->parentRep( ))
          scene.removeItem( *item );
      }

      // Remove the rest
      scene.clear( );
    }


    NEUROSCHEME_DEBUG_CHECK( scene.views( ).size( ) != 0,
                             "Scene with no view" );
    QGraphicsView* gv = scene.views( )[0];

#define SHOW_SCENE_CORNERS
#ifdef SHOW_SCENE_CORNERS
    {
      // std::cout << gv->width( ) << " x " << gv->height( ) << std::endl;
      QGraphicsEllipseItem* center =
        new QGraphicsEllipseItem( -10, -10, 20, 20 );
      scene.addItem( center );
      QGraphicsEllipseItem* tl =
        new QGraphicsEllipseItem( -gv->width( ) / 2 - 10,
                                  -gv->height( ) / 2 - 10,
                                  20, 20 );

      scene.addItem( tl );
      QGraphicsEllipseItem* br =
        new QGraphicsEllipseItem( gv->width( ) / 2 - 10,
                                  gv->height( ) / 2 - 10,
                                  20, 20 );

      scene.addItem( br );
    }
#endif

    unsigned int maxItemWidth = 0, maxItemHeight = 0;

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
        auto item = graphicsItemRep->item( &scene );
        // std::cout << "new item " << item << std::endl;

        // Find out if its entity is selected
        // and if so set its pen
        const auto& repsToEntities =
          RepresentationCreatorManager::repsToEntities( );
        const auto entities = repsToEntities.at( representation );
        if ( entities.size( ) < 1 )
          Log::log( NS_LOG_HEADER +
                    "No entities associated to representation",
                    LOG_LEVEL_ERROR );

        auto selectableItem = dynamic_cast< SelectableItem* >( item );
        if ( selectableItem )
        {
          auto selectedState = SelectionManager::getSelectedState(
            *entities.begin( ));

          // if ( selectedState == selectedStateSelectedState::SELECTED )
          selectableItem->setSelected( selectedState );


          auto shapeItem =
            dynamic_cast< QAbstractGraphicsShapeItem* >( item );
          if ( shapeItem )
          {
            if ( selectedState == SelectedState::SELECTED )
              shapeItem->setPen( InteractionManager::getSelectedPen( ));
            else if ( selectedState == SelectedState::PARTIALLY_SELECTED )
              shapeItem->setPen(
                InteractionManager::getPartiallySelectedPen( ));

          }
        }
        //std::cout << &scene << " add item " << std::endl;
        scene.addItem( item );
        QRectF rect = item->childrenBoundingRect( ) | item->boundingRect( );

        if ( rect.width( ) > maxItemWidth )
          maxItemWidth = rect.width( );

        if ( rect.height( ) > maxItemHeight )
          maxItemHeight = rect.height( );
      }
    }

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

    // std::cout << "maxItemWidth = " << maxItemWidth << std::endl;
    // std::cout << "maxItemHeight = " << maxItemHeight << std::endl;
    // std::cout << "deltaX = " << deltaX << std::endl;
    // std::cout << "deltaY = " << deltaY << std::endl;
    int _x = 0;
    int _y = 0;

    float iconAspectRatio = float( maxItemWidth ) / float( maxItemHeight);
    float canvasAspectRatio;

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
        auto item = graphicsItemRep->item( &scene );
        if ( item )
        {
          QRectF rect = item->childrenBoundingRect( ) | item->boundingRect( );
          qreal posX = _x * deltaX * scale - gv->width( ) / 2 +
            leftMargin - scale * rect.center( ).x( );
          qreal posY = _y * deltaY * scale - gv->height( ) / 2 +
            topMargin - scale * rect.center( ).y( );
          qreal scale_ = forceScale ? forcedScale : scale;

          item->setPos( posX, posY );
          item->setScale( scale_ );
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

  Layout* GridLayout::clone( void ) const
  {
    return new GridLayout;
  }

}
