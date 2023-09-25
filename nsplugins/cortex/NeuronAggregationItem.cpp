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
#include "NeuronAggregationItem.h"
#include "NeuronTypeAggregationRep.h"
#include "NeuronTypeAggregationItem.h"
#include "NeuronItem.h"
#include <nslib/reps/CollapseButtonItem.h>
#include <nslib/error.h>
#include <QPen>

namespace nslib
{
  namespace cortex
  {
    LayerItem::LayerItem( const LayerRep& layerRep )
    : _layer{0}
    {
      setAcceptHoverEvents( true );
      setFlags( QGraphicsItem::ItemIsPanel );
      assert( this->isPanel( ));
      this->_parentRep = &( const_cast< LayerRep& >( layerRep ));
    }

    void LayerItem::create(
      const unsigned int layer_,
      QGraphicsItem *parent_,
      const QPoint& pLayerUL,
      const QPoint& pLayerUM,
      const QPoint& pLayerUR,
      unsigned int layerHeight,
      unsigned int numNeuronsHeight,
      float percPyr,
      float percInter,
      const QBrush& brush_ )
    {
      assert( parent_ );
      _layer = layer_;
      setParentItem( parent_ );

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
        const float numNeuronsPercWidth = percPyr;
        QPoint pWidget = ( pLayerLL - pLayerLM ) * numNeuronsPercWidth;
        QPolygon numNeuronsPoly;
        numNeuronsPoly << pLayerLM + pWidget - QPoint( 0,numNeuronsHeight )
                       << pLayerLM - QPoint( 0,numNeuronsHeight )
                       << pLayerLM
                       << pLayerLM + pWidget;

        layerPath = QPainterPath(  );
        layerPath.addPolygon( numNeuronsPoly );
        layerPath.closeSubpath(  );

        QGraphicsPathItem* numNeuronsPathItem =
          new QGraphicsPathItem( this );
        numNeuronsPathItem->setPath( layerPath );
        numNeuronsPathItem->setPen( Qt::NoPen );
        numNeuronsPathItem->setBrush( QBrush( QColor( 200,100,100 )));
      }

      {
        const float numNeuronsPercWidth = percInter;
        QPoint pWidget = ( pLayerLR - pLayerLM ) * numNeuronsPercWidth;

        QPolygon numNeuronsPoly;
        numNeuronsPoly << pLayerLM + pWidget - QPoint( 0,numNeuronsHeight )
                       << pLayerLM - QPoint( 0,numNeuronsHeight )
                       << pLayerLM
                       << pLayerLM + pWidget;

        layerPath = QPainterPath(  );
        layerPath.addPolygon( numNeuronsPoly );
        layerPath.closeSubpath(  );

        QGraphicsPathItem* numNeuronsPathItem =
          new QGraphicsPathItem( this );
        numNeuronsPathItem->setPath( layerPath );

        numNeuronsPathItem->setPen( Qt::NoPen );
        numNeuronsPathItem->setBrush( QBrush( QColor( 200,100,100 )));
      }
    }

    unsigned int& LayerItem::layer( void )
    {
      return _layer;
    }

