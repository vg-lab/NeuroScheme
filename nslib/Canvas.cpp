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
#include "Canvas.h"
#include "DataManager.h"
#include "DomainManager.h"
#include "InteractionManager.h"
#include "Loggers.h"
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"
#include "reps/Item.h"
#include <QHBoxLayout>
#include <nslib/layouts/GridLayout.h>
#include <nslib/layouts/CircularLayout.h>
#include <nslib/layouts/CameraBasedLayout.h>
#include <nslib/layouts/ScatterPlotLayout.h>

namespace nslib
{
  // NOTE: compilers warn if you turn this into constexpr because of definition in GraphicsView.
  const double GraphicsView::scaleFactor = 1.04;
  const double GraphicsView::scaleFactorInv = 1.0 / 1.04;

  GraphicsView::GraphicsView( QWidget* parent_ )
    : QGraphicsView( parent_ )
  {}

  void GraphicsView::mousePressEvent( QMouseEvent* event_ )
  {
    if ( this->parentWidget( ) )
    {
      auto canvas = dynamic_cast< Canvas* >( this->parentWidget( ));
      if(canvas)
      {
        PaneManager::activePane(canvas);
      }
    }

    auto item = itemAt( event_->pos( ));
    InteractionManager::mousePressEvent( this, item, event_ );

    QGraphicsView::mousePressEvent( event_ );
  }

  void GraphicsView::mouseReleaseEvent( QMouseEvent* event_ )
  {
    auto item = itemAt( event_->pos( ));
    InteractionManager::mouseReleaseEvent( item, event_ );

    QGraphicsView::mouseReleaseEvent( event_ );
  }

  void GraphicsView::mouseMoveEvent( QMouseEvent* event_ )
  {
    QGraphicsView::mouseMoveEvent( event_ );

    auto item = itemAt( event_->pos( ));
    auto shapeItem = dynamic_cast< QAbstractGraphicsShapeItem* >( item );
    InteractionManager::mouseMoveEvent( this, shapeItem, event_ );
  }

  void GraphicsView::wheelEvent( QWheelEvent* event_ )
  {
    setTransformationAnchor( QGraphicsView::AnchorUnderMouse );

    // Scale the view / do the zoom
    const int delta = event_->angleDelta( ).y( );

    if ( delta > 0 )
    {
      // Zoom in
      this->scale( scaleFactor, scaleFactor );
    }
    else if ( delta < 0 )
    {
      // Zooming out
      this->scale( scaleFactorInv, scaleFactorInv );
    }
    else
    {
      // sceneRect should be the same
      return;
    }

    auto sceneRect = this->scene( )->itemsBoundingRect( );
    this->setSceneRect( sceneRect );

    // Don't call superclass handler here
    // as wheel is normally used for moving scrollbars
  }

  void GraphicsScene::contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
  {
    const QPointF mousePoint = event_->scenePos( );
    // If not clicked in an item
    if ( !this->itemAt( mousePoint, QTransform( )))
    {
      InteractionManager::contextMenuEvent( nullptr, event_ );
    }
    else
      QGraphicsScene::contextMenuEvent( event_ );
  }

  Canvas::Canvas( QWidget * parent_ )
    : _graphicsView( new GraphicsView( parent_ ))
    , _graphicsScene( new GraphicsScene )
    , _activeLayoutIndex( -1 )
    , _repsScale( 1.0f )
  {
    _graphicsScene->setParent( this );
    _graphicsView->setScene( _graphicsScene );
    _graphicsView->setInteractive( true );
    _graphicsView->setMouseTracking( true );
    _graphicsView->viewport( )->setMouseTracking( true );
    _graphicsView->setRenderHints( QPainter::Antialiasing |
                                   QPainter::HighQualityAntialiasing );
    _graphicsView->setDragMode( QGraphicsView::ScrollHandDrag );
    _graphicsView->setSizePolicy( QSizePolicy::Expanding,
                                  QSizePolicy::Expanding );
    _graphicsView->setSizePolicy( QSizePolicy::Expanding,
                                  QSizePolicy::Expanding );
    _graphicsView->show( );
    _graphicsView->resize( this->size( ));
    QHBoxLayout* layout_ = new QHBoxLayout;
    layout_->addWidget( _graphicsView );
    this->QWidget::setLayout( layout_ );
    this->leaveEvent( nullptr );
    this->setObjectName( "pane" );

    this->activeLayoutIndex( Layout::TLayoutIndexes::GRID );
    this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    this->setLayout( Layout::TLayoutIndexes::GRID, new GridLayout( ));
    this->setLayout( Layout::TLayoutIndexes::CAMERA, new CameraBasedLayout( ),
      DomainManager::getActiveDomain( )->domainName( ) == "congen" );
    this->setLayout( Layout::TLayoutIndexes::SCATTER, new ScatterPlotLayout( ));
    this->setLayout( Layout::TLayoutIndexes::CIRCULAR, new CircularLayout( ));
    this->setLayout( Layout::TLayoutIndexes::FREE,
      new FreeLayout( InteractionManager::statusBar( )));
  }

