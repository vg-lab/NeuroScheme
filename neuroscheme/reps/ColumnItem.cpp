#include "../error.h"
#include "ColumnItem.h"
#include "NeuronItem.h"
#include <QPen>

namespace neuroscheme
{
  CollapseButton::CollapseButton( void )
  {
    this->setAcceptHoverEvents( true );
  }

  void CollapseButton::mousePressEvent( QGraphicsSceneMouseEvent* event )
  {
    // Toggle collapse
    if ( event->buttons( ) & Qt::LeftButton )
    {
      CollapsableItem*  pI =
        dynamic_cast< CollapsableItem* >( this->parentItem( ));
      if ( pI )
        pI->toggleCollapse( );
    }
  }

  void CollapseButton::hoverEnterEvent(
    QGraphicsSceneHoverEvent* /* event */ )
  {
    this->setBrush( QBrush( QColor( 120, 85, 100 )));
  }

  void CollapseButton::hoverLeaveEvent(
    QGraphicsSceneHoverEvent* /* event */ )
  {
    this->setBrush( QBrush( QColor( 85, 85, 100 )));
  }

  ColumnLayerItem::ColumnLayerItem( unsigned int layer_,
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
    QPoint pLayerLM ( pLayerUM + QPoint( 0, layerHeight ) );
    QPoint pLayerLL ( pLayerUL + QPoint( 0, layerHeight ) );

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

  unsigned int & ColumnLayerItem::layer( void )
  {
    return _layer;
  }



  ColumnItem::ColumnItem( const ColumnRep& columnRep,
                          unsigned int size )
    : _collapseItemVerticalLine( nullptr )
  {

    const NeuronRep& meanNeuron =
      columnRep.getProperty( "meanNeuron" ).value< NeuronRep >( );

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
    this->setBrush( QBrush( QColor( 114, 188, 196 )));

    unsigned int nrSize = size/6;

    // Container for neuron
    QGraphicsEllipseItem *nrCont = new QGraphicsEllipseItem( this );

    nrCont->setRect( -int( nrSize ) * 0.8f,  -int( nrSize ) * 0.8f,
                    nrSize * 1.6f,  nrSize * 1.6f );

    nrCont->setPen( Qt::NoPen );
    nrCont->setBrush( QBrush( QColor( 255, 255, 255 )));
    nrCont->setPos( 0, -int( size ) / 16 );

    auto _meanNeuronItem = new NeuronItem( meanNeuron, size / 6 );

    _meanNeuronItem->setParentItem( nrCont );

    // Collapse button
    // Create uncollapse icon
    QGraphicsEllipseItem* collapseButton;
    QGraphicsLineItem* collapseItemHorizontalLine;

    int collapseButtonSize = int( roundf( float( size ) * 0.04f ));
    QPoint collapseButtonPos( 0, int( size ) / 6 );
    collapseButton = new CollapseButton( );
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
      new QGraphicsLineItem(0, - collapseButtonSize / 3,
                            0, collapseButtonSize / 3 );
    _collapseItemVerticalLine->setPen( QPen( QColor( 255, 255, 255 ),
                                            collapseButtonSize / 9));
    _collapseItemVerticalLine->setOpacity(1);
    _collapseItemVerticalLine->setParentItem( collapseButton );

    // Layers
    const auto& layers =
      columnRep.getProperty( "layers" ).value< ColumnRep::ColumnLayers >( );
    NEUROSCHEME_DEBUG_CHECK(
      layers.size( ) == 7,
      "neuroscheme::ColumnItem: incorrect number of LayerReps" );
    unsigned int layerHeight = size / 20;
    unsigned int layerPadding = size/100;
    for ( int i = 1; i <= 6; i++ )
    {
      QPoint pLayerUL = pLM + ( pLL - pLM ) * 0.8 +
        QPoint( 0, layerPadding + ( i - 1 ) * ( layerHeight + layerPadding ));
      QPoint pLayerUM = pLM +
        QPoint( 0, layerPadding + ( i - 1 ) * (layerHeight + layerPadding ));
      QPoint pLayerUR = pLR - ( pLR - pLM ) * ( 1 - 0.8f ) +
        QPoint( 0, layerPadding + ( i - 1 ) * ( layerHeight + layerPadding ));

      const float& percPyr =
        layers[ i ].getProperty( "leftPerc" ).value< float >( );
      const float& percInter =
        layers[ i ].getProperty( "rightPerc" ).value< float >( );;

      auto layerItem =
        new ColumnLayerItem( i,
                             collapseButton,
                             pLayerUL - collapseButtonPos,
                             pLayerUM - collapseButtonPos,
                             pLayerUR - collapseButtonPos,
                             layerHeight, size/40,
                             percPyr, percInter,
                             QBrush( QColor( 114 + i * 10,
                                             188 + i * 10,
                                             196 + i * 5 )));
      layerItem->setFlag( QGraphicsItem::ItemStacksBehindParent );
      _layerItems[ i - 1 ] = layerItem;
      _layerAnimations[ i - 1 ] =
        new QPropertyAnimation( layerItem, "opacity" );

    }
    this->collapse( false ); // Start collapsed without anim
  }

  void ColumnItem::collapse( bool anim )
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

  void ColumnItem::uncollapse( bool anim )
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
