#include "../error.h"
#include "MiniColumnItem.h"
#include "NeuronItem.h"
#include "CollapseButtonItem.h"
#include <QPen>

namespace neuroscheme
{

  MiniColumnItem::MiniColumnItem( const MiniColumnRep& columnRep,
                                  unsigned int size )
    : NeuronAggregationItem( )
  {

    const NeuronRep& meanNeuron =
      columnRep.getProperty( "meanNeuron" ).value< NeuronRep >( );
    const auto& layers =
      columnRep.getProperty( "layers" ).value< MiniColumnRep::Layers >( );

    // Create the polygon for the basic column icon
    QPainterPath path_;
    QPolygon poly;

    QPoint pUL (-int(size)/3, -int(size)/24);
    QPoint pUM (           0, -int(size)/6);
    QPoint pUR ( int(size)/3, -int(size)/24);

    QPoint pLR ( int(size)/3, +int(size)/24);
    QPoint pLM (           0, +int(size)/6);
    QPoint pLL (-int(size)/3, +int(size)/24);

    poly << pLR << pLM << pLL
         << pUL << pUM << pUR;

    path_.addPolygon( poly );
    path_.closeSubpath(  );
    _createNeuronAggregationItem(
      meanNeuron,
      layers,
      path_,
      pLL, pLM, pLR,
      QColor( 126, 204, 145 ),
      size );

    //  this->setBrush( QBrush( QColor( 114, 188, 196 )));
  }

} // namespace neuroscheme
