/*
 * Copyright (c) 2017 GMRV/URJC/UPM.
 *
 * Authors: Iago Calvo Lista <i.calvol@alumnos.urjc.es>
 *          Fernando Lucas Pérez <f.lucas@alumnos.urjc.es>
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

#include "AutoConnectionArrowItem.h"
#include "ConnectionArrowItem.h"
#include "ConnectionArrowRep.h"

#include <nslib/Color.h>
#include <nslib/InteractionManager.h>
#include <nslib/reps/InteractiveItem.h>
#include <nslib/reps/Item.h>
#include <nslib/reps/SelectableItem.h>
#include <QGraphicsPolygonItem>
#include <QPropertyAnimation>
#include <nslib/Config.h>
#include <cmath>
#include <Eigen/Dense>

namespace nslib
{
  namespace congen
  {
    const float AutoConnectionArrowItem::Rad_To_Deg = 180.0f*M_PI_Inverse;

    //Values used to draw the arrowhead, made const to avoid recalculating
    const float AutoConnectionArrowItem::M_PI_0825 = 0.825f * M_PI_Float;
    const float AutoConnectionArrowItem::M_PI_1115 = 1.115f * M_PI_Float;

    //Default angle for drawing the autoconnections when the layout is a grid
    const float AutoConnectionArrowItem::M_PI_175 = 1.75f * M_PI_Float;

    float AutoConnectionArrowItem::_centersDistFactor = 0.5f;
    float AutoConnectionArrowItem::_arcSizeFactor = 0.35f;

    float  AutoConnectionArrowItem::glyphRadius = 0.0f;
    float  AutoConnectionArrowItem::arcRadius;
    float  AutoConnectionArrowItem::dist;
    float  AutoConnectionArrowItem::startAngle;
    float  AutoConnectionArrowItem::arcDegrees;

    AutoConnectionArrowItem::AutoConnectionArrowItem(
      const AutoConnectionArrowRep& autoConnectionArrowRep )
      : ConnectionArrowItem( autoConnectionArrowRep )
    {
      _arrowThickness = 1.3f *
        autoConnectionArrowRep.getProperty( "width" ).value< unsigned int >( );
    }

    const QLineF& AutoConnectionArrowItem::line( void )
    {
      return _line;
    }

    void AutoConnectionArrowItem::setLine( const QLineF& line_ )
    {
      _line = line_;
      createAutoArrow( float( _line.p1( ).x( )), _line.p1( ).y( ) == 1.0f ,
        _line.p2( ));
    }

    void AutoConnectionArrowItem::createAutoArrow( float glyphRadius_,
      bool isGrid_, QPointF glyphCenter_ )
    {
      float itemInvScale = 1.0f
        / static_cast< float >( this->scale( ));

      auto glyphCenter = itemInvScale * glyphCenter_;

      //Recalculates static data when relevant changes have been made
      auto newGlyphRadius = glyphRadius_ * itemInvScale;
      if( newGlyphRadius != glyphRadius )
      {
        glyphRadius = newGlyphRadius;
        AutoConnectionArrowItem::recalcArcData( );
      }

      float relativeAngle;
      if( isGrid_ )
      {
        //Fixes arrow position to the upper right corner when layout is grid
        relativeAngle = M_PI_175;
      }
      else
      {
        //Fixes arrow position to the point furthest from the canvas centre
        relativeAngle = atanf(
          float( glyphCenter.y( ) / glyphCenter.x( )));
        if( glyphCenter.x( ) < 0 )
        {
          relativeAngle += M_PI_Float;
        }
      }

      //Arrow arc center
      QPointF arcCenter = QPointF(
        glyphCenter.x( ) + dist * cosf( relativeAngle ),
        glyphCenter.y( ) + dist * sinf( relativeAngle ));

      //Angle where the arrow arc will start
      relativeAngle = startAngle + M_PI_Float - relativeAngle;

      //Arrow start
      _arrowOrigin = QPointF(
        arcCenter.x( ) + arcRadius * cosf( relativeAngle ),
        arcCenter.y( ) - arcRadius * sinf( relativeAngle ));

      //Arrow end
      _arrowDest = QPointF(
        arcCenter.x( ) + arcRadius * cosf( relativeAngle + arcDegrees ),
        arcCenter.y( ) - arcRadius * sinf( relativeAngle + arcDegrees ));

      //Data necessary to draw the arrow
      float arrowWidth = 0.23f * nslib::Config::scale( ) * arcRadius;
      float arrowAngle = M_PI_0825 + arcDegrees + relativeAngle;

      QPointF arrowHead1 = _arrowDest - QPointF(
        sinf( arrowAngle ) * arrowWidth, cosf( arrowAngle ) * arrowWidth );

      arrowAngle = M_PI_1115 + arcDegrees + relativeAngle;

      QPointF arrowHead2 = _arrowDest - QPointF(
        sinf( arrowAngle ) * arrowWidth, cosf( arrowAngle ) * arrowWidth );

      //Creates arrowhead
      QPolygonF arrowShape;
      arrowShape << arrowHead1 << _arrowDest << arrowHead2;

      //Creates the path where the arrow will be drawn
      auto painterPath = QPainterPath( );

      //Draws the back of the arrow
      if( arcDegrees < M_PI_x2 )
      {
        QPointF relativeDir = _arrowOrigin - glyphCenter;

        Eigen::Vector2d vector( relativeDir.x( ), relativeDir.y( ));
        vector.normalize( );
        vector = vector * arrowWidth;

        QPointF arrowBack = QPointF( -vector.y( ), vector.x( ));

        QPointF arrowI1 = _arrowOrigin + arrowBack;
        QPointF arrowI2 = _arrowOrigin - arrowBack;

        painterPath.moveTo( arrowI1 );
        painterPath.lineTo( arrowI2 );
      }

      painterPath.moveTo( _arrowOrigin );

      //Draws the complete arrow with the calculated data
      painterPath.arcTo( arcCenter.x( ) - arcRadius,
        arcCenter.y( ) - arcRadius, arcRadius * 2.0f, arcRadius * 2.0f,
        relativeAngle * Rad_To_Deg, arcDegrees * Rad_To_Deg );

      painterPath.addPolygon( arrowShape );

      this->setBrush( QBrush( QColor( 0, 0, 0, 0 )));
      this->setPen( QPen( QBrush( color ), _arrowThickness ));
      this->setPath( painterPath );
      this->setZValue( -100.0f );

    }

    void AutoConnectionArrowItem::recalcArcData( )
    {
      //Arrow arc radius
      arcRadius = ( glyphRadius * _arcSizeFactor );

      //Distance between arrow and glyph centres
      dist = glyphRadius + arcRadius * ( _centersDistFactor );

      if( _centersDistFactor < 1 )
      {
        //Angle were arcs start
        startAngle = acosf(
          ( arcRadius * arcRadius + dist * dist - glyphRadius * glyphRadius ) /
            ( 2.0f * arcRadius * dist ));
      }
      else
      {
        startAngle = 0.0f;
      }
      //Degrees covered by the arc
      arcDegrees = M_PI_x2 - startAngle - startAngle;
    }

    void AutoConnectionArrowItem::hoverEnter( void )
    {
      this->setZValue( 100 );
      this->setBrush( QBrush( Qt::NoBrush ));
      this->setPen( QPen( QBrush( hoverColor ), _arrowThickness ));

      if( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen(
          QPen( QBrush( hoverColor ), _arrowThickness ));
        _arrowCircleEnd->setBrush( QBrush( hoverColor ));
      }
    }

    void AutoConnectionArrowItem::hoverLeaveEvent(
      QGraphicsSceneHoverEvent* event_ )
    {
      auto rep = dynamic_cast< ConnectionArrowRep* >( _parentRep );
      if( rep )
      {
        rep->hoverLeaveEvent( event_ );
      }
    }

    void AutoConnectionArrowItem::hoverLeave( void )
    {
      this->setZValue( -100 );
      this->setBrush( QBrush( Qt::NoBrush ));
      this->setPen( QPen( QBrush( color ), _arrowThickness ));
      if( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( QPen( QBrush( color ), _arrowThickness ));
        _arrowCircleEnd->setBrush( QBrush( Qt::NoBrush ));
      }
    }

    void AutoConnectionArrowItem::highlight( scoop::Color color_ )
    {
      this->setZValue( 100 );
      this->setBrush( QBrush( Qt::NoBrush ));
      this->setPen(
        QPen( QBrush( QColor( 50, 206, 22 )/*Green*/ ), _arrowThickness ));
      if( _arrowCircleEnd != nullptr )
      {
        _arrowCircleEnd->setPen( QPen( QBrush( color_ ), _arrowThickness ));
        _arrowCircleEnd->setBrush( QColor( 50, 206, 22 )/*Green*/ );
      }
    }

    void
    AutoConnectionArrowItem::centersDistFactor( float centersDistFactor_ )
    {
      _centersDistFactor = centersDistFactor_;
      glyphRadius = 0.0f;
    }

    void AutoConnectionArrowItem::arcSizeFactor( float arcSizeFactor_ )
    {
      _arcSizeFactor = arcSizeFactor_;
      glyphRadius = 0.0f;

    }

  } // namespace congen
} // namespace nslib
