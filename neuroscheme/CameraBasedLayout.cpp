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
#include "CameraBasedLayout.h"
#include "reps/Item.h"
#include "error.h"
#include "RepresentationCreatorManager.h"

namespace neuroscheme
{

  CameraBasedLayout::CameraBasedLayout( void )
    : Layout( "3D" )
  {
    _optionsWidget->layout( )->addWidget( new QPushButton( "hola 3D1" ), 0, 0 );
    _optionsWidget->layout( )->addWidget( new QPushButton( "hola 3D2" ), 0, 1 );

  }

  void CameraBasedLayout::_arrangeItems( QGraphicsScene* scene,
                                         const shift::Representations& reps,
                                         bool animate )
  {
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
        auto graphicsItem = graphicsItemRep->item( scene );
        auto item = dynamic_cast< Item* >( graphicsItem );
        auto obj = dynamic_cast< QObject* >( graphicsItem );
        if ( graphicsItem && item )
        {
#define ANIM_DURATION 1200
          if ( obj && animate )
          {
            item->posAnim( ).setTargetObject( obj );
            item->posAnim( ).setPropertyName( "pos" );
            item->scaleAnim( ).setTargetObject( obj );
            item->scaleAnim( ).setPropertyName( "scale" );
            item->posAnim( ).setDuration( ANIM_DURATION );
            item->scaleAnim( ).setDuration( ANIM_DURATION );
            item->posAnim( ).setStartValue( graphicsItem->pos( ));
            item->scaleAnim( ).setStartValue( graphicsItem->scale( ));
            item->posAnim( ).setEndValue( QPoint( 0, 0));
            item->scaleAnim( ).setEndValue( 1 );
            item->posAnim( ).start( );
            item->scaleAnim( ).start( );
          }
          else
          {
            graphicsItem->setPos( 0, 0 );
            graphicsItem->setScale( 1 );
          }
        }
      }
    }
  }
} // namespace neuroscheme
