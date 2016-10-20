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

#define NEAR 0.1f
#define FAR 100.0f
#define FOV 53.1301f

  CameraBasedLayout::CameraBasedLayout( void )
    : Layout( "3D" )
  {
    _optionsWidget->layout( )->addWidget( new QPushButton( "hola 3D1" ), 0, 0 );
    _optionsWidget->layout( )->addWidget( new QPushButton( "hola 3D2" ), 0, 1 );

    _viewMatrix = Matrix4f::Identity( );
  }

  void CameraBasedLayout::_arrangeItems( QGraphicsScene* scene,
                                         const shift::Representations& reps,
                                         bool animate )
  {
    float ratio = float( scene->width( )) / float( scene->height( ));;

    double S = 1.0 / ( tan( FOV * 0.5 * M_PI / 180.0 ));

    _projectionMatrix.row( 0 ) = Vector4f( -S / ratio, 0, 0, 0);
    _projectionMatrix.row( 1 ) = Vector4f( 0, S, 0, 0 );
    _projectionMatrix.row( 2 ) =
      Vector4f( 0.0f, 0.0f, -( NEAR + FAR ) / ( NEAR - FAR ),
                2 * NEAR * FAR / (NEAR - FAR));
    _projectionMatrix.row( 3 ) = Vector4f( 0, 0, 1, 0 );

    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );

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

          const auto entities = repsToEntities.at( representation );
          if ( entities.size( ) < 1 )
            Log::log( NS_LOG_HEADER +
                      "No entities associated to representation",
                      LOG_LEVEL_ERROR );
          auto center =
            DomainManager::getActiveDomain( )->entity3DPosition(
              *entities.begin( ));

          Vector4f pos = _viewMatrix * center;
          const float distance = pos.norm( );
          const bool behindCamera = pos.z( ) > 0;
          pos = _projectionMatrix * pos;
          pos /= pos.w( );
          pos[0] = pos[0] * float( scene->width( )) * 0.5f;
          pos[1] = pos[1] * float( scene->height( )) * 0.5f;

          auto x = pos[0];
          auto y = pos[1];

          auto scale = 250.0 / distance;
          auto zValue = -distance;

          // Temporarily use fake values
          x = 0; y = 0; scale = 1;

#define ANIM_DURATION 1200
          if ( obj && animate )
          {
            if ( behindCamera )
            {
              graphicsItem->setScale( 0.000001 );
            } else
            {
              graphicsItem->setZValue( zValue );
              item->posAnim( ).setTargetObject( obj );
              item->posAnim( ).setPropertyName( "pos" );
              item->scaleAnim( ).setTargetObject( obj );
              item->scaleAnim( ).setPropertyName( "scale" );
              item->posAnim( ).setDuration( ANIM_DURATION );
              item->scaleAnim( ).setDuration( ANIM_DURATION );
              item->posAnim( ).setStartValue( graphicsItem->pos( ));
              item->scaleAnim( ).setStartValue( graphicsItem->scale( ));
              item->posAnim( ).setEndValue( QPoint( x, y ));
              item->scaleAnim( ).setEndValue( scale );
              item->posAnim( ).start( );
              item->scaleAnim( ).start( );
            }
          }
          else
          {
            if ( behindCamera )
            {
              graphicsItem->setScale( 0.000001 );
            } else
            {
              graphicsItem->setPos( x, y );
              graphicsItem->setScale( scale );
            }
          }
        }
      }
    }
  }
} // namespace neuroscheme
