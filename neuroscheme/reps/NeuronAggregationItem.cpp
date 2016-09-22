#include "NeuronAggregationItem.h"
#include "NeuronItem.h"
#include "CollapseButtonItem.h"
#include "error.h"
#include <QPen>

namespace neuroscheme
{

  LayerItem::LayerItem( unsigned int layer_,
                                    QGraphicsItem *parent_,
                                    const QPoint& pLayerUL,
                                    const QPoint& pLayerUM,
                                    const QPoint& pLayerUR,
                                    unsigned int layerHeight,
                                    unsigned int numNeuronsHeight,
                                    float percPyr,
                                    float percInter,
                                    const QBrush& brush_ )
  : QGraphicsPathItem( parent_ )
  , _layer( layer_  )
  {

    QPainterPath layerPath;
    QPolygon layerPoly;

    QPoint pLayerLR( pLayerUR + QPoint( 0, layerHeight ) );
    QPoint pLayerLM( pLayerUM + QPoint( 0, layerHeight ) );
    QPoint pLayerLL( pLayerUL + QPoint( 0, layerHeight ) );

    layerPoly << pLayerUL << pLayerUM << pLayerUR
              << pLayerLR << pLayerLM << pLayerLL;

    layerPath.addPolygon( layerPoly );
    layerPath.closeSubpath(  );

    this->setPath( layerPath );
    this->setPen( Qt::NoPen );
    this->setBrush( brush_ );

    {
      float numNeuronsPercWidth = percPyr;
      QPoint pWidget = ( pLayerLL - pLayerLM ) * numNeuronsPercWidth;
      QPolygon numNeuronsPoly;
      numNeuronsPoly << pLayerLM + pWidget - QPoint( 0,numNeuronsHeight )
                     << pLayerLM - QPoint( 0,numNeuronsHeight )
                     << pLayerLM
                     << pLayerLM + pWidget;

      layerPath = QPainterPath(  );
      layerPath.addPolygon( numNeuronsPoly );
      layerPath.closeSubpath(  );

      QGraphicsPathItem * numNeuronsPathItem =
        new QGraphicsPathItem( this );
      numNeuronsPathItem->setPath( layerPath );
      numNeuronsPathItem->setPen( Qt::NoPen );
      numNeuronsPathItem->setBrush( QBrush( QColor( 200,100,100 )));

    }
    {
      float numNeuronsPercWidth = percInter;
      QPoint pWidget = ( pLayerLR - pLayerLM ) * numNeuronsPercWidth;

      QPolygon numNeuronsPoly;
      numNeuronsPoly << pLayerLM + pWidget - QPoint( 0,numNeuronsHeight )
                     << pLayerLM - QPoint( 0,numNeuronsHeight )
                     << pLayerLM
                     << pLayerLM + pWidget;

      layerPath = QPainterPath(  );
      layerPath.addPolygon( numNeuronsPoly );
      layerPath.closeSubpath(  );

      QGraphicsPathItem * numNeuronsPathItem =
        new QGraphicsPathItem( this );
      numNeuronsPathItem->setPath( layerPath );

      numNeuronsPathItem->setPen( Qt::NoPen );
      numNeuronsPathItem->setBrush( QBrush( QColor( 200,100,100 )));
    }
  }

  unsigned int & LayerItem::layer( void )
  {
    return _layer;
  }

