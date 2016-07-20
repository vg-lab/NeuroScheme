#include "NeuronItem.h"
#include "RingItem.h"
#include <QPen>

namespace neuroscheme
{
  NeuronItem::NeuronItem( const NeuronRep& neuronRep,
                          unsigned int size )
  {
    const Color& bgColor = neuronRep.getProperty( "bg" ).value< Color >( );
    const NeuronRep::TSymbol& symbol =
      neuronRep.getProperty( "symbol" ).value< NeuronRep::TSymbol >( );
    const NeuronRep::Rings rings =
      neuronRep.getProperty( "rings" ).value< NeuronRep::Rings >( );

    auto somaItem = new QGraphicsEllipseItem( );
    somaItem->setRect( - int( size ) / 2,
                       - int( size ) / 2,
                       size,
                       size );
    somaItem->setPen( Qt::NoPen );
    somaItem->setBrush( QBrush( QColor( bgColor.r( ),
                                        bgColor.g( ),
                                        bgColor.b( ))));

    QGraphicsItem* symbolItem = _createSymbolItem( symbol, size );

    const unsigned int ringItemsWidth = int( size / 10 );
    const unsigned int ringItemPadding = 2;

    somaItem->setParentItem( this );
    if ( symbolItem )
       symbolItem->setParentItem( this );

    unsigned int ringCount = 1;
    for ( const auto& ring : rings )
    {
      RingItem* ringItem =
        new RingItem(
          90,
          size / 2 + ( ringItemPadding + ringItemsWidth ) * ringCount,
          size / 2 + ( ringItemPadding + ringItemsWidth ) * ringCount,
          ringItemsWidth,
          ring.getProperty( "angle" ).value< int >( ),
          ring.getProperty( "color" ).value< Color >( ));

      ringItem->setParentItem( this );
      ringItem->setPen( Qt::NoPen );
      ringItem->setZValue( -1 );

      ringCount++;
    }

  }

  QGraphicsItem* NeuronItem::_createSymbolItem( NeuronRep::TSymbol symbol,
                                                unsigned int size )
  {
    QGraphicsItem* symbolItem = nullptr;

    switch ( symbol )
    {
    case NeuronRep::TRIANGLE:
    {
      QPolygonF triangle;
      triangle.append( QPointF(                 0, - int( size ) / 3 ));
      triangle.append( QPointF(          size / 3, size/4 ));
      triangle.append( QPointF( - int( size ) / 3, size / 4 ));
      triangle.append( QPointF(                 0, - int( size ) / 3 ));

      auto triangleItem =
        new QGraphicsPolygonItem( triangle );

      triangleItem->setPen( Qt::NoPen );
      triangleItem->setBrush( QBrush( QColor( 255, 255, 255 )));
      triangleItem->setX( 0 );
      triangleItem->setY( - int( size ) /20 );

      symbolItem = triangleItem;

      break;
    }
    case NeuronRep::CIRCLE:
    {

      auto circleItem = new QGraphicsEllipseItem( );
      circleItem->setRect( - int( size ) / 4, - int( size ) / 4,
                           size / 2, size / 2);
      circleItem->setPen( Qt::NoPen );
      circleItem->setBrush( QBrush( QColor(255, 255, 255 )));

      symbolItem = circleItem;

      break;
    }
    default:
      break;
      // no symbol needed
    }

    return symbolItem;
  }


} // namespace neuroscheme