  Canvas::~Canvas( void )
  {
    if ( _graphicsView )
    {
      delete _graphicsView;
    }
    if ( _graphicsScene )
    {
      for( auto rep : _reps )
      {
        auto graphicsItemRep =
          dynamic_cast< QGraphicsItemRepresentation* >( rep );
        graphicsItemRep->deleteItem( _graphicsScene );
        graphicsItemRep->items( ).erase( _graphicsScene );
      }
      delete _graphicsScene;
    }
  }

  void Canvas::connectLayoutSelector( void )
  {
    connect( this->layouts( ).layoutSelector( ), SIGNAL( currentIndexChanged( int )),
             this, SLOT( layoutChanged( int )));
  }

  const GraphicsScene& Canvas::scene( void ) const
  {
    return *_graphicsScene;
  }

  GraphicsScene& Canvas::scene( void )
  {
    return *_graphicsScene;
  }

  const GraphicsView& Canvas::view( void ) const
  {
    return *_graphicsView;
  }

  GraphicsView& Canvas::view( void )
  {
    return *_graphicsView;
  }

  void Canvas::mousePressEvent( QMouseEvent* /* event */ )
  {
    nslib::PaneManager::activePane( this );
  }

  void Canvas::enterEvent( QEvent*  )
  {
    // nslib::PaneManager::activePane( this );
    // this->setObjectName("pane");
    // this->setStyleSheet("#pane { border: 3px dotted rgba( 0,0,0,15%); }");
  }

  void Canvas::leaveEvent( QEvent*  )
  {
    // this->setObjectName("pane");
    // this->setStyleSheet("#pane { border: 3px solid rgba( 0,0,0,0%); }");
  }

  void Canvas::keyPressEvent( QKeyEvent *event_ )
  {
    switch( event_->key( ))
    {
    case Qt::Key_H:
      PaneManager::paneDivision( PaneManager::HORIZONTAL );
      PaneManager::newPane( this );
      break;
    case Qt::Key_V:
      PaneManager::paneDivision( PaneManager::VERTICAL );
      PaneManager::newPane( this );
      break;
    case Qt::Key_K:
      PaneManager::killPane( this );
      break;
    default:
      break;
    }
  }

  void Canvas::resizeEvent( QResizeEvent* )
  {
    Loggers::get( )->log( "Canvas resize event",
                         LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );
    const QSize viewerSize = this->view( ).size( );
    const QRectF rectf = QRectF( - viewerSize.width( ) / 2,
                                 - viewerSize.height( ) / 2,
                                 viewerSize.width( ) -2,
                                 viewerSize.height( ) -2);

    const auto transf = this->view( ).transform( );
    this->view( ).fitInView( rectf, Qt::KeepAspectRatio );
    this->view( ).setSceneRect( rectf );
    this->view( ).setTransform( transf );

    auto activeLayoutIndex_ =
      this->layouts( ).getLayout( this->activeLayoutIndex( ));
    if ( activeLayoutIndex_ )
      activeLayoutIndex_->refresh( false );
  }

  void Canvas::showEvent( QShowEvent* /* event_ */ )
  {
    resizeEvent( nullptr );
  }

  const Layouts& Canvas::layouts( void ) const
  {
    return _layouts;
  }
  Layouts& Canvas::layouts( void )
  {
    return _layouts;
  }

  int Canvas::activeLayoutIndex( void ) const
  {
    return _activeLayoutIndex;
  }

  void Canvas::activeLayoutIndex( int activeLayoutIndex_ )
  {
    _activeLayoutIndex = activeLayoutIndex_;
  }

