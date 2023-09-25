/*
 * Copyright (c) 2016 VG-Lab/URJC/UPM.
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
#include "../Loggers.h"
#include "CameraBasedLayout.h"
#include "../reps/Item.h"
#include "../error.h"
#include "../PaneManager.h"
#include "../RepresentationCreatorManager.h"

namespace nslib
{

#define NEAR 0.1f
#define FAR 100.0f
#define FOV 53.1301f

  CameraBasedLayout::CameraBasedLayout( void )
    : Layout( "3D", Layout::CAMERA_ENABLED )
  {
  }

  void CameraBasedLayout::_arrangeItems( const shift::Representations& reps,
                                         bool animate,
                                         const shift::Representations& )
  {
    const auto sceneWidth = 1000; //_scene->width( );
    const auto sceneHeight = 1000; //_scene->height( );
    const float ratio = static_cast<float>( sceneWidth ) / static_cast<float>( sceneHeight );
    const double S = 1.0 / ( tan( FOV * 0.5 * M_PI / 180.0 ));

    Matrix4f _projectionMatrix;
    _projectionMatrix.row( 0 ) = Vector4f( -S / ratio, 0, 0, 0);
    _projectionMatrix.row( 1 ) = Vector4f( 0, S, 0, 0 );
    _projectionMatrix.row( 2 ) =
      Vector4f( 0.0f, 0.0f, -( NEAR + FAR ) / ( NEAR - FAR ),
                2 * NEAR * FAR / (NEAR - FAR));
    _projectionMatrix.row( 3 ) = Vector4f( 0, 0, 1, 0 );

    const auto& repsToEntities =
      RepresentationCreatorManager::repsToEntities( );

    const auto& viewMatrix = PaneManager::viewMatrix( );
    for ( const auto& representation : reps )
    {
      auto graphicsItemRep =
        dynamic_cast< nslib::QGraphicsItemRepresentation* >( representation );
      if ( graphicsItemRep )
      {
        auto graphicsItem = graphicsItemRep->item( &_canvas->scene( ));
        if ( graphicsItem->parentItem( ))
          continue;

        auto item = dynamic_cast< Item* >( graphicsItem );
        auto obj = dynamic_cast< QObject* >( graphicsItem );
        if ( graphicsItem && item )
        {

          const auto entities = repsToEntities.at( representation );
          if ( entities.size( ) < 1 )
          {
            Loggers::get( )->log( "No entities associated to representation",
              LOG_LEVEL_ERROR, NEUROSCHEME_FILE_LINE );
          }
          auto center = DomainManager::getActiveDomain( )->entity3DPosition(
            *entities.begin( ));

          Vector4f pos = viewMatrix * center;
          const float distance = pos.norm( );
          bool behindCamera = pos.z( ) > 0;

//#define ANIM_DURATION 1200

          if ( behindCamera || distance == 0.0f)
          {
            graphicsItem->setScale( 0.000001f );
          }
          else
          {
            pos = _projectionMatrix * pos;
            auto posW = pos.w( );
            if ( posW == 0.0f )
            {
              posW = 2.0f;
            }
            else
            {
              posW = posW + posW;
            }
            auto x = pos[0] * float( sceneWidth ) / posW;
            auto y = pos[1] * float( sceneHeight ) / posW;

            auto scale = 250.0f / distance;
            auto zValue = -distance;

            if ( obj && animate )
            {
              graphicsItem->setZValue( zValue );
              animateItem( graphicsItem, scale, QPoint( x, y ));
            }
            else // not animation
            {
              graphicsItem->setZValue( zValue );
              graphicsItem->setPos( x, y );
              graphicsItem->setScale( scale );
            }
          }
        }
      }
    }
  }
} // namespace nslib
