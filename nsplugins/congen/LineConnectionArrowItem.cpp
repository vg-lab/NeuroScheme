//
// Created by iagocl on 25/01/18.
//

#include <nslib/Config.h>
#include "LineConnectionArrowItem.h"



namespace nslib
{
  namespace congen
  {

    LineConnectionArrowItem::LineConnectionArrowItem(
        const LineConnectionArrowRep& connectionArrowRep )
      {
      _arrowCircleEnd = nullptr;
      this->_parentRep =
          &( const_cast< LineConnectionArrowRep& >( connectionArrowRep ) );


      this->setAcceptHoverEvents( true );

      _arrowThickness = nslib::Config::scale( ) *
                        connectionArrowRep.getProperty(
                            "width" ).value<unsigned int>( );
      }
    void LineConnectionArrowItem::setLine( const QLineF& line_ )
    {
      _line = line_;
      createArrow( _line.p1( ), _line.p2( ));
    }

    void LineConnectionArrowItem::createArrow( const QPointF& origin,
                                           const QPointF& dest )
    {
      _arrowOrigin  = origin;
      _arrowDest    = dest;

      QPolygonF arrowShape;

      float arrowWidth  = 6 * nslib::Config::scale( );
      float arrowLength = 3 * nslib::Config::scale( );

      QLineF auxLine( origin, dest );

      auto lengthInv = 1.0f / auxLine.length( );

      double angle = ::acos( auxLine.dx( ) * lengthInv );
      if ( auxLine.dy( ) >= 0 )
        angle = ( M_PI * 2.0 ) - angle;

      QPointF arrowInit = auxLine.pointAt(
          1.0f - (arrowLength * lengthInv ));
      QPointF arrowP1 = arrowInit -
                        QPointF( sin( angle + M_PI_3 ) * arrowWidth,
                                 cos( angle + M_PI_3 ) * arrowWidth );
      QPointF arrowP2 = arrowInit -
                        QPointF( sin(angle + M_PI - M_PI_3 ) * arrowWidth,
                                 cos( angle + M_PI - M_PI_3 ) * arrowWidth );

      QPointF arrowI1 = _arrowOrigin -
                        QPointF( sin( angle + M_PI ) * arrowWidth,
                                 cos( angle + M_PI ) * arrowWidth );
      QPointF arrowI2 = _arrowOrigin +
                        QPointF( sin(angle - M_PI ) * arrowWidth,
                                 cos( angle - M_PI ) * arrowWidth );


      float size = arrowLength;

      /*
      if ( _arrowOriItem != nullptr ) delete _arrowOriItem;
      _arrowOriItem = new QGraphicsEllipseItem( );
      _arrowOriItem->setRect( origin.x( ) - size * 0.5f,
                              origin.y( ) - size * 0.5f,
                              size,
                              size );

      _arrowOriItem->setPen( Qt::NoPen );
      _arrowOriItem->setBrush( QBrush( color ));
      _arrowOriItem->setPen( QPen( QBrush( color ), _arrowThickness ));
      _arrowOriItem->setParentItem( this );
       */

      arrowShape.clear( );

      if ( this->_parentRep->getProperty( "head" ).
          value< shiftgen::ConnectionArrowRep::TArrowHead >( ) ==
           shiftgen::ConnectionArrowRep::TArrowHead::CIRCLE )
      {
        _arrowCircleEnd = new QGraphicsEllipseItem( );
        _arrowCircleEnd->setRect( dest.x( ) - size * 0.5f,
                                  dest.y( ) - size * 0.5f,
                                  size,
                                  size );

        _arrowCircleEnd->setPen( Qt::NoPen );
        _arrowCircleEnd->setBrush( QBrush( color ));
        _arrowCircleEnd->setPen( QPen( QBrush( color ), _arrowThickness ));
        _arrowCircleEnd->setParentItem( this );

        arrowShape  << arrowI1
                    << arrowI2
                    << auxLine.p1( )
                    << auxLine.p2( )
                    << auxLine.p1( );
      }
      else
      {
        arrowShape  << arrowI1
                    << arrowI2
                    << auxLine.p1( )
                    << arrowInit
                    << arrowP1
                    << auxLine.p2( )
                    << arrowP2
                    << arrowInit
                    << auxLine.p1( );
      }

      this->setBrush( QBrush( color ));
      this->setPen( QPen( QBrush( color ), _arrowThickness ));
      this->setPolygon( arrowShape );
      this->setZValue( -100.0f );

    }

  } // namespace congen
} // namespace nslib