  void NeuronAggregationItem::_createNeuronAggregationItem(
    const NeuronRep& meanNeuron,
    const Layers& layers,
    const QPainterPath& path_,
    const QPoint& layerLeftPoint,
    const QPoint& layerCenterPoint,
    const QPoint& layerRightPoint,
    const QColor& baseColor,
    unsigned int size )
  {
    this->setPath( path_ );
    this->setPen( Qt::NoPen );
//    this->setBrush( QBrush( QColor( 114, 188, 196 )));
    this->setBrush( QBrush( baseColor ));

    unsigned int nrSize = size/7;

    // Container for neuron
    QGraphicsEllipseItem *nrCont = new QGraphicsEllipseItem( this );

    nrCont->setRect( -int( nrSize ) * 0.8f,  -int( nrSize ) * 0.8f,
                    nrSize * 1.6f,  nrSize * 1.6f );

    nrCont->setPen( Qt::NoPen );
    nrCont->setBrush( QBrush( QColor( 255, 255, 255 )));
    //nrCont->setPos( 0, -int( size ) / 16 );

    auto _meanNeuronItem = new NeuronItem( meanNeuron, nrSize, false );
    // To avoid destruction of parent which is not dynamically allocated
    _meanNeuronItem->parentRep( nullptr );

    _meanNeuronItem->setParentItem( nrCont );

    // Collapse button
    // Create uncollapse icon
    QGraphicsEllipseItem* collapseButton;
    QGraphicsLineItem* collapseItemHorizontalLine;

    int collapseButtonSize = int( roundf( float( size ) * 0.04f ));
    QPoint collapseButtonPos( 0, int( size ) / 6 );
    collapseButton = new CollapseButtonItem( );
    collapseButton->setRect( - collapseButtonSize / 2,
                              - collapseButtonSize / 2,
                              collapseButtonSize,
                              collapseButtonSize );

    collapseButton->setPen( Qt::NoPen );
    collapseButton->setBrush( QBrush( QColor( 85, 85, 100 )));
    collapseButton->setPos( collapseButtonPos );

    collapseButton->setParentItem( this );

    collapseItemHorizontalLine =
      new QGraphicsLineItem( - collapseButtonSize / 3, 0,
                             collapseButtonSize / 3, 0 );
    collapseItemHorizontalLine->setPen( QPen( QColor( 255, 255, 255 ),
                                               collapseButtonSize / 9 ));
    collapseItemHorizontalLine->setOpacity( 1 );
    collapseItemHorizontalLine->setParentItem( collapseButton );

    _collapseItemVerticalLine =
      new QGraphicsLineItem( 0, - collapseButtonSize / 3,
                             0, collapseButtonSize / 3 );
    _collapseItemVerticalLine->setPen( QPen( QColor( 255, 255, 255 ),
                                             collapseButtonSize / 9));
    _collapseItemVerticalLine->setOpacity(1);
    _collapseItemVerticalLine->setParentItem( collapseButton );

    // Layers
    // const auto& layers =
    //   columnRep.getProperty( "layers" ).value< ColumnRep::Layers >( );
    NEUROSCHEME_DEBUG_CHECK(
      layers.size( ) == 7,
      "neuroscheme::ColumnItem: incorrect number of LayerReps" );
    unsigned int layerHeight = size / 20;
    unsigned int layerPadding = size/100;
    for ( int i = 1; i <= 6; i++ )
    {
      QPoint pLayerUL =
        layerCenterPoint +
        ( layerLeftPoint - layerCenterPoint ) * 0.8 +
        QPoint( 0, layerPadding + ( i - 1 ) * ( layerHeight + layerPadding ));
      QPoint pLayerUM =
        layerCenterPoint +
        QPoint( 0, layerPadding + ( i - 1 ) * (layerHeight + layerPadding ));
      QPoint pLayerUR =
        layerRightPoint -
        ( layerRightPoint - layerCenterPoint ) * ( 1 - 0.8f ) +
        QPoint( 0, layerPadding + ( i - 1 ) * ( layerHeight + layerPadding ));

      const float& percPyr =
        layers[ i ].getProperty( "leftPerc" ).value< float >( );
      const float& percInter =
        layers[ i ].getProperty( "rightPerc" ).value< float >( );

      auto layerItem =
        new LayerItem( i,
                       collapseButton,
                       pLayerUL - collapseButtonPos,
                       pLayerUM - collapseButtonPos,
                       pLayerUR - collapseButtonPos,
                       layerHeight, size/40,
                       percPyr, percInter,
                       QBrush(
                         QColor( std::min( baseColor.red( ) + i * 8, 255 ),
                                 std::min( baseColor.green( ) + i * 8, 255 ),
                                 std::min( baseColor.blue( ) + i * 5 , 255 ))));
      layerItem->setFlag( QGraphicsItem::ItemStacksBehindParent );
      _layerItems[ i - 1 ] = layerItem;
      _layerAnimations[ i - 1 ] =
        new QPropertyAnimation( layerItem, "opacity" );

    }
    this->collapse( false ); // Start collapsed without ani

  }


  void NeuronAggregationItem::collapse( bool anim )
  {
    _collapseItemVerticalLine->show( );

    unsigned int count = 0;
    for ( int i = 5; i >= 0; --i )
    {
      if ( anim )
      {
        _layerAnimations[ i ]->setDuration( 200 + count++ * 80 );
        _layerAnimations[ i ]->setStartValue( 1 );
        _layerAnimations[ i ]->setEndValue( 0 );

        _layerAnimations[ i ]->start( );
      }
      else
      {
        _layerItems[ i ]->setEnabled( false );
        _layerItems[ i ]->setVisible( false );
      }


    }
    _collapsed = true;


  }

  void NeuronAggregationItem::uncollapse( bool anim )
  {
    _collapseItemVerticalLine->hide( );

    unsigned int count = 0;
    for ( unsigned int i = 0; i < 6; ++i )
    {
      _layerAnimations[ i ]->disconnect( SIGNAL( finished( )));
      _layerItems[ i ]->setEnabled( true );
      _layerItems[ i ]->setVisible( true );

      if ( anim )
      {
        _layerAnimations[ i ]->setDuration( 200 + count++ * 80 );
        _layerAnimations[ i ]->setStartValue( 0 );
        _layerAnimations[ i ]->setEndValue( 1 );

        _layerAnimations[ i ]->start( );
      }
      else
        _layerItems[ i ]->setOpacity( 1 );
    }
    _collapsed = false;
  }


} // namespace neuroscheme
