#include "../../error.h"
#include "../../reps/CollapseButtonItem.h"
#include "ColumnItem.h"
#include "NeuronItem.h"
#include <QPen>

namespace neuroscheme
{

  ColumnItem::ColumnItem( const ColumnRep& columnRep,
                          QGraphicsScene* scene,
                          unsigned int size )
    : NeuronAggregationItem( )
  {

    const NeuronRep& meanNeuron =
      columnRep.getProperty( "meanNeuron" ).value< NeuronRep >( );
    const auto& layers =
      columnRep.getProperty( "layers" ).value< ColumnRep::Layers >( );
    const auto& neuronAggReps =
      columnRep.getProperty( "neuronTypeAggregations" ).
      value< ColumnRep::NeuronTypeAggregations >( );

    // Create the polygon for the basic column icon
    QPainterPath path_;
    QPolygon poly;

    QPoint pUL ( -int( size )/3, -int( size )/6 );
    QPoint pUR (  int( size )/3, -int( size )/6 );
    QPoint pLR (  int( size )/3, +int( size )/24 );
    QPoint pLM (              0, +int( size )/6 );
    QPoint pLL ( -int( size )/3, +int( size )/24 );

    poly << pUL << pUR << pLR << pLM << pLL;

    path_.addPolygon( poly );
    path_.closeSubpath(  );

    _createNeuronAggregationItem(
      scene,
      meanNeuron,
      layers,
      neuronAggReps,
      path_,
      pLL, pLM, pLR,
      QColor( 114, 188, 196 ),
      size );

    this->_parentRep = &( const_cast< ColumnRep& >( columnRep ));
    //  this->setBrush( QBrush( QColor( 114, 188, 196 )));
  }

} // namespace neuroscheme