  void NeuronAggregationItem::_createNeuronAggregationItem(
    QGraphicsScene* scene_,
    const NeuronRep* meanNeuron,
    const Layers& layers,
    const NeuronTypeAggregations& neuronTypeAggs,
    const QPainterPath& path_,
    const QPoint& layerLeftPoint,
    const QPoint& layerCenterPoint,
    const QPoint& layerRightPoint,
    const QColor& baseColor,
    unsigned int size )
  {
    this->setPath( path_ );
    this->setPen( Qt::NoPen );
    this->setBrush( QBrush( baseColor ));

    const unsigned int nrSize = size/7;
    const int inrSize = static_cast<int>(nrSize);

    // Container for neuron
    QGraphicsEllipseItem *nrCont = new QGraphicsEllipseItem( this );

    nrCont->setRect( -inrSize * 0.8f,  -inrSize * 0.8f,
                     nrSize * 1.6f,  nrSize * 1.6f );

    nrCont->setPen( Qt::NoPen );
    nrCont->setBrush( QBrush( QColor( 255, 255, 255 )));

    auto _meanNeuronItem = new NeuronItem( meanNeuron, nrSize, false );

    // To avoid destruction of parent which is not dynamically allocated
    //_meanNeuronItem->parentRep( nullptr );
    _meanNeuronItem->setParentItem( nrCont );

    // Collapse button
    // Create uncollapse icon
    QGraphicsEllipseItem* collapseButton;
    QGraphicsLineItem* collapseItemHorizontalLine;

    const int collapseButtonSize = static_cast<int>( roundf( static_cast<float>( size ) * 0.04f ));
    QPoint collapseButtonPos( 0, static_cast<int>( size ) / 6 );
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

    NEUROSCHEME_DEBUG_CHECK(
      layers.size( ) == 7,
      "nslib::ColumnItem: incorrect number of LayerReps" );
    const unsigned int layerHeight = size / 20;
    const unsigned int layerPadding = size/100;
    for ( int layer = 1; layer <= 6; ++layer )
    {
      QPoint pLayerUL =
        layerCenterPoint +
        ( layerLeftPoint - layerCenterPoint ) * 0.8 +
        QPoint( 0, layerPadding + ( layer - 1 ) *
                ( layerHeight + layerPadding ));
      QPoint pLayerUM =
        layerCenterPoint +
        QPoint( 0, layerPadding + ( layer - 1 ) *
                (layerHeight + layerPadding ));
      QPoint pLayerUR =
        layerRightPoint -
        ( layerRightPoint - layerCenterPoint ) * ( 1 - 0.8f ) +
        QPoint( 0, layerPadding + ( layer - 1 ) *
                ( layerHeight + layerPadding ));

      const float& percPyr =
        layers[ layer ]->getPropertyValue< float >( "leftPerc", .0f );
      const float& percInter =
        layers[ layer ]->getPropertyValue< float >( "rightPerc", .0f );

      auto layerRep = dynamic_cast< LayerRep* >( layers[ layer ] );
      assert( layerRep );
      auto item_ = layerRep->item( scene_ );
      auto layerItem = dynamic_cast< LayerItem* >( item_ );
      assert( layerItem );
      layerItem->create(
        layer,
        collapseButton,
        //this,
        pLayerUL - collapseButtonPos,
        pLayerUM - collapseButtonPos,
        pLayerUR - collapseButtonPos,
        layerHeight, size/40,
        percPyr, percInter,
        QBrush(
          QColor( std::min( baseColor.red( ) + layer * 8, 255 ),
                  std::min( baseColor.green( ) + layer * 8, 255 ),
                  std::min( baseColor.blue( ) + layer * 5 , 255 ))));

      layerItem->setFlag( QGraphicsItem::ItemStacksBehindParent );
      _layerItems[ layer - 1 ] = layerItem;
      _layerAnimations[ layer - 1 ] =
        new QPropertyAnimation( layerItem, "opacity" );

      for ( int i = 0; i < 2; ++i )
      {
        auto neuronTypeAggRep =
          dynamic_cast< NeuronTypeAggregationRep* >(
            neuronTypeAggs[ layer + i * 7 ] );
        assert( neuronTypeAggRep );
        auto item__ = neuronTypeAggRep->item( scene_ );
        auto neuronTypeAggItem =
          dynamic_cast< NeuronTypeAggregationItem* >( item__ );
        assert( neuronTypeAggItem );
        neuronTypeAggItem->create( size / 25 );
        neuronTypeAggItem->setParentItem( layerItem );
        auto item = neuronTypeAggItem->symbolItem( );
        item->setBrush(
          QBrush(
            QColor( std::min( baseColor.red( ) + layer * 8, 255 ),
                    std::min( baseColor.green( ) + layer * 8, 255 ),
                    std::min( baseColor.blue( ) + layer * 5 , 255 ))));

        item->setPen( Qt::NoPen );

        if ( i == 0 )
        {
          item->setX(( pLayerUL - collapseButtonPos ).x( ) - int(size)/20 );
          item->setY(( pLayerUL - collapseButtonPos ).y( ) + int(size)/40 );
        }
        else
        {
          item->setX( -( pLayerUL - collapseButtonPos ).x( ) + int(size)/20 );
          item->setY(( pLayerUL - collapseButtonPos ).y( ) + int(size)/40 );
        }
      }
    }

    for ( int i = 0; i < 2; ++i )
    {
      auto neuronTypeAggRep =
        dynamic_cast< NeuronTypeAggregationRep* >(
          neuronTypeAggs[ i * 7 ] );
      assert( neuronTypeAggRep );
      auto item__ = neuronTypeAggRep->item( scene_ );
      auto neuronTypeAggItem =
        dynamic_cast< NeuronTypeAggregationItem* >( item__ );
      assert( neuronTypeAggItem );
      neuronTypeAggItem->create( size / 15 );
      neuronTypeAggItem->setParentItem( this );
      auto item = neuronTypeAggItem->symbolItem( );
      item->setBrush( QBrush( baseColor ));
      item->setPen( Qt::NoPen );
      if ( i == 0 )
      {
        item->setX( -int(size)/2.5f );
        item->setY( 0 );
      }
      else
      {
        item->setX( int(size)/2.5f );
        item->setY( 0 );
      }
    }

    NEUROSCHEME_DEBUG_CHECK(
      neuronTypeAggs.size( ) == 14,
      "nslib::NeuronAggregationItem: incorrect number"
      "of NeuronTypeAggregations" );

    // for ( int i = 0; i < 14; i++ )
    // {
    //   auto neuronTypeAggRep =
    //     dynamic_cast< NeuronTypeAggregationRep* >( neuronTypeAggs[ i ] );
    //   assert( neuronTypeAggRep );
    //   auto item_ = neuronTypeAggRep->item( scene );
    //   auto neuronTypeAggItem =
    //     dynamic_cast< NeuronTypeAggregationItem* >( item_ );
    //   assert( neuronTypeAggItem );
    //   neuronTypeAggItem->create( );
    //   neuronTypeAggItem->setParentItem( this );
    //   auto item = neuronTypeAggItem->symbolItem( );
    //   item->setBrush( QBrush( baseColor ));
    //   item->setPen( Qt::NoPen );
    //   item->setX( pLayerUL.x( ) - size / 1.5f );
    //   item->setY( pLayerUL.y( ) + size / 2 );
    // }

    this->collapse( false ); // Start collapsed
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

  void NeuronAggregationItem::hoverEnterEvent(
    QGraphicsSceneHoverEvent* event_ )
    {
      auto qGraphicsItemRep =
        dynamic_cast< QGraphicsItemRepresentation* >( _parentRep );
      if ( qGraphicsItemRep )
      {
        for ( auto& item : qGraphicsItemRep->items( ))
        {
          auto qAbstractGraphicItem =
            dynamic_cast< QAbstractGraphicsShapeItem* >( item.second );
          if ( qAbstractGraphicItem )
          {
            InteractionManager::hoverEnterEvent(
              qAbstractGraphicItem, event_ );
          }
        }
      }
    }

    void NeuronAggregationItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* event_ )
    {
      auto qGraphicsItemRep =
        dynamic_cast< QGraphicsItemRepresentation* >( _parentRep );
      if ( qGraphicsItemRep )
      {
        for( auto& item : qGraphicsItemRep->items( ))
        {
          auto qAbstractGraphicItem =
            dynamic_cast< QAbstractGraphicsShapeItem* >( item.second );
          if( qAbstractGraphicItem )
          {
            InteractionManager::hoverLeaveEvent( qAbstractGraphicItem, event_ );
          }
        }
      }
    }

    void NeuronAggregationItem::contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
    {
      InteractionManager::contextMenuEvent( this, event_ );
    }

  } // namespace cortex
} // namespace nslib
