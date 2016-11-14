#include "../../error.h"
#include "../../reps/CollapseButtonItem.h"
#include "MiniColumnItem.h"
#include "NeuronItem.h"
#include <QPen>

namespace neuroscheme
{

  MiniColumnItem::MiniColumnItem( const MiniColumnRep& miniColumnRep,
                                  QGraphicsScene* scene,
                                  unsigned int size )
    : NeuronAggregationItem( )
  {
    // std::cout << "props: " << miniColumnRep.properties( ).size( ) << std::endl;;
    // for ( const auto p : miniColumnRep.properties( ))
    //   std::cout << p.first << std::endl;
    // const auto& shiftNeuronRep =
    //   miniColumnRep.getProperty( "meanNeuron" ).value< shiftgen::NeuronRep >( );
    // NeuronRep meanNeuron( shiftNeuronRep );

    const NeuronRep& meanNeuron =
      miniColumnRep.getProperty( "meanNeuron" ).value< NeuronRep >( );
    const auto& layers =
      miniColumnRep.getProperty( "layers" ).value< MiniColumnRep::Layers >( );
    const auto& neuronAggReps =
      miniColumnRep.getProperty( "neuronTypeAggregations" ).
      value< MiniColumnRep::NeuronTypeAggregations >( );

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
      scene,
      meanNeuron,
      layers,
      neuronAggReps,
      path_,
      pLL, pLM, pLR,
      QColor( 126, 204, 145 ),
      size );

    this->_parentRep = &( const_cast< MiniColumnRep& >( miniColumnRep ));

    //  this->setBrush( QBrush( QColor( 114, 188, 196 )));
  }

} // namespace neuroscheme
