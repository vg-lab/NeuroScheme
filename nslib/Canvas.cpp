/*
 * Copyright (c) 2016 GMRV/URJC/UPM.
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
#include "Log.h"
#include "PaneManager.h"
#include "RepresentationCreatorManager.h"
#include "reps/Item.h"
#include <QHBoxLayout>

namespace nslib
{

  GraphicsView::GraphicsView( QWidget* parent_ )
    : QGraphicsView( parent_ )
  {
  }

  void GraphicsView::mousePressEvent( QMouseEvent* event_ )
  {
    if ( this->parentWidget( ) &&
         dynamic_cast< Canvas* >( this->parentWidget( )))
      PaneManager::activePane(
        dynamic_cast< Canvas* >( this->parentWidget( )));

    auto item = itemAt( event_->pos( ));
    if ( item )
      InteractionManager::mousePressEvent( item, event_ );
    else
      InteractionManager::mousePressEvent( nullptr, event_ );

    QGraphicsView::mousePressEvent( event_ );
  }

  void GraphicsView::mouseReleaseEvent( QMouseEvent* event_ )
  {
    auto item = itemAt( event_->pos( ));
    if ( item )
      InteractionManager::mouseReleaseEvent( item, event_ );
    else
      InteractionManager::mouseReleaseEvent( nullptr, event_ );

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
    double scaleFactor = 1.05;
    int delta = event_->angleDelta( ).y( );

    if ( delta > 0 )
    {
      // Zoom in
      scale(scaleFactor, scaleFactor);
    }
    else if ( delta < 0 )
    {
      // Zooming out
      this->scale( 1.0f / scaleFactor, 1.0f / scaleFactor );
    }

    // Don't call superclass handler here
    // as wheel is normally used for moving scrollbars
  }

  void GraphicsScene::contextMenuEvent( QGraphicsSceneContextMenuEvent* event_ )
  {
    QPointF mousePoint = event_->scenePos( );
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
    this->setLayout( layout_ );
    this->leaveEvent( nullptr );
    this->setObjectName( "pane" );
  }

  Canvas::~Canvas( void )
  {
    if ( _graphicsView ) delete _graphicsView;
    if ( _graphicsScene ) delete _graphicsScene;
  }


  void Canvas::connectLayoutSelector( void )
  {
    connect( this->layouts( ).layoutSelector( ),
             SIGNAL( currentIndexChanged( int )),
             this,
             SLOT( layoutChanged( int )));

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
    Log::log( NS_LOG_HEADER, LOG_LEVEL_VERBOSE );
    const QSize viewerSize = this->view( ).size( );
    const QRectF rectf = QRectF( - viewerSize.width( ) / 2,
                                 - viewerSize.height( ) / 2,
                                 viewerSize.width( ) -2,
                                 viewerSize.height( ) -2);

    QTransform transf = this->view( ).transform( );
    this->view( ).fitInView( rectf, Qt::KeepAspectRatio );
    this->view( ).setSceneRect( rectf );
    this->view( ).setTransform( transf );

    this->layouts( ).getLayout(
      this->activeLayoutIndex( ))->refresh( false );
  }

  void Canvas::showEvent( QShowEvent* /* event_ */ )
  {
    resizeEvent( 0 );
  }


  const Layouts& Canvas::layouts( void ) const
  {
    return _layouts;
  }
  Layouts& Canvas::layouts( void )
  {
    return _layouts;
  }

  int Canvas::activeLayoutIndex( void )
  {
    return _activeLayoutIndex;
  }

  void Canvas::activeLayoutIndex( int activeLayoutIndex_ )
  {
    _activeLayoutIndex = activeLayoutIndex_;
  }

  void Canvas::layoutChanged( int index )
  {
    if ( index == -1 )
    {
      Log::log( NS_LOG_HEADER +
                " trying to change to a layout with -1 index",
                LOG_LEVEL_WARNING );

      return;
    }
    Log::log( NS_LOG_HEADER + "layout changed to " + std::to_string( index ),
              LOG_LEVEL_VERBOSE );
    if ( _layouts.getLayout( index ))
    {
      _activeLayoutIndex = index;
      _layouts.getLayout( index )->optionsWidget( );
      auto layout_ = PaneManager::layout( );
      if ( layout_ )
      {
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

        layout_->addWidget( _layouts.getLayout( index )->optionsWidget( ),
                            2, 0 );
        displayEntities( _entities, true, false );
        _layouts.getLayout( index )->optionsWidget( )->show( );

      }
      else
        Log::log( NS_LOG_HEADER + " null pane layout",
                  LOG_LEVEL_WARNING );
    }
    else
      Log::log( NS_LOG_HEADER +
                " null layout with index" + std::to_string( index ),
                LOG_LEVEL_WARNING );

  }

  void Canvas::displayEntities( shift::Entities& entities_,
                                bool animate, bool refreshProperties_ )
  {
    nslib::Log::log( NS_LOG_HEADER + "displayEntities " +
                     std::to_string( entities_.size( )),
                           nslib::LOG_LEVEL_VERBOSE );

    if ( refreshProperties_ )
    {
      refreshProperties( entities_ );
      for ( auto& layout_ : layouts( ).map( ))
      {
        layout_.second->refreshWidgetsProperties( _properties );
      }
    }

    _entities = entities_;
    assert( _layouts.getLayout( _activeLayoutIndex ));
    _layouts.getLayout( _activeLayoutIndex )->display( entities_,
                                                       _reps,
                                                       animate );

  }

  void Canvas::addLayout( Layout* layout_ )
  {
    Log::log( NS_LOG_HEADER, LOG_LEVEL_VERBOSE );

    _layouts.addLayout( layout_ );
    layout_->canvas( this );
  }

  Canvas* Canvas::clone( void ) const
  {
    auto canvas = new Canvas( );
    canvas->_properties = _properties;
    canvas->_reps = _reps;
    canvas->_entities = _entities;
    assert( canvas->scene( ).views( ).size( ) != 0 );
    for ( auto layout_ : _layouts.map( ))
    {
      auto newLayout = layout_.second->clone( );
      newLayout->refreshWidgetsProperties( _properties );
      canvas->addLayout( newLayout );
    }
    canvas->activeLayoutIndex( this->_activeLayoutIndex );
    canvas->displayEntities( canvas->_entities, false, true );
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

  void Canvas::refreshProperties(
    const shift::Entities& entities_ )
  {
    fires::Objects objs;
    fires::Objects filteredObjs;

    _properties.clear( );

    for ( const auto& entity : entities_.vector( ))
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


} // namespace nslib
