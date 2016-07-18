#include "ColumnItem.h"
#include "NeuronItem.h"
#include <QPen>

namespace neuroscheme
{
  ColumnItem::ColumnItem( shiftgen::NeuronRep meanNeuron,
                          shiftgen::ColumnRep::ColumnLayers, // layers,
                          unsigned int size )
  {

    // Create the polygon for the basic column icon
    QPainterPath path_;
    QPolygon poly;

    QPoint pUL ( -int( size )/3, -int( size )/4 );
    QPoint pUR (  int( size )/3, -int( size )/4 );
    QPoint pLR (  int( size )/3, +int( size )/24 );
    QPoint pLM (            0, +int( size )/6 );
    QPoint pLL ( -int( size )/3, +int( size )/24 );

    poly << pUL << pUR << pLR << pLM << pLL;

    path_.addPolygon( poly );
    path_.closeSubpath(  );

    this->setPath( path_ );
    this->setPen( Qt::NoPen );
    this->setBrush( QBrush( QColor( 176,175,204 )));


    unsigned int nrSize = size/6;

    // Container for neuron
    QGraphicsEllipseItem *nrCont = new QGraphicsEllipseItem( this );

    nrCont->setRect( -int( nrSize ) * 0.8f,  -int( nrSize ) * 0.8f,
                    nrSize * 1.6f,  nrSize * 1.6f );

    nrCont->setPen( Qt::NoPen );
    nrCont->setBrush( QBrush( QColor( 255, 255, 255 )));
    nrCont->setPos( 0, -int( size ) / 16 );

    auto _meanNeuronItem = new NeuronItem(
      meanNeuron.getProperty( "bg" ).value< Color >( ),
      meanNeuron.getProperty( "symbol" ).value< NeuronRep::TSymbol >( ),
      meanNeuron.getProperty( "rings" ).value< NeuronRep::Rings >( ),
      size / 6 );

    _meanNeuronItem->setParentItem( nrCont );

  }

} // namespace neuroscheme
