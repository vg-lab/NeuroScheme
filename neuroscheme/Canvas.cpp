#include "Canvas.h"
#include "Log.h"
#include "PaneManager.h"
#include <QHBoxLayout>

namespace neuroscheme
{

  GraphicsView::GraphicsView( QWidget* parent_ )
    : QGraphicsView( parent_ )
  {
  }

  void GraphicsView::mousePressEvent( QMouseEvent* event )
  {
    PaneManager::activePane( dynamic_cast< Canvas* >( this->parentWidget( )));
    QGraphicsView::mousePressEvent( event );
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
      this->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // Don't call superclass handler here
    // as wheel is normally used for moving scrollbars
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
    // std::cout << "Canvas size " << this->width( ) << " x " << this->height( ) << std::endl;
    // std::cout << "Canvas size " << _graphicsView->width( ) << " x " << _graphicsView->height( ) << std::endl;
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
    neuroscheme::PaneManager::activePane( this );
  }

  void Canvas::enterEvent( QEvent*  )
  {
    // neuroscheme::PaneManager::activePane( this );
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
      PaneManager::newPane( this, PaneManager::HORIZONTAL );
      break;
    case Qt::Key_V:
      PaneManager::newPane( this, PaneManager::VERTICAL );
      break;
    default:
      break;
    }
  }

  void Canvas::resizeEvent( QResizeEvent* )
  {
    // std::cout << "Resize Canvas" << std::endl;
    const QSize viewerSize = this->view( ).size( );
    const QRectF rectf = QRectF( - viewerSize.width( ) / 2,
                                 - viewerSize.height( ) / 2,
                                 viewerSize.width( ) -2,
                                 viewerSize.height( ) -2);

    QTransform transf = this->view( ).transform( );
    this->view( ).fitInView( rectf, Qt::KeepAspectRatio );
    this->view( ).setSceneRect( rectf );
    this->view( ).setTransform( transf );

    // std::cout << this->view( ).width( ) << " x "
    //           << this->view( ).height( ) << std::endl;

    this->layouts( ).getLayout(
      this->activeLayoutIndex( ))->refresh( false, false );
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
    // std::cout << "layout changed in " << this
    //           << " to index " << index << std::endl;
    if ( _layouts.getLayout( index ))
    {
      _activeLayoutIndex = index;
      // std::cout << _layouts.getLayout( index )->name( ) << std::endl;
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

        layout_->addWidget(
          _layouts.getLayout( index )->optionsWidget( ),
          2, 0 );
        _layouts.getLayout( index )->optionsWidget( )->show( );
        //displayReps( _reps, true );
        displayEntities( _entities, true, false );
      }
    }
  }

  void Canvas::displayEntities( const shift::Entities& entities_,
                                bool animate, bool refreshProperties )
  {
    _entities = entities_;
    assert( _layouts.getLayout( _activeLayoutIndex ));
    _layouts.getLayout( _activeLayoutIndex )->displayEntities(
      entities_, animate, refreshProperties );

    if ( refreshProperties )
    {
      for ( auto& layout_ : layouts( ).map( ))
      {
        if ( layout_.second == layouts( ).getLayout( activeLayoutIndex( )))
          continue;
        layout_.second->refreshProperties(
          layouts( ).getLayout(
            activeLayoutIndex( ))->representations( ));
      }
    }
  }

  // void Canvas::displayReps( shift::Representations& reps_, bool animate )
  // {
  //   _reps = reps_;
  //   assert( _layouts.getLayout( _activeLayoutIndex ));
  //   _layouts.getLayout( _activeLayoutIndex )->displayItems(
  //     reps_, animate );
  // }

  void Canvas::addLayout( Layout* layout_ )
  {
    _layouts.addLayout( layout_ );
    layout_->scene( _graphicsScene );
  }

  Canvas* Canvas::clone( void ) const
  {
    auto canvas = new Canvas( );
    assert( canvas->scene( ).views( ).size( ) != 0 );
    for ( auto layout_ : _layouts.map( ))
    {
      auto newLayout = layout_.second->clone( );
      newLayout->setRepresentations( layout_.second->representations( ));
      newLayout->refreshProperties( layout_.second->representations( ));
      canvas->addLayout( newLayout );
      // std::cout << "clone layout" << layout_.second->name( ) << std::endl;
    }
    canvas->activeLayoutIndex( this->_activeLayoutIndex );
//    std::cout << "setting active layout " << this->_activeLayoutIndex << std::endl;
    canvas->displayEntities( _entities, false, true );
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


} // namespace neuroscheme