  void Canvas::layoutChanged( int index )
  {
    if ( index <= Layout::TLayoutIndexes::UNDEFINED )
    {
      Loggers::get( )->log( "Trying to change to a layout with negative index",
        LOG_LEVEL_WARNING );

      return;
    }

    if ( index == _activeLayoutIndex )
    {
      Loggers::get( )->log( "Trying to change to a layout already in use",
        LOG_LEVEL_VERBOSE );
      return;
    }

    Loggers::get( )->log( "Layout changed to " + std::to_string( index ),
      LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

    if ( _layouts.getLayout( index ))
    {
      _activeLayoutIndex = index;
      auto actualLayout = _layouts.getLayout( index );
      auto actualOptions = actualLayout->optionsWidget( );
      _layouts.layoutSelector( )->setCurrentIndex( index );
      auto layout_ = PaneManager::layout( );
      if ( layout_ )
      {
        if( _activeLayoutIndex == Layout::TLayoutIndexes::FREE )
        {
          dynamic_cast< FreeLayout* >( actualLayout )->init( );
        }

        auto item = layout_->itemAtPosition( 2, 0 );
        if ( item )
        {
          auto widget = item->widget( );
          if ( widget )
          {
            auto index_ = layout_->indexOf( widget );
            if ( index_ != -1 )
            {
              layout_->takeAt( index_ );
              widget->hide( );
            }
          }
        }

        layout_->addWidget( actualOptions, 2, 0 );
        displayEntities( true, false );
        actualOptions->show( );

      }
      else
        Loggers::get( )->log( "Null pane layout",
          LOG_LEVEL_WARNING, NEUROSCHEME_FILE_LINE );
    }
    else
      Loggers::get( )->log( "Null layout with index" + std::to_string( index ),
        LOG_LEVEL_WARNING, NEUROSCHEME_FILE_LINE );
  }

  void Canvas::displayEntities( bool animate, bool refreshProperties_ )
  {
    nslib::Loggers::get( )->log(
      "displayEntities " + std::to_string( _entities.size( )),
      nslib::LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

    if ( refreshProperties_ )
    {
      refreshProperties( );
      for ( auto& layout_ : layouts( ).map( ))
      {
        layout_.second->refreshWidgetsProperties( _properties );
      }
    }

    assert( _layouts.getLayout( _activeLayoutIndex ));
    _layouts.getLayout( _activeLayoutIndex )->display( _entities,
      _reps, animate );
  }

  void Canvas::setEntities( shift::Entities& entities_)
  {
    _sceneEntities = _entities = entities_;
    if ( Config::showNoHierarchyEntities( ))
    {
      _entities.addEntities( DataManager::noHierarchyEntities( ));
    }
  }

  void Canvas::displayEntities( shift::Entities& entities_,
    bool animate, bool refreshProperties_ )
  {
    setEntities( entities_ );
    displayEntities( animate, refreshProperties_ );
  }

  void Canvas::setLayout(
    unsigned int layoutIndex_, Layout* layout_, bool disabled_ )
  {
    Loggers::get( )->log( "Add layout", LOG_LEVEL_VERBOSE, NEUROSCHEME_FILE_LINE );

    _layouts.setLayout( layoutIndex_, layout_, disabled_ );
    layout_->canvas( this );
  }

  Canvas* Canvas::clone( void )
  {
    auto canvas = new Canvas( );
    canvas->_properties = this->_properties;
    canvas->_reps = this->_reps;
    canvas->_entities = this->_entities;
    canvas->_sceneEntities = this->_sceneEntities;
    canvas->_repsScale = this->_repsScale;
    assert( canvas->scene( ).views( ).size( ) != 0 );

    canvas->activeLayoutIndex( this->_activeLayoutIndex );
    switch ( this->_activeLayoutIndex )
    {
      case Layout::TLayoutIndexes::FREE:
        {
          FreeLayout* origFreeLayout = dynamic_cast< FreeLayout* >(
            this->layouts( ).getLayout( Layout::TLayoutIndexes::FREE ));
          FreeLayout* freeLayout = dynamic_cast< FreeLayout* >(
            canvas->layouts( ).getLayout( Layout::TLayoutIndexes::FREE ));
          freeLayout->init( );
          bool moveNewEntities = origFreeLayout->moveNewEntitiesChecked( );
          freeLayout->moveNewEntitiesChecked( false );
          for( shift::Representation* rep : this->_reps )
          {
            auto graphicsItemRep =
              dynamic_cast< QGraphicsItemRepresentation* >( rep );
            if( graphicsItemRep )
            {
              auto oldItem = graphicsItemRep->item( &this->scene( ));
              auto item = graphicsItemRep->item( &canvas->scene( ));
              canvas->scene( ).addItem( item );
              item->setScale( canvas->_repsScale );
              item->setPos( oldItem->pos( ));
            }
          }
          canvas->displayEntities( false, true );
          freeLayout->moveNewEntitiesChecked( moveNewEntities );
        }
        break;
      case Layout::TLayoutIndexes::CIRCULAR:
        {
          CircularLayout* origCricularLayout = dynamic_cast< CircularLayout* >(
            this->layouts( ).getLayout( Layout::TLayoutIndexes::CIRCULAR ));
          CircularLayout* circularLayout = dynamic_cast< CircularLayout* >(
            canvas->layouts( ).getLayout( Layout::TLayoutIndexes::CIRCULAR ));
          circularLayout->radius( origCricularLayout->radius( ));
        }
        break;
      case Layout::TLayoutIndexes::GRID:
        {
          GridLayout* origGridLayout = dynamic_cast< GridLayout* >(
            this->layouts( ).getLayout( Layout::TLayoutIndexes::GRID ));
          GridLayout* gridLayout = dynamic_cast< GridLayout* >(
            canvas->layouts( ).getLayout( Layout::TLayoutIndexes::GRID ));
          gridLayout->padding( origGridLayout->paddingX( ),
            origGridLayout->paddingY( ));
        }
        break;
      default:
        {
          canvas->displayEntities( false, true );
        }
        break;
    }

    canvas->layouts( ).layoutSelector( )->setCurrentIndex(
      this->_activeLayoutIndex );
    canvas->connectLayoutSelector( );
    return canvas;
  }

  const shift::Representations& Canvas::reps( void ) const
  {
    return _reps;
  }
  shift::Representations& Canvas::reps( void )
  {
    return _reps;
  }

  void Canvas::refreshProperties( void )
  {
    fires::Objects objs;
    fires::Objects filteredObjs;

    _properties.clear( );

    for ( const auto& entity : _entities.vector( ))
    {
      if ( DataManager::entities( ).
           relationships( ).count( "isSubEntityOf" ) != 0 )
      {
        const auto& relSubEntityOf =
          *( DataManager::entities( ).
             relationships( )[ "isSubEntityOf" ]->asOneToOne( ));
        if ( relSubEntityOf.count( entity->entityGid( )) == 0 )
        {
          // If not subentity add it
          objs.add( entity );
        }
        else
        {
          continue;
        }
      }
      else // if the realtionship does not exist
      {
        objs.push_back( entity );
      }

      for ( const auto& propertyGid_ : entity->properties( ))
      {
        const auto& prop = fires::PropertyGIDsManager::getPropertyLabel(
          propertyGid_.first );
        if ( fires::PropertyManager::getAggregator( propertyGid_.first ))
        {
          if ( _properties.find( prop ) == _properties.end( ))
            _properties[ prop  ] = TPropertyData{ 0, 0 };
        }
      }
    }

    fires::AggregateConfig aggregateConfigMax;
    fires::AggregateConfig aggregateConfigMin;

    for ( const auto& p : _properties )
    {
      aggregateConfigMax.addProperty(
        p.first,
        fires::PropertyManager::getAggregator( p.first ),
        fires::PropertyAggregator::MAX );
      aggregateConfigMin.addProperty(
        p.first,
        fires::PropertyManager::getAggregator( p.first ),
        fires::PropertyAggregator::MIN );
    }

    fires::Aggregate aggregate;
    fires::Objects aggregatedMinObjects = objs;
    aggregate.eval( aggregatedMinObjects, aggregateConfigMin );
    fires::Object* aggregatedMinObj = aggregatedMinObjects[ 0 ];

    fires::Objects aggregatedMaxObjects = objs;
    aggregate.eval( aggregatedMaxObjects, aggregateConfigMax );
    fires::Object* aggregatedMaxObj = aggregatedMaxObjects[ 0 ];

    for ( auto& p : _properties )
    {
      const std::string& label = p.first;
      p.second = TPropertyData
        {
          fires::PropertyManager::getPropertyCaster( label )->toInt(
            aggregatedMinObj->getProperty( label ),
            fires::PropertyCaster::FLOOR ),
          fires::PropertyManager::getPropertyCaster( label )->toInt(
            aggregatedMaxObj->getProperty( label ),
            fires::PropertyCaster::CEIL )
        };
    }
  }

  void Canvas::addEntity( shift::Entity* entity_, const bool isInput_  )
  {
    if( !isInput_)
    {
      _sceneEntities.add( entity_ );
    }
    _entities.add( entity_ );
  }

  void Canvas::removeEntity( const shift::Entity* entity_, const bool isInput_  )
  {
    if( !isInput_)
    {
      _sceneEntities.removeIfContains( entity_ );
    }
    _entities.removeIfContains( entity_ );
  }

  qreal Canvas::repsScale( ) const
  {
    return _repsScale;
  }

  void Canvas::repsScale( const qreal repsScale_ )
  {
    _repsScale = repsScale_;
  }

} // namespace nslib
